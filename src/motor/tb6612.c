#include "tb6612.h"
#include "key.h"
#include "zephyr/init.h"
#include "zephyr/sys/util.h"
#include <stdbool.h>
#include <sys/errno.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "D:\Downloads\zephyr_new\myapp\new\car_2026\src\simulink\ARMCortex-M\car_2026\car_2026.h"
LOG_MODULE_REGISTER(tb6612, LOG_LEVEL_INF);


static const struct gpio_dt_spec led_motor_st =GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);


static void motor_key_callback(enum key_id_e key_id_em,enum key_event_e event_em)
{
    switch (key_id_em) {
        case KEY_0_em:
        {
            if (event_em == KEY_EVENT_DOUBLE_em) {
                car_2026_U.running_flag  ^= 1;
                gpio_pin_set_dt(&led_motor_st,car_2026_U.running_flag);
            }
        }
        break; 
        default:
        break;       
    }
}

KEY_SUBSCRIBE_DEFINE(motor_key,motor_key_callback,"motor");



#define PWM_PERIOD_NS   20000000  /* 20ms = 50Hz */

struct motor_cfg_t{
    const struct device* pwm_dev_pst;
    uint32_t pwm_channel_ul;
    const struct gpio_dt_spec motor_in1_st;
    const struct gpio_dt_spec motor_in2_st;
};

struct motor_t {
    struct motor_base_t base;
    const struct motor_cfg_t* cfg_pst;
};

static struct motor_t s_motor_a_st;
struct motor_base_t* g_motor_a_pst;


static struct motor_t s_motor_b_st;
struct motor_base_t* g_motor_b_pst;

static int tb6612_init(struct motor_t* me,const struct motor_cfg_t* cfg_pst,const char* name)
{


    
    if (!me || !cfg_pst || !name) {
        return -EINVAL;
    }

    if (!device_is_ready(cfg_pst->pwm_dev_pst)) {
            LOG_ERR("Motor %s PWM not ready", name);
            return -ENODEV;
    }
    
    me->cfg_pst = cfg_pst;
    me->base.name = name;

    int ret;

    ret = gpio_pin_configure_dt(&me->cfg_pst->motor_in1_st, GPIO_OUTPUT_INACTIVE);
    if (ret)
      return ret;

    ret = gpio_pin_configure_dt(&me->cfg_pst->motor_in2_st, GPIO_OUTPUT_INACTIVE);
    if (ret)
      return ret;

    /* 可以设置pwm的周期 实质上是对tim的重装寄存器进行设置 */
    pwm_set(me->cfg_pst->pwm_dev_pst,me->cfg_pst->pwm_channel_ul, PWM_PERIOD_NS, 0, 0);

    LOG_INF("Motor %s initialized", name);
    return 0;
}



int tb6612_board_init(void)
{
    int ret;

    /* 获取 PWM 设备 */
    ret = gpio_pin_configure_dt(&led_motor_st, GPIO_OUTPUT_INACTIVE);

    if (ret) return ret;

/* 返回的是结构体的值 GPIO_DT_SPEC_GET(DT_NODELABEL(ain1), gpios) */
    static const struct motor_cfg_t motor_a_cfg_st = {
        .pwm_dev_pst = DEVICE_DT_GET(DT_NODELABEL(pwm2)),
        .pwm_channel_ul = 1,
        .motor_in1_st = GPIO_DT_SPEC_GET(DT_NODELABEL(ain1), gpios),
        .motor_in2_st = GPIO_DT_SPEC_GET(DT_NODELABEL(ain2), gpios),
    };

    ret = tb6612_init(&s_motor_a_st,&motor_a_cfg_st,"motor_a");
    g_motor_a_pst = &s_motor_a_st.base;

    if (ret)
      return ret;
    
    static const struct motor_cfg_t motor_b_cfg_st = 
    {
        .pwm_dev_pst = DEVICE_DT_GET(DT_NODELABEL(pwm2)),
        .pwm_channel_ul = 2,
        .motor_in1_st = GPIO_DT_SPEC_GET(DT_NODELABEL(bin2), gpios),
        .motor_in2_st = GPIO_DT_SPEC_GET(DT_NODELABEL(bin1), gpios),
    };

    ret = tb6612_init(&s_motor_b_st, &motor_b_cfg_st, "motor_b");
    g_motor_b_pst = &s_motor_b_st.base;

    if (ret)
      return ret;
    
    return 0;
}

SYS_INIT(tb6612_board_init, APPLICATION, 10);


int motor_set(struct motor_base_t* base, int16_t speed)
{
    if (!base) return -EINVAL;

    struct motor_t* me = CONTAINER_OF(base, struct motor_t, base);

    uint32_t pulse_ns_ul;

    
    if (speed > 1000) speed = 1000;
    if (speed < -1000) speed = -1000;

    if (speed > 0) {
        gpio_pin_set_dt(&me->cfg_pst->motor_in1_st, 1);
        gpio_pin_set_dt(&me->cfg_pst->motor_in2_st, 0);
        pulse_ns_ul = (PWM_PERIOD_NS / 1000) * speed;
    } else if (speed < 0) {
        gpio_pin_set_dt(&me->cfg_pst->motor_in1_st, 0);
        gpio_pin_set_dt(&me->cfg_pst->motor_in2_st, 1);
        pulse_ns_ul = (PWM_PERIOD_NS / 1000) * (-speed);
    } else {
        gpio_pin_set_dt(&me->cfg_pst->motor_in1_st, 0);
        gpio_pin_set_dt(&me->cfg_pst->motor_in2_st, 0);
        pulse_ns_ul = 0;
    }

    return pwm_set(me->cfg_pst->pwm_dev_pst, me->cfg_pst->pwm_channel_ul, PWM_PERIOD_NS, pulse_ns_ul, 0);
}

int motor_brake(struct motor_base_t* base)
{
    if (!base) return -EINVAL;

    struct motor_t* me = CONTAINER_OF(base, struct motor_t, base);

    gpio_pin_set_dt(&me->cfg_pst->motor_in1_st, 1);
    gpio_pin_set_dt(&me->cfg_pst->motor_in2_st, 1);

    return pwm_set(me->cfg_pst->pwm_dev_pst, me->cfg_pst->pwm_channel_ul, PWM_PERIOD_NS, 0, 0);
}







