#include "debug.h"
#include <stdint.h>
#include <string.h>
#include <sys/_intsup.h>
#include "car_2026.h"
#include "tb6612.h"
#include "uart_base.h"
#include "uarts.h"
#include "value_to_str.h"
#include "zephyr/init.h"
// #include "zephyr/drivers/gpio.h"
#define LINE_BUF_SIZE 128


static void  dispatch_line(char* line_pc);

// static const struct gpio_dt_spec led_motor_st =GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

//但是现在我没办法保证是一口气就能获得完整字节啊
//只能这样了，需要数据必须含义\r\n才能保证s_lie_pos被清零，否则得输入两次
int debug_par_check(uint8_t* data,uint32_t len32,void* user_data)
{
    static char s_line_buf[LINE_BUF_SIZE];       
    static uint8_t s_lie_pos = 0;

    for (int i = 0; i < len32; i++) {
        if (data[i] == '\n' || data[i] == '\r') {
            if (s_lie_pos > 0) {
                s_line_buf[s_lie_pos] = '\0';
                dispatch_line(s_line_buf);
                s_lie_pos = 0;
            }
            continue;
        }
        if (s_lie_pos < LINE_BUF_SIZE - 1) {
            s_line_buf[s_lie_pos++] = (char)data[i];
        }
        else {
            s_lie_pos = 0;
        }
    }
    return 0;
}


static void  dispatch_line(char* line_pc)
{
    float values_pf[4] = {0};
    // int16_t values_ps[4] = {0};
    str_to_float(line_pc, values_pf, 4);

    if (strncmp(line_pc, "turn_tar:", 8) == 0) {
        // int16_t averget_pwm = (int16_t)(values_pf[1] * 25);
        // int16_t turn_pwm = (int16_t)(values_pf[0] * 10);

        // motor_set(g_motor_a_pst, averget_pwm - turn_pwm);
        // motor_set(g_motor_b_pst, averget_pwm + turn_pwm);

        car_2026_U.tar_spd_a = values_pf[1];
        car_2026_U.tar_spd_b = values_pf[1];
        
        uart_transmit(g_uart_computer, "tar_spd_set_success\r\n", 21);
    }
    else if (strncmp(line_pc, "spd_kp:", 7) == 0) {
        SPD_KP = values_pf[0];
        uart_transmit(g_uart_computer, "spd_kp_set\r\n", 21);

    }
    else if (strncmp(line_pc, "spd_ki:", 7) == 0) {
        SPD_KI = values_pf[0];
        uart_transmit(g_uart_computer, "spd_ki_set\r\n", 21);
    }
}

int debug_init(void)
{
    return uart_register_callback(g_uart_computer, debug_par_check,NULL);
}

SYS_INIT(debug_init, APPLICATION, 13);