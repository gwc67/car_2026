#include "ano_device_com.h"
#include "..\ano.h"
#include "uarts.h"
#include "zephyr/init.h"
#include "zephyr/logging/log.h"
#include <sys/_intsup.h>

LOG_MODULE_REGISTER(ano_device_com);

void com_receive_anl(uint8_t* data,uint8_t len8)
{
    uint8_t check_sum1 = 0, check_sum2 = 0;
    if (*(data + 3) != (len8 - 6))
    {
        return;
    }
    for (uint8_t i = 0; i < len8 - 2; i++)
    {
        check_sum1 += *(data + i);
        check_sum2 += check_sum1;
    }

    if ((check_sum1 != *(data + len8 - 2)) || (check_sum2 != *(data + len8 - 1)))
    {
        return;
    }
    
    if(*(data + 2) == 0x00)
    {
        struct ck_t snap = {0};
        if (snap.id_uc == *(data + 4) && snap.sc_uc == *(data + 5) && snap.ac_uc == *(data + 6)) {
            ano_clear_wait(g_com_ano);
        }
    }
    else if (*(data + 2) == 0xe0)
    {
        struct ck_t send2check = {0};
        send2check.id_uc = *(data + 2);
        send2check.sc_uc = check_sum1;
        send2check.ac_uc = check_sum2;
        ano_set_send2check(g_com_ano,&send2check);
    }
    else if (*(data + 2) == 0xe1)
    {
        struct ck_t send2check = {0};
        send2check.id_uc = *(data + 2);
        send2check.sc_uc = check_sum1;
        send2check.ac_uc = check_sum2;
        ano_set_send2check(g_com_ano,&send2check);
    }

}

void com_add_send_data(uint8_t frame,uint8_t *cnt_ptr,uint8_t* data)
{
    switch (frame) {
        case 0x00:
        {
            struct ck_t send2check = {0};
            ano_get_send2check(g_com_ano,&send2check);
            data[(*cnt_ptr)++] = send2check.id_uc;
            data[(*cnt_ptr)++] = send2check.sc_uc;
            data[(*cnt_ptr)++] = send2check.ac_uc;
        }
        break;
        case 0x01:
        {
            data[(*cnt_ptr)++] = 0x01;
        }
        break;
        default:
            break;
    }
}


void com_send_buffer(uint8_t *data,uint8_t len8)
{
    uart_transmit(g_uart_computer, data, len8);
}


//prio 可能没有明显效果，比如定时事件中，EVT_TIMER_1000MS 一产生就被取走，导致即使EVT_TIMER_500MS 优先级更高，却是1000MS的事件先执行
//因为1000MS的事件先产生，想要解决的话，就只能够通过约束soft_timer中 1000MS的回调注册在500ms之后 syster_timer_init 中约束
// 
static int s_ano_device_com_init(void)
{
    //默认都可以事件触发
    int ret = ano_register_callback(g_com_ano, com_receive_anl, com_add_send_data, com_send_buffer);
    if (ret) {
        LOG_ERR("frame_fail_set");
        return ret;
    }
    ret = ano_set_send_id(g_com_ano, 0x02,EVT_TIMER_10MS,2);
    if (ret) {
        LOG_ERR("frame_fail_set");
        return ret;
    }
    ret = ano_set_send_id(g_com_ano, 0x01,EVT_TIMER_10MS,1);
    if (ret) {
        LOG_ERR("frame_fail_set");
        return ret;
    }
    LOG_INF("frame_init_success");    
    return 0;
}

SYS_INIT(s_ano_device_com_init,APPLICATION,INIT_3);