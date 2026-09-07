#include "encode.h"
#include "menu/menu.h"
#include "motor/tb6612.h"
#include "uart/uart_base.h"
#include "zephyr/device.h"
#include "zephyr/drivers/gpio.h"
#include "zephyr/kernel.h"
#include "zephyr/syscalls/kernel.h"
#include "zephyr/syscalls/uart.h"
#include <stdint.h>
#include <stdio.h>
#include "src\uart\uarts.h"

K_MSGQ_DEFINE(uart_tx_queue, sizeof(struct uart_event_t), 30,4);
K_MSGQ_DEFINE(uart_rx_queue, sizeof(struct uart_event_t), 30,4);


static K_THREAD_STACK_DEFINE(s_stack_5ms_high, 4096); /* 1.5KB */
static K_THREAD_STACK_DEFINE(s_stack_20ms_high, 4096); /* 1.5KB */
static K_THREAD_STACK_DEFINE(s_stack_task_tx, 4096); /* 1.5KB */
static K_THREAD_STACK_DEFINE(s_stack_task_rx, 4096); /* 1.5KB */

static struct k_thread s_thread_5ms_high;
static struct k_thread s_thread_20ms_high;

static struct k_thread s_thread_task_tx;
static struct k_thread s_thread_task_rx;

// static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_ALIAS(led0),gpios);

// extern volatile uint8_t running_flag;

void s_task_5ms_high(void *p1,void *p2,void *p3 )
{
    while (1) {
        encoder_update_all();
        k_sleep(K_MSEC(5));
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


void s_task_tx(void* p1,void* p2,void *p3)
{
    struct uart_event_t tx_event;
    while (1) {
        k_msgq_get(&uart_tx_queue,&tx_event,K_FOREVER);
        uart_tx_callback(tx_event.base, tx_event.type_e);
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


typedef  int (*uart_callback_t)(uint8_t* data,uint32_t len32,void* user_data);

int test_callback(uint8_t*data,uint32_t len32,void* user_data)
{
    uart_transmit(g_uart_computer, data, len32);
}

int main(void)
{

    k_thread_create(&s_thread_5ms_high, s_stack_5ms_high, sizeof(s_stack_5ms_high), s_task_5ms_high, NULL, NULL, NULL, K_PRIO_PREEMPT(4), 0, K_NO_WAIT);
    k_thread_create(&s_thread_20ms_high, s_stack_20ms_high, sizeof(s_stack_20ms_high), s_task_20ms_high, NULL, NULL, NULL, K_PRIO_PREEMPT(10), 0, K_NO_WAIT);
    k_thread_create(&s_thread_task_tx, s_stack_task_tx, sizeof(s_stack_task_tx), s_task_tx, NULL,NULL, NULL, K_PRIO_PREEMPT(3), 0,K_NO_WAIT);
    k_thread_create(&s_thread_task_rx, s_stack_task_rx, sizeof(s_stack_task_rx), s_task_rx, NULL,NULL, NULL, K_PRIO_PREEMPT(3), 0,K_NO_WAIT);
    uart_register_callback(g_uart_computer,test_callback ,NULL);
    while (1) {
        
        // gpio_pin_toggle_dt(&led0);
        k_sleep(K_MSEC(1000));
	}
	return 0;
}