#include "encode.h"
#include "menu/menu.h"
#include "motor/tb6612.h"
#include "simulink/ARMCortex-M/car_2026/car_2026.h"
#include "uart/uart_base.h"
#include "uart/uarts.h"
#include "zephyr/drivers/counter.h"
#include "zephyr/kernel.h"
#include <stdint.h>
#include <stdio.h>

K_MSGQ_DEFINE(uart_rx_queue, sizeof(struct uart_event_t), 30,4);


static K_THREAD_STACK_DEFINE(s_stack_1ms_high, 4096); /* 1.5KB */
static K_THREAD_STACK_DEFINE(s_stack_5ms_high, 4096); /* 1.5KB */
static K_THREAD_STACK_DEFINE(s_stack_20ms_high, 4096); /* 1.5KB */
static K_THREAD_STACK_DEFINE(s_stack_task_rx, 4096); /* 1.5KB */

static struct k_thread s_thread_1ms_high;
static struct k_thread s_thread_5ms_high;
static struct k_thread s_thread_20ms_high;
static struct k_thread s_thread_task_rx;


static K_SEM_DEFINE(tim5_sem, 0, 1);

static const struct device *tim5_dev_pst;


static void my_1ms_isr(const struct device *dev,void* user_data)
{
	k_sem_give(&tim5_sem);
}

int tim5_1ms_init(void)
{
	int ret;
    struct counter_top_cfg top_cfg;
    uint32_t freq;
    uint32_t ticks;

    /* 1. 获取设备 */
    tim5_dev_pst = DEVICE_DT_GET(DT_ALIAS(tim5));
    
    /* 2. 检查设备是否就绪 —— 这是必须的！ */
    if (!device_is_ready(tim5_dev_pst)) {
        printk("TIM5 device not ready!\n");
        return -ENODEV;
    }

	freq = counter_get_frequency(tim5_dev_pst);
	printk("TIM5 frequency: %u Hz\n", freq);

	ticks = counter_us_to_ticks(tim5_dev_pst, 1000);  /* 1000us = 1ms */
    printk("TIM5 1ms ticks: %u\n", ticks);

    /* 5. 配置 top value（非 const，因为 ticks 是动态计算的） */

    top_cfg.ticks = ticks;
    top_cfg.callback = my_1ms_isr;
    top_cfg.user_data = NULL;
    top_cfg.flags = 0;  /* 默认：计数器归零，触发回调 */

	
    ret = counter_start(tim5_dev_pst);
    if (ret) {
        printk("counter_start failed: %d\n", ret);
        return ret;
    }

    /* 7. 设置 top value —— 这会产生周期中断！ */
    ret = counter_set_top_value(tim5_dev_pst, &top_cfg);
    if (ret) {
        printk("counter_set_top_value failed: %d\n", ret);
        return ret;
    }

    printk("TIM5 1ms periodic interrupt started\n");
    return 0;
}

SYS_INIT(tim5_1ms_init, APPLICATION, 10);


static void s_task_1ms_high(void *p1,void *p2,void *p3)
{
	while (1) {
        k_sem_take(&tim5_sem, K_FOREVER);
        car_2026_step0();
	}
}

void s_task_5ms_high(void *p1,void *p2,void *p3 )
{
    while (1) {
        encoder_update_all();

        struct encoder_data_t motor_a;
        struct encoder_data_t motor_b;

        encoder_get_data(g_encoder_a_pst, &motor_a);
        encoder_get_data(g_encoder_b_pst, &motor_b);
        car_2026_U.motor_a_spd = motor_a.rpm_f;
        car_2026_U.spd_b = motor_b.rpm_f;
        car_2026_step1();
        motor_set(g_motor_a_pst, car_2026_Y.pwm_a);
        motor_set(g_motor_b_pst, car_2026_Y.pwm_b);
        k_sleep(K_MSEC(4));
    }
}

//要加入循环才行
void s_task_20ms_high(void* p1,void* p2,void *p3)
{
    while (1) {
        menu_task_v();
        k_sleep(K_MSEC(20));
    }
}

void s_task_rx(void* p1,void* p2,void *p3)
{
    struct uart_event_t rx_event;

    for (;;)
    {
        k_msgq_get(&uart_rx_queue, &rx_event, K_FOREVER);
        if (rx_event.type_e == UART_EVENT_RX_DATA)
        {
            uart_rx_analyze(rx_event.base);
        }
    }
}





int test_callback(uint8_t*data,uint32_t len32,void* user_data)
{
    uart_transmit(g_uart_computer, data, len32);
    return 0;
}

int main(void)
{

    k_thread_create(&s_thread_1ms_high, s_stack_1ms_high, sizeof(s_stack_1ms_high), s_task_1ms_high, NULL, NULL, NULL, K_PRIO_PREEMPT(3), 0, K_NO_WAIT);
    k_thread_create(&s_thread_5ms_high, s_stack_5ms_high, sizeof(s_stack_5ms_high), s_task_5ms_high, NULL, NULL, NULL, K_PRIO_PREEMPT(4), 0, K_NO_WAIT);
    k_thread_create(&s_thread_20ms_high, s_stack_20ms_high, sizeof(s_stack_20ms_high), s_task_20ms_high, NULL, NULL, NULL, K_PRIO_PREEMPT(10), 0, K_NO_WAIT);
    k_thread_create(&s_thread_task_rx, s_stack_task_rx, sizeof(s_stack_task_rx), s_task_rx, NULL,NULL, NULL, K_PRIO_PREEMPT(3), 0,K_NO_WAIT);
    while (1) {
        k_sleep(K_MSEC(1000));
	}
	return 0;
}