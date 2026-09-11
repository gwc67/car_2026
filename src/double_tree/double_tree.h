#ifndef __DOUBLE_TREE_H
#define __DOUBLE_TREE_H


#include "event.h"
#include "zephyr/sys/clock.h"
#include <stdint.h>
typedef enum {
    TREE_OK                 = 0,
    TREE_ERR_NULL_PTR       = -1,
    TREE_ERR_FULL           = -2,
    TREE_ERR_EMPTY          = -3,
    TREE_ERR_TIMEOUT        = -4,
    TREE_ERR_NOT_INIT       = -5,
}tree_err_t;

#ifndef TREE_MAX_CAPACITY 
#define TREE_MAX_CAPACITY (64U)
#endif

typedef struct tree_queue tree_queue_t ;

tree_queue_t *tree_queue_create(void);

tree_err_t tree_queue_push(tree_queue_t* tree,const struct event_t *event, k_timeout_t timeout);

tree_err_t tree_queue_push_from_isr(tree_queue_t* tree,const struct event_t *event);

tree_err_t tree_queue_pop(tree_queue_t *tree, struct event_t *out_event,
                          k_timeout_t timeout);
uint32_t tree_queue_size(const tree_queue_t* tree);

#endif
