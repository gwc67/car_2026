#include "encode.h"
#include "OLED_Menu.h"
#include "zephyr/device.h"
#include "zephyr/drivers/sensor.h"
#include "zephyr/init.h"
#include "zephyr/kernel.h"
#include "zephyr/logging/log.h"
#include "zephyr/sys/util.h"
#include <complex.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/_intsup.h>
#include <sys/errno.h>

LOG_MODULE_REGISTER(encoder,LOG_LEVEL_INF);


struct encoder_cfg_t{
    const struct device *encoder_device_pst;
    bool  reverse_b;
};


struct encoder_device_t
{
    struct encoder_base_t base;
    const struct encoder_cfg_t* cfg_pst;
    int32_t last_raw_l; 
    uint32_t last_tick_ul;
    struct encoder_data_t data_st;
};

static struct encoder_device_t s_encoder_a_st;
struct encoder_base_t* g_encoder_a_pst;

static struct encoder_device_t s_encoder_b_st;
struct encoder_base_t* g_encoder_b_pst;

#define  ENCODE_CPR  44

#define TIM_ARR         65493   /* UINT16_MAX - (UINT16_MAX % 44) - 1 */



static int encoder_init(struct encoder_device_t* me,const struct encoder_cfg_t* cfg_pst,const char* name)
{
    if (!me || !cfg_pst) {
        LOG_ERR("Encode %s device not ready",name);
        return -EINVAL;
    }
    me->cfg_pst = cfg_pst;
    me->base.name = name;
    me->last_tick_ul = k_uptime_get_32();
    LOG_INF("Encode %s initialized",name);
    return 0;
    
}

static int encoder_board_init(void)
{

    static const struct encoder_cfg_t encoder_cfg_a_st = {
        .encoder_device_pst = DEVICE_DT_GET(DT_ALIAS(encodera)),
        .reverse_b = false, /* 不进行反转 */
    };
    int ret = encoder_init(&s_encoder_a_st,&encoder_cfg_a_st,"encoder_a");
    g_encoder_a_pst = &s_encoder_a_st.base;
    if (ret)   return ret;
    


    static const struct encoder_cfg_t encoder_cfg_b_st = {
        .encoder_device_pst = DEVICE_DT_GET(DT_ALIAS(encoderb)),
        .reverse_b = true, /* 不进行反转 */
    };
    ret = encoder_init(&s_encoder_b_st,&encoder_cfg_b_st,"encoder_b");
    g_encoder_b_pst = &s_encoder_b_st.base;
    if (ret)   return ret;
    
   
    return 0;    
}

SYS_INIT(encoder_board_init, APPLICATION, 10);

int encoder_read(struct encoder_base_t* base)
{
    if (!base) {
        return -EINVAL;
    }
    
    struct encoder_device_t* me = CONTAINER_OF(base, struct encoder_device_t, base);

    struct sensor_value val_st;
    int ret;

    ret = sensor_sample_fetch(me->cfg_pst->encoder_device_pst);
    if (ret != 0) {
        LOG_ERR("Encoder %s sample fetch failed: %d",base->name,ret);
        return ret;
    }



    ret = sensor_channel_get(me->cfg_pst->encoder_device_pst,SENSOR_CHAN_ENCODER_COUNT  ,&val_st);

    if (ret != 0) {
        LOG_ERR("Encoder %s channel get failed : %d",base->name,ret);
        return ret;
    }

    uint32_t current_tick_ul = k_uptime_get_32();

    int32_t current_raw_l = val_st.val1;     /* -32768 ~ 32767 */
    int32_t last_raw_l = me->last_raw_l ;

    int32_t delta_l = current_raw_l - last_raw_l;

    if (delta_l > 60000) {
        delta_l -= 0xFFFF ;
        delta_l += 20;                      //为什么传感器有这样的毛病，需要加一个20进行补偿？
    }
    else if (delta_l < -60000) {
        delta_l += 0xFFFF;
    }
    
    uint32_t dt_ms = current_tick_ul - me->last_tick_ul;

    int8_t dir_c = me->cfg_pst->reverse_b ? -1 : 1;
    

    delta_l *= dir_c;
    
    //计数器自增
    me->data_st.position_l  += delta_l;
    me->last_raw_l = current_raw_l;
    me->last_tick_ul = current_tick_ul;
    
    if (dt_ms > 0) {
        me->data_st.rpm_f = 
        (float)delta_l / dt_ms * 10;}

    return 0;
}

void encoder_update_all(void)
{
    encoder_read(g_encoder_a_pst);
    encoder_read(g_encoder_b_pst);
    menu_request_refresh(g_encode_oled_pst);
}


int encoder_get_data(struct encoder_base_t* base, struct encoder_data_t* out)
{
     if (!base || !out) {
        return EINVAL;
     }

    struct encoder_device_t* me = CONTAINER_OF(base, struct encoder_device_t, base);

    *out =  me->data_st;
    
    return 0;
}

