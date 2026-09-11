#include "encode.h"
#include "euler.h"
#include "menu/OLED_Menu.h"
#include "menu/menu.h"
#include "motor/tb6612.h"
#include "mpu6050.h"
#include "simulink/ARMCortex-M/car_2026/car_2026.h"
#include "uart/uart_base.h"
#include "uart/uarts.h"
#include "value_to_str.h"
#include "zephyr/drivers/counter.h"
#include "zephyr/kernel.h"
#include "zephyr/posix/sys/stat.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/_intsup.h>

// K_MSGQ_DEFINE(uart_rx_quesue, sizeof(struct uart_event_t), 30,4);


static K_THREAD_STACK_DEFINE(s_stack_1ms_high, 4096); /* 1.5KB */
static K_THREAD_STACK_DEFINE(s_stack_5ms_high, 4096); /* 1.5KB */
static K_THREAD_STACK_DEFINE(s_stack_5ms_low, 4096); /* 1.5KB */
static K_THREAD_STACK_DEFINE(s_stack_10ms, 4096); /* 1.5KB */

static struct k_thread s_thread_1ms_high;
static struct k_thread s_thread_5ms_high;
static struct k_thread s_thread_5ms_low;
static struct k_thread s_thread_10ms;


static K_SEM_DEFINE(tim5_sem, 0, 1);
static K_SEM_DEFINE(uart_print_sem, 0, 1); // 初始计数值为0，最大值为1，用作二进制信号量

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
        uart_rx_analyze(g_uart_computer);
        car_2026_step0();
        motor_set(g_motor_a_pst, car_2026_Y.pwm_a);
        motor_set(g_motor_b_pst, car_2026_Y.pwm_b);

	}
}

void s_task_5ms_high(void *p1,void *p2,void *p3 )
{
    while (1) {
        euler_update();
        encoder_update_all();
        struct encoder_data_t motor_a;
        struct encoder_data_t motor_b;
        encoder_get_data(g_encoder_a_pst, &motor_a);
        encoder_get_data(g_encoder_b_pst, &motor_b);

        
        car_2026_U.spd_a = motor_a.rpm_f;
        car_2026_U.spd_b = motor_b.rpm_f;
        car_2026_step1();


        k_sem_give(&uart_print_sem);
        k_sleep(K_MSEC(4));
    }
}



#define TELEMETRY_BUF_SIZE (150U)
#define TELEMETRY_FIELD_SIZE    (10U)

static void format_telemetry(char *buf, size_t buf_size) {
//   char motor_a[TELEMETRY_FIELD_SIZE];
//   char motor_b[TELEMETRY_FIELD_SIZE];
  char tar_spd_a[TELEMETRY_FIELD_SIZE];
  char tar_spd_b[TELEMETRY_FIELD_SIZE];
  char motor_a_fliter[TELEMETRY_FIELD_SIZE];
  char motor_b_fliter[TELEMETRY_FIELD_SIZE];
  char pwm_b[TELEMETRY_FIELD_SIZE];
  char yaw[TELEMETRY_FIELD_SIZE];
  char turn_spd[TELEMETRY_FIELD_SIZE];

  

//   float_to_str(motor_a, sizeof(motor_a), car_2026_U.spd_a, 2);
//   float_to_str(motor_b, sizeof(motor_b), car_2026_U.spd_b, 2);
  float_to_str(tar_spd_a, sizeof(tar_spd_a), car_2026_Y.tar_spd_a_2, 2);
  float_to_str(tar_spd_b, sizeof(tar_spd_b), car_2026_Y.tar_spd_b_2, 2);
  float_to_str(motor_a_fliter, sizeof(motor_a_fliter), car_2026_Y.spd_fliter_a, 2);
  float_to_str(motor_b_fliter, sizeof(motor_b_fliter), car_2026_Y.spd_fliter_b, 2);
  float_to_str(pwm_b, sizeof(pwm_b), car_2026_Y.pwm_b, 2);
  float_to_str(yaw, sizeof(yaw), car_2026_Y.yaw_out, 2);
  float_to_str(turn_spd, sizeof(turn_spd), car_2026_Y.turn_spd, 2);

  snprintf(buf, buf_size, "%s,%s,%s,%s,%s,%s,%s\r\n",tar_spd_a,
           tar_spd_b,motor_a_fliter,motor_b_fliter,pwm_b,yaw,
        turn_spd);
}

static void menu_refresh(void)
{
    menu_request_refresh(g_mpu6050_euler_oled_pst);
    menu_request_refresh(g_mpu6050_raw_oled_pst);
}

//要加入循环才行
void s_task_5ms_low(void* p1,void* p2,void *p3)
{
    static char s_telemetry_buf[TELEMETRY_BUF_SIZE];
    while (1) {
        k_sem_take(&uart_print_sem, K_FOREVER);
        menu_refresh();
        format_telemetry(s_telemetry_buf, sizeof(s_telemetry_buf));
        uart_transmit(g_uart_computer, s_telemetry_buf, strlen(s_telemetry_buf));
        menu_task_v();
    }
}

void s_task_10ms(void* p1,void* p2,void *p3)
{
    for (;;)
    {
        car_2026_step2();
        k_sleep(K_MSEC(9));
    }
}



int main(void)
{

    k_thread_create(&s_thread_1ms_high, s_stack_1ms_high, sizeof(s_stack_1ms_high), s_task_1ms_high, NULL, NULL, NULL, K_PRIO_PREEMPT(3), 0, K_NO_WAIT);
    k_thread_create(&s_thread_5ms_high, s_stack_5ms_high, sizeof(s_stack_5ms_high), s_task_5ms_high, NULL, NULL, NULL, K_PRIO_PREEMPT(4), 0, K_NO_WAIT);
    k_thread_create(&s_thread_5ms_low, s_stack_5ms_low, sizeof(s_stack_5ms_low), s_task_5ms_low, NULL, NULL, NULL, K_PRIO_PREEMPT(10), 0, K_NO_WAIT);
    k_thread_create(&s_thread_10ms, s_stack_10ms, sizeof(s_stack_10ms),s_task_10ms, NULL,NULL, NULL, K_PRIO_PREEMPT(5), 0,K_NO_WAIT);
    while (1) {
        k_sleep(K_MSEC(1000));
	}
	return 0;
}