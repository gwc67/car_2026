#include "ano/ano_base.h"
#include "double_tree/double_tree.h"
#include "encode.h"
#include "euler.h"
#include "menu/OLED_Menu.h"
#include "menu/menu.h"
#include "motor/tb6612.h"
#include "simulink/ARMCortex-M/car_2026/car_2026.h"
#include "uart/uart_base.h"
#include "value_to_str.h"
#include "zephyr/kernel.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/_intsup.h>
#include "event.h"  
#include "zephyr/kernel/thread.h"
#include "zephyr/kernel/thread_stack.h"
#include "zephyr/syscalls/kernel.h"

K_MSGQ_DEFINE(uart_rx_queue, sizeof(struct uart_event_t), 30,4);

static K_THREAD_STACK_DEFINE(s_statck_ano, 4096);      
static K_THREAD_STACK_DEFINE(s_stack_rx,2048);
static K_THREAD_STACK_DEFINE(s_statck_dispatch, 4096); 
static K_THREAD_STACK_DEFINE(s_stack_1ms_high, 4096); 
static K_THREAD_STACK_DEFINE(s_stack_5ms_high, 4096); 
static K_THREAD_STACK_DEFINE(s_stack_10ms, 4096); 
static K_THREAD_STACK_DEFINE(s_stack_5ms_low,4096);


static struct k_thread s_thread_ano;
static struct k_thread s_thread_rx;
static struct k_thread s_thread_5ms_high;
static struct k_thread s_thread_5ms_low;
static struct k_thread s_thread_dispatch;
static struct k_thread s_thread_10ms;
static struct k_thread s_thread_1ms_high;


extern struct k_sem tim5_sem;
extern struct k_msgq ano_tx_queue;
static K_SEM_DEFINE(uart_print_sem, 0, 1); // 初始计数值为0，最大值为1，用作二进制信号量

tree_queue_t *tree;

static void s_task_1ms_high(void *p1,void *p2,void *p3)
{
	while (1) {
        k_sem_take(&tim5_sem, K_FOREVER);
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
  char tar_spd_a[TELEMETRY_FIELD_SIZE];
  char tar_spd_b[TELEMETRY_FIELD_SIZE];
  char motor_a_fliter[TELEMETRY_FIELD_SIZE];
  char motor_b_fliter[TELEMETRY_FIELD_SIZE];
  char pwm_b[TELEMETRY_FIELD_SIZE];
  char yaw[TELEMETRY_FIELD_SIZE];
  char turn_spd[TELEMETRY_FIELD_SIZE];

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
        // k_msgq_get(&uart_rx_queue,&uart_event, K_FOREVER);
        // uart_rx_analyze(uart_event.base);
        
        menu_refresh();
        // struct ano_event_t test = {g_com_ano,0x01};
        // k_msgq_put(&ano_tx_queue,&test, K_NO_WAIT);
        // format_telemetry(s_telemetry_buf, sizeof(s_telemetry_buf));
        // uart_transmit(g_uart_computer, s_telemetry_buf, strlen(s_telemetry_buf));
        menu_task_v();
        // k_sleep(K_MSEC(1000));

    }
}


void s_task_10ms(void* p1,void* p2,void *p3)
{

    for (;;) {

        car_2026_step2();
        k_sleep(K_MSEC(9));
    }
}

void s_task_dispatch(void* p1,void* p2,void *p3)
{
    struct event_t evt;
    tree = tree_queue_create();
    for (;;)
    {
        tree_err_t ret = tree_queue_pop(tree,&evt, K_FOREVER);    
        if (ret == TREE_OK) {
            dispatch_event(&evt);
        }
    }
}


void s_task_rx(void* p1,void* p2, void* p3)
{
    struct uart_event_t uart_event;
    for (; ; ) {
        k_msgq_get(&uart_rx_queue, &uart_event, K_FOREVER);
        if (uart_event.type_e == UART_EVENT_RX_DATA) {
            uart_rx_analyze(uart_event.base);
        }
    }
}

void s_task_ano(void* p1,void* p2,void* p3)
{
    struct ano_event_t event;
    for (; ; ) {
        k_msgq_get(&ano_tx_queue, &event, K_FOREVER);
        ano_send_data(event.me, event.frame);
    }
}

int main(void)
{

    k_thread_create(&s_thread_1ms_high, s_stack_1ms_high, sizeof(s_stack_1ms_high), s_task_1ms_high, NULL, NULL, NULL, K_PRIO_PREEMPT(3), 0, K_NO_WAIT);
    k_thread_create(&s_thread_5ms_high, s_stack_5ms_high, sizeof(s_stack_5ms_high), s_task_5ms_high, NULL, NULL, NULL, K_PRIO_PREEMPT(4), 0, K_NO_WAIT);
    k_thread_create(&s_thread_5ms_low, s_stack_5ms_low, sizeof(s_stack_5ms_low), s_task_5ms_low, NULL, NULL, NULL, K_PRIO_PREEMPT(10), 0, K_NO_WAIT);
    k_thread_create(&s_thread_10ms, s_stack_10ms, sizeof(s_stack_10ms),s_task_10ms, NULL,NULL, NULL, K_PRIO_PREEMPT(5), 0,K_NO_WAIT);
    k_thread_create(&s_thread_dispatch, s_statck_dispatch, sizeof(s_statck_dispatch),s_task_dispatch, NULL,NULL, NULL, K_PRIO_PREEMPT(7), 0,K_NO_WAIT);
    k_thread_create(&s_thread_ano, s_statck_ano, sizeof(s_statck_ano),s_task_ano, NULL,NULL, NULL, K_PRIO_PREEMPT(6), 0,K_NO_WAIT);
    k_thread_create(&s_thread_rx, s_stack_rx, sizeof(s_stack_rx),s_task_rx, NULL,NULL, NULL, K_PRIO_PREEMPT(7), 0,K_NO_WAIT);


    while (1) {
        k_sleep(K_MSEC(1000));
	}
	return 0;
}