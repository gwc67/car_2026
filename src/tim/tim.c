#include "tim.h"
#include "zephyr/drivers/counter.h"
#include "zephyr/kernel.h"
#include "event.h"

static const struct device *tim5_dev_pst;

K_SEM_DEFINE(tim5_sem, 0, 1);

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

SYS_INIT(tim5_1ms_init, APPLICATION, INIT_1);




