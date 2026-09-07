#include "key.h"
#include "zephyr/dt-bindings/input/input-event-codes.h"
#include "zephyr/init.h"
#include "zephyr/input/input.h"
#include "zephyr/kernel.h"
#include "zephyr/logging/log.h"
#include "zephyr/logging/log_core.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/_intsup.h>

LOG_MODULE_REGISTER(key_event,LOG_LEVEL_INF);

#define DOUBLE_CLICK_MS 300
#define LONG_CLICK_MS  1000
#define REPEAT_MS       200

enum gesture_state_e {
    STATE_IDLE_em,
    STATE_WAIT_RELEASE_em,
    STATE_WAIT_DOUBLE_em,
    STATE_REPEAT_em,
};

struct key_runtime {
    enum gesture_state_e state_em;
    uint32_t tick_ms_ul;
    bool pressed_b;
};
static struct key_runtime s_run_time_pst[KEY_NUM_em];/* 每个按键的运行时间 */
static struct key_subscriber_t* s_sub_list_pst ;  /* 订阅者链表头 */

static enum key_id_e s_double_key_em;   /* 正在检测双击的按键 */
static enum key_id_e s_long_key_em;     /* 正在检测长按的按键 */
static enum key_id_e s_repeat_key_em;   /* 正在重复连发的按键 */

/* ---- 全局共享 work（只有3个！）---- */
static struct k_work_delayable s_double_work;
static struct k_work_delayable s_long_work;
static struct k_work_delayable s_repeat_work;



static void emit_event(enum key_id_e key_id_em, enum key_event_e key_event_em)
{
    struct key_subscriber_t* current_subscriber_pst = s_sub_list_pst;    //当前订阅者的表头

    //通过while轮询进行操作所有的订阅者
    while (current_subscriber_pst) {
        if (current_subscriber_pst->callback_pst) {
            current_subscriber_pst->callback_pst(key_id_em,key_event_em);
        }
        current_subscriber_pst = current_subscriber_pst->next_pst;      //执行完自动注册下一个
    }
}


/* work 回调函数 */
static void double_click_timeout(struct k_work *work)
{
    struct key_runtime *run_time_pst = &s_run_time_pst[s_double_key_em];
    
    if (run_time_pst->state_em == STATE_WAIT_DOUBLE_em) {
        emit_event(s_double_key_em, KEY_EVENT_SINGLE_em);
        run_time_pst->state_em = STATE_IDLE_em;
    }
}

static void long_press_timeout(struct k_work *work)
{
    struct key_runtime *run_time_pst = &s_run_time_pst[s_long_key_em];
    
    if (run_time_pst->state_em == STATE_WAIT_RELEASE_em && run_time_pst->pressed_b) {
        emit_event(s_long_key_em, KEY_EVENT_LONG_em);
        run_time_pst->state_em = STATE_REPEAT_em;
        run_time_pst->tick_ms_ul = k_uptime_get_32();
        
        /* 启动重复 */
        s_repeat_key_em = s_long_key_em;
        k_work_schedule(&s_repeat_work, K_MSEC(REPEAT_MS));
    }
}

static void repeat_timeout(struct k_work *work)
{
    struct key_runtime *run_time_pst = &s_run_time_pst[s_repeat_key_em];
    
    if (run_time_pst->state_em == STATE_REPEAT_em && run_time_pst->pressed_b) {
        emit_event(s_repeat_key_em, KEY_EVENT_REPEAT_em);
        run_time_pst->tick_ms_ul = k_uptime_get_32();
        k_work_schedule(&s_repeat_work, K_MSEC(REPEAT_MS));
    }
}




//将中断里面的evt下code的编码转换成自己定义的id号
static enum key_id_e code_to_key(uint16_t code)
{
    switch (code) {
    case INPUT_KEY_0: return KEY_0_em;   /* 11 → 0 */
    case INPUT_KEY_1: return KEY_1_em;   /* 2 → 1 */
    case INPUT_KEY_2: return KEY_2_em;   /* 3 → 2 */
    case INPUT_KEY_3: return KEY_3_em;   /* 4 → 3 */
    default: return KEY_NUM_em;
    }
}

static void raw_input_cb(struct input_event* evt,void *user_data )
{
    //只处理按键的按下和释放 ，其他的是比如键盘和按键的按钮
    if (evt->type != INPUT_EV_KEY)  return;

    enum key_id_e key_id_em = code_to_key(evt->code);
    bool pressed_b = (evt->value == 1);         //1 代表按下，0代表释放

    if (key_id_em >= KEY_NUM_em) return;

    struct key_runtime *run_time_pst = &s_run_time_pst[key_id_em];
    uint32_t now_ms_ul = k_uptime_get_32();

    run_time_pst->pressed_b = pressed_b;


        if (pressed_b) {
        /* 按下 */
        switch (run_time_pst->state_em) {
        case STATE_IDLE_em:
            run_time_pst->state_em = STATE_WAIT_RELEASE_em;
            run_time_pst->tick_ms_ul = now_ms_ul;
            
            /* 启动长按检测 */
            s_long_key_em = key_id_em;
            k_work_schedule(&s_long_work, K_MSEC(LONG_CLICK_MS));
            break;

        case STATE_WAIT_DOUBLE_em:
            /* 双击！取消之前的单击检测 */
            k_work_cancel_delayable(&s_double_work);
            emit_event(key_id_em, KEY_EVENT_DOUBLE_em);
            run_time_pst->state_em = STATE_IDLE_em;
            break;
        default:
            break;
        }
    } else {
        /* 释放 */
        switch (run_time_pst->state_em) {
        case STATE_WAIT_RELEASE_em:
            /* 短按释放：取消长按，启动双击检测 */
            k_work_cancel_delayable(&s_long_work);
            run_time_pst->state_em = STATE_WAIT_DOUBLE_em;
            run_time_pst->tick_ms_ul = now_ms_ul;
            
            s_double_key_em = key_id_em;
            k_work_schedule(&s_double_work, K_MSEC(DOUBLE_CLICK_MS));
            break;

        case STATE_REPEAT_em:
            /* 长按后释放：停止重复 */
            k_work_cancel_delayable(&s_repeat_work);
            run_time_pst->state_em = STATE_IDLE_em;
            break;
        default:
            break;
        }
    }
}

//像userdata，我个人认为应该是一个回调函数却被多个设备注册了
//串口的使用的那时候，明明就是回调函数传入的userdata是device的设备指针，这里并没有传入device
INPUT_CALLBACK_DEFINE(NULL,raw_input_cb, NULL);

void key_event_subscribe(struct key_subscriber_t* sub_pst)
{
    sub_pst->next_pst = s_sub_list_pst;
    s_sub_list_pst = sub_pst;
}

static int key_event_init(void)
{
    for (int i = 0; i < KEY_NUM_em ; i++) {
        s_run_time_pst[i].state_em = STATE_IDLE_em;
        s_run_time_pst[i].pressed_b = false;
    }

    k_work_init_delayable(&s_double_work, double_click_timeout);
    k_work_init_delayable(&s_long_work, long_press_timeout);
    k_work_init_delayable(&s_repeat_work, repeat_timeout);

    LOG_INF("key event initialized");
    return 0;
}

SYS_INIT(key_event_init, APPLICATION, 10);

