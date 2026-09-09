#include "uart_true.h"
#include "uart_base.h"
#include "zephyr/drivers/uart.h"
#include "zephyr/kernel.h"
#include "zephyr/sys/ring_buffer.h"
#include "zephyr/toolchain.h"
#include <stdbool.h>
#include <stdint.h>
#include <sys/errno.h>

extern struct k_msgq uart_rx_queue;   // 引用 main.c 中的队列

//因为是一个一个取的其实更本用不到，每次触发中断1字节必定进行运输完成了
static int s_uart_tx_it(struct uart_base_t* base,uint8_t *data,uint32_t len32)
{
    struct uart_device_t *me = CONTAINER_OF(base, struct uart_device_t, base); 
    ring_buf_put(&me->tx_ring, data, len32);
    if (!me->is_busy_b) {
        me->is_busy_b = true;
        uart_irq_tx_enable(me->uart_device);
    }
    return 0;
}

//通过引入不同的回调，可以对it，和dma产生只需要一次启动即可，就像zephyr一样
static int s_uart_rx_enalbe_it(struct uart_base_t* base,uint32_t timeout)
{
    struct uart_device_t* me = CONTAINER_OF(base, struct uart_device_t, base);
    ARG_UNUSED(timeout);
    uart_irq_rx_enable(me->uart_device);
    return 0;
}

static int s_uart_rx_analyze(uart_base_t* base)
{
    struct uart_device_t* me = CONTAINER_OF(base, struct uart_device_t, base);  

    if (me->callback == NULL) {
        return -EINVAL;
    }

    uint8_t data[100];
    
    while (!ring_buf_is_empty(&me->rx_ring)) {
        uint32_t len = ring_buf_get(&me->rx_ring,data,sizeof(data));
        me->callback(data,len,me->user_data);
    };
    return 0;
}    
    
static int s_uart_callback_register(uart_base_t* base,uart_user_cb_t callback,void* user_data)
{
    struct uart_device_t* me = CONTAINER_OF(base, struct uart_device_t, base);  
    me->user_data = user_data;
    me->callback = callback;
    return 0;
}



//这里和当时stm32裸机freertos最大的不同就是，zephyr中不能开启，串口空闲中断（stm32独特）
//因此不好设置事件驱动线程，只能将整个中断回调作为tx_callback了
static void s_uart_isr(const struct device *dev, void *user_data)
{
     struct uart_device_t* me = (struct uart_device_t* )user_data;

     uart_irq_update(dev);

    if(uart_irq_tx_ready(dev))
    {
      /* 发一次 TX_DONE 后立即关闭 TX 中断, 防止在 FIFO 有空间期间
       * ISR 被反复触发把 uart_tx_queue 刷满.
       * 线程处理完 TX_DONE 后若 ring_buf 还有数据会再 uart_irq_tx_enable(). */
      uint8_t byte;
      if (ring_buf_get(&me->tx_ring, &byte, 1) == 1) {
        uart_fifo_fill(dev, &byte, 1);
      } 
      else {
        uart_irq_tx_disable(dev);
        me->is_busy_b = false;
      }
    }

    if (uart_irq_rx_ready(dev)) {
      int len;
      while ((len = uart_fifo_read(dev, me->rx_data, me->rx_len32)) > 0) {
        /* 只写入实际读取的字节数, 避免写入垃圾数据导致 ring buffer 损坏 */
        if (ring_buf_put(&me->rx_ring, me->rx_data, len) != len) {
          break;
        }
      }

    //   struct uart_event_t event = {
    //       .base = &me->base,
    //       .type_e = UART_EVENT_RX_DATA,
    //   };
    //   k_msgq_put(&uart_rx_queue, &event, K_NO_WAIT);
    }
}




static int s_uart_tx_dma(uart_base_t* base,uint8_t* data ,uint32_t len32)
{
    struct uart_device_t* me = CONTAINER_OF(base, struct uart_device_t, base);
    if (!me->uart_device || !data || len32 == 0) {
        return  -EINVAL;
    }

    ring_buf_put(&me->tx_ring, data, len32);

    if (!me->is_busy_b) {
        me->is_busy_b = true;

        int32_t len = ring_buf_get(&me->tx_ring, me->tx_data, me->tx_len32);
        int ret = uart_tx(me->uart_device, me->tx_data, len, SYS_FOREVER_US);

        if (ret != 0) {
            me->is_busy_b = false;
            return -EIO;
        }
    }
    // 如果 is_busy_b == true，DMA 正在发送，数据已存入 ring buffer
    // 等 UART_TX_DONE 时 uart_isr_dma 会取 ring buffer 继续发
    
    return 0;
}


static int s_uart_rx_enable_dma(uart_base_t* base,uint32_t timeout_ul)
{
    struct uart_device_t* me = CONTAINER_OF(base, struct uart_device_t, base);

    if (!me->uart_device) {
        return -ENODEV;
    }
    int ret = uart_rx_enable(me->uart_device, me->rx_data, me->rx_len32,timeout_ul);
    if (ret != 0) {
        return ret;
    }

    return 0;
}

static void s_uart_isr_dma(const struct device* dev,struct uart_event* evt,void* user_data)
{
    struct uart_device_t *me = (struct uart_device_t* )user_data;
    
    switch (evt->type) {
        case UART_TX_DONE:
        //缓冲发送完成，
            if (!ring_buf_is_empty(&me->tx_ring)) {
            
                uint32_t len = ring_buf_get(&me->tx_ring, me->tx_data, me->tx_len32);
                int ret = uart_tx(dev,me->tx_data,len,SYS_FOREVER_US);
                if (ret != 0) {
                    me->is_busy_b = false; //启动失败，标记空闲
                }
            }
            else 
            {
                me->is_busy_b = false;      //无后续数据，标记空闲
            }
            break;
        case UART_TX_ABORTED:
            me->is_busy_b = false;
            break;
        case UART_RX_RDY:
            ring_buf_put(&me->rx_ring, &evt->data.rx.buf[evt->data.rx.offset], evt->data.rx.len);
            struct uart_event_t event = {
                .base = &me->base,
                .type_e = UART_EVENT_RX_DATA,
            };
            k_msgq_put(&uart_rx_queue, &event, K_NO_WAIT);
            break;
        case UART_RX_BUF_REQUEST:
            uart_rx_buf_rsp(dev, me->rx_data,me->rx_len32);           //重新以当前rx_hw_buf为起点开启吗？也就是类似之前的重新开启it吗？
            break;
        case UART_RX_BUF_RELEASED:                                                   //IT模式下的释放不需要干任何事情
            break;
        default:
            break;
    }
}

const uart_ops_t uart_ops_it = {
    .uart_rx_enable = s_uart_rx_enalbe_it,
    .uart_transmit = s_uart_tx_it,
    .uart_register_callback = s_uart_callback_register,
    .uart_rx_analyze = s_uart_rx_analyze,
};

static const uart_ops_t uart_ops_dma = {
    .uart_transmit = s_uart_tx_dma,
    .uart_rx_enable = s_uart_rx_enable_dma,
    .uart_register_callback = s_uart_callback_register,   // 使用static本地函数，避免名字冲突导致递归
    .uart_rx_analyze = s_uart_rx_analyze,                 // 同上，复用已有的static函数
};

int uart_it_init(struct uart_device_t* me,const struct uart_cfg_t* cfg, const char *name)
{
    if (!me || !cfg->rx_data || !cfg->tx_data || !cfg->rx_ring_data || !cfg->tx_ring_data) {
        return -EINVAL;
    }
    me->base.name = name;
    me->base.ops = &uart_ops_it;
    me->uart_device = cfg->uart_device;
    
    ring_buf_init(&me->rx_ring,cfg->rx_ring_len32,cfg->rx_ring_data);
    ring_buf_init(&me->tx_ring,cfg->tx_ring_len32,cfg->tx_ring_data);

    me->rx_data = cfg->rx_data;
    me->tx_data = cfg->tx_data;

    me->tx_len32 = cfg->tx_len32;
    me->rx_len32 = cfg->rx_len32;

    return uart_irq_callback_user_data_set(me->uart_device, s_uart_isr,me);
}

int uart_dma_init(struct uart_device_t* me,const struct uart_cfg_t* cfg, const char *name)
{
    if (!me || !cfg) {
        return  -EINVAL;
    }
    me->base.name = name;
    me->base.ops = &uart_ops_dma;
    me->uart_device = cfg->uart_device;
    
    ring_buf_init(&me->rx_ring,cfg->rx_ring_len32,cfg->rx_ring_data);
    ring_buf_init(&me->tx_ring,cfg->tx_ring_len32,cfg->tx_ring_data);

    me->rx_data = cfg->rx_data;
    me->tx_data = cfg->tx_data;

    me->tx_len32 = cfg->tx_len32;
    me->rx_len32 = cfg->rx_len32;
    
    return uart_callback_set(me->uart_device, s_uart_isr_dma,me);
}