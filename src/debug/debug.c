#include "debug.h"
#include <stdint.h>
#include <string.h>
#include <sys/_intsup.h>
#include "uart_base.h"
#include "uarts.h"
#include "value_to_str.h"
#include "zephyr/init.h"
// #include "zephyr/drivers/gpio.h"
#define LINE_BUF_SIZE 128


static void  dispatch_line(char* line_pc);

// static const struct gpio_dt_spec led_motor_st =GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

//但是现在我没办法保证是一口气就能获得完整字节啊
int debug_par_check(uint8_t* data,uint32_t len32,void* user_data)
{
    static char s_line_buf[LINE_BUF_SIZE];       
    uint8_t lie_pos = 0;

    for (int i = 0; i < len32; i++) {
        if (data[i] == '\n' || data[i] == '\r') {
            if (lie_pos > 0) {
                s_line_buf[lie_pos] = '\0';
                dispatch_line(s_line_buf);
                lie_pos = 0;
            }
            continue;
        }
        if (lie_pos < LINE_BUF_SIZE - 1) {
            s_line_buf[lie_pos++] = (char)data[i];
        }
        else {
            lie_pos = 0;
        }
    }
    
    // while (!g_ut2_rx_ring_pst)) {
    //     my_ring_buf_get(g_ut2_rx_ring_pst, &byte, 1);
    //     if (byte == '\n' || byte == '\r') {
    //         if (lie_pos_uc > 0) {
    //             s_line_buf_pc[lie_pos_uc] = '\0';
    //             dispatch_line(s_line_buf_pc);
    //             lie_pos_uc = 0;
    //         }
    //         continue;  // 换行符本身不入缓冲区
    //     }
    //     if (lie_pos_uc < LINE_BUF_SIZE - 1) {
    //         s_line_buf_pc[lie_pos_uc++] = (char)byte;
    //     } else {
    //         lie_pos_uc = 0;
    //     }
    // }
}


static void  dispatch_line(char* line_pc)
{
    float values_pf[4] = {0};
    // int16_t values_ps[4] = {0};
    str_to_float(line_pc, values_pf, 4);

    if (strncmp(line_pc, "tar_spd:", 8) == 0) {
        uart_transmit(g_uart_computer, "tar_spd_set_success\r\n", 21);
    }
}

int debug_init(void)
{
    return uart_register_callback(g_uart_computer, debug_par_check,NULL);
}

SYS_INIT(debug_init, APPLICATION, 13);