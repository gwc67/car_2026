#include "ano_base.h"
#include "ano_true.h"
#include "uarts.h"
#include "zephyr/init.h"

static struct ano_device_t s_com_st;
static uint8_t s_com_rxbuffer[FRAME_MAX_LENGTH];
static struct ano_frame_t s_com_frame_st;
struct ano_base_t* g_com_ano;

static struct ano_device_t s_k230_st;
static uint8_t s_k230_rxbuffer[FRAME_MAX_LENGTH];
static struct ano_frame_t s_k230_frame_st;
struct ano_base_t* g_k230_ano;

int ano_board_init(void)
{
    int result = 0;

    result = ano_device_init(&s_com_st, &s_com_frame_st, s_com_rxbuffer,g_uart_computer,"computer");
    if (result) {
        return result;
    }
    g_com_ano = &s_com_st.base;

    result = ano_device_init(&s_k230_st, &s_k230_frame_st, s_k230_rxbuffer,g_k230_uart,"k230");
    if (result) {
        return result;
    }
    g_k230_ano = &s_k230_st.base;

    return 0;
}

SYS_INIT(ano_board_init,APPLICATION,INIT_2);