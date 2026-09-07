#include "zephyr/device.h"
#include "zephyr/drivers/gpio.h"
#include "zephyr/kernel.h"

static K_THREAD_STACK_DEFINE(s_stack_5ms_high, 4096); /* 1.5KB */
static struct k_thread s_thread_5ms_high;

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_ALIAS(led0),gpios);


void s_task_5ms_high(void *p1,void *p2,void *p3 )
{
    k_sleep(K_MSEC(1000));
}

int main(void)
{
    /* GPIO_DT_SPEC_GET 只是把 led0 解析成 {port=gpioa, pin=8, flags=ACTIVE_HIGH}。
     * 复位后 PA8 默认是"输入/浮空"模式, 必须先配置成输出, 后面 set 才有效. */
    if (!gpio_is_ready_dt(&led0)) {
        return -1;
    }
    if (gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE) < 0) {
        return -1;
    }

    k_thread_create(&s_thread_5ms_high, s_stack_5ms_high, sizeof(s_stack_5ms_high), s_task_5ms_high, NULL, NULL, NULL, K_PRIO_PREEMPT(4), 0, K_NO_WAIT);
    while (1) {
        
        gpio_pin_toggle_dt(&led0);
        k_sleep(K_MSEC(1000));
	}
	return 0;
}