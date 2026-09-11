#include "uart_base.h"
#include "uart_true.h"
#include "zephyr/device.h"
#include "zephyr/init.h"
#include <sys/_intsup.h>
#include "event/event.h"
#define RING_TX_LEN 512
#define RING_RX_LEN 512
#define TX_LEN 30
#define RX_LEN 30

static struct uart_device_t s_uart_computer;
static uint8_t s_com_ring_rx[RING_RX_LEN];
static uint8_t s_com_ring_tx[RING_TX_LEN];
static uint8_t s_com_tx[TX_LEN];
static uint8_t s_com_Rx[RX_LEN];

struct uart_base_t* g_uart_computer;


int uart_board_init(void)
{
    
    
    struct uart_cfg_t com_cfg = {
        .uart_device = DEVICE_DT_GET(DT_NODELABEL(usart2)),
        .rx_data = s_com_Rx,
        .tx_data = s_com_tx,
        .rx_ring_data = s_com_ring_rx,
        .tx_ring_data = s_com_ring_tx,
        .tx_len32 = TX_LEN,
        .tx_ring_len32 = RING_TX_LEN,
        .rx_len32 = RX_LEN,
        .rx_ring_len32 = RING_RX_LEN,
    };
    int ret = uart_it_init(&s_uart_computer, &com_cfg, "uart_computer");
    if (ret) {
        return ret;
    }
    g_uart_computer = &s_uart_computer.base;
    
    return 0;
}

SYS_INIT(uart_board_init, APPLICATION, INIT_1);

int uart_receive_enable_all(void)
{
    int ret = uart_receive_enable(g_uart_computer,1000);
    return ret;
}

SYS_INIT(uart_receive_enable_all, APPLICATION, INIT_7);
