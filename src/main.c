#include "encode.h"
#include "menu/menu.h"
#include "motor/tb6612.h"
#include "zephyr/device.h"
#include "zephyr/drivers/gpio.h"
#include "zephyr/kernel.h"
static K_THREAD_STACK_DEFINE(s_stack_5ms_high, 4096); /* 1.5KB */
static K_THREAD_STACK_DEFINE(s_stack_20ms_high, 4096); /* 1.5KB */
static struct k_thread s_thread_5ms_high;
static struct k_thread s_thread_20ms_high;

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

int main(void)
{

    k_thread_create(&s_thread_5ms_high, s_stack_5ms_high, sizeof(s_stack_5ms_high), s_task_5ms_high, NULL, NULL, NULL, K_PRIO_PREEMPT(4), 0, K_NO_WAIT);
    k_thread_create(&s_thread_20ms_high, s_stack_20ms_high, sizeof(s_stack_20ms_high), s_task_20ms_high, NULL, NULL, NULL, K_PRIO_PREEMPT(10), 0, K_NO_WAIT);
    while (1) {
        
        // gpio_pin_toggle_dt(&led0);
        k_sleep(K_MSEC(1000));
	}
	return 0;
}