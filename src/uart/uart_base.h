
#ifndef     __UART_BASE_H
#define     __UART_BASE_H

#include <stdint.h>
#include "stdbool.h"
typedef struct uart_base_t uart_base_t ;

enum uart_event_type_e
{
    UART_EVENT_TX_REQ  = 0,
    UART_EVENT_TX_DONE,
    UART_EVENT_RX_DATA,
};

typedef  int (*uart_user_cb_t)(uint8_t* data,uint32_t len32,void* user_data);

typedef struct {
    int (*uart_transmit)(uart_base_t* me,uint8_t* data ,uint32_t len32);
    int (*uart_rx_enable)(uart_base_t* me);                       
    int (*uart_rx_analyze)(uart_base_t*me);
    int (*uart_register_callback)(uart_base_t* me,uart_user_cb_t callback,void* user_data);
    int (*uart_tx_callback)(struct uart_base_t* base,enum uart_event_type_e event);
}uart_ops_t;

struct uart_base_t {
   const char* name;
   const uart_ops_t* ops;
};

//使用uart_event_t 创建RX一个消息队列
struct uart_event_t {
    enum uart_event_type_e type_e;
    struct uart_base_t* base;
    bool is_busy_b;    
};



int uart_transmit(uart_base_t* me,uint8_t* data_puc ,uint32_t len32);
int uart_receive_enable(uart_base_t* me);
int uart_register_callback(uart_base_t* me,uart_user_cb_t callback,void* user_data);
int uart_rx_analyze(uart_base_t*me);
#endif
