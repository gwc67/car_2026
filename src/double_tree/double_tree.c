#include "double_tree.h"
#include "event.h"
#include "zephyr/irq.h"
#include "zephyr/kernel.h"
#include "zephyr/sys/clock.h"
#include "zephyr/syscalls/kernel.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/_types.h>


struct tree_queue {
    struct event_t      heap[TREE_MAX_CAPACITY];
    uint32_t            size;
    uint32_t            seq_counter;
    struct k_mutex      mutex;
    struct k_sem        sem;
    bool                initialized;
};

static struct tree_queue s_tree_instance;

static inline bool is_higher_prio(const struct event_t* a,
                            const struct event_t* b)
{
    if (a->prio > b->prio) {
        return true;
    }
    if (a->prio == b->prio && a->seq < b->seq) {
        return true;
    }
    return false;
}


static void heap_swap(struct event_t* a,struct event_t *b)
{
    struct event_t tmp = *a;
    *a = *b;
    *b = tmp;
}

static void heap_bubble_up(struct tree_queue* pq, uint32_t idx)
{
    while (idx > 0) {
        uint32_t parent = (idx - 1U) / 2U;

        if (is_higher_prio(&pq->heap[idx], &pq->heap[parent])) {
            heap_swap(&pq->heap[idx], &pq->heap[parent]);
            idx = parent;
        }
        else {
            break;
        }
    }
}

//将数组里面最低的拿上来进行bubble_down
static void heap_bubble_down(struct tree_queue* pq,uint32_t idx)
{
    while(1)
    {
        uint32_t left = (2U * idx)  + 1U;
        uint32_t right = (2U * idx) + 2U;
        uint32_t highest = idx;

        //left 的 idx 小于 pq.size （越大优先级越低，pq.size是最大的）
        // 左 比 右 大 先执行 第一个if 交换 左   发现左比右大不执行，最后将左大放到头上
        // 左 比 右 小  先执行 第一个if 交换 左  发现右会比左大，执行，最后将右大放到头上
        // 左 = 右  同一
        if (left < pq->size && 
            is_higher_prio(&pq->heap[left], &pq->heap[highest])) {
            highest = left;
        }
        if (right < pq->size && is_higher_prio(&pq->heap[right], &pq->heap[highest])) {
            highest = right;
        }
        if (highest != idx) {
            heap_swap(&pq->heap[idx], &pq->heap[highest]);
            idx = highest;
        }
        else {
            break;
        }
    }
}

tree_queue_t * tree_queue_create(void)
{
    struct tree_queue* tree = &s_tree_instance;

    if (tree->initialized) {
        return tree;
    }

    tree->size = 0U;
    tree->size = 0U;

    k_mutex_init(&tree->mutex);
    k_sem_init(&tree->sem, 0U, (uint32_t)TREE_MAX_CAPACITY);

    tree->initialized = true;
    return tree;
}

tree_err_t tree_queue_push(tree_queue_t* tree,const struct event_t* event,k_timeout_t timeout)
{
    int ret;

    if (tree == NULL || event == NULL) {
        return TREE_ERR_NULL_PTR;
    }
    if (!tree->initialized) {
        return TREE_ERR_NOT_INIT;
    }

    ret = k_mutex_lock(&tree->mutex, timeout);

    if (ret) {
        return TREE_ERR_TIMEOUT;
    }

    if (tree->size >= (uint32_t)TREE_MAX_CAPACITY) {
        k_mutex_unlock(&tree->mutex);
        return TREE_ERR_FULL;
    }

    tree->heap[tree->size]   = *event;
    tree->heap[tree->size].seq = tree->seq_counter;
    tree->seq_counter++;

    heap_bubble_up(tree, tree->size);
    tree->size++;
    k_mutex_unlock(&tree->mutex);

    k_sem_give(&tree->sem);

    return TREE_OK;
}


//中断和线程最多的区别就是一个是互斥锁，一个是关开中断
tree_err_t tree_queue_push_from_isr(tree_queue_t* tree,const struct event_t* event)
{
    unsigned int key;

    if (tree == NULL || event == NULL) {
        return TREE_ERR_NULL_PTR;
    }
    if (!tree->initialized) {
        return TREE_ERR_NOT_INIT;
    }

    key = irq_lock();

    if (tree->size >= (uint32_t)TREE_MAX_CAPACITY) {
        irq_unlock(key);
        return TREE_ERR_FULL;
    }

    tree->heap[tree->size]   = *event;
    tree->heap[tree->size].seq = tree->seq_counter;
    tree->seq_counter++;
     
    heap_bubble_up(tree, tree->size);
    tree->size++;

    irq_unlock(key);

    k_sem_give(&tree->sem);

    return TREE_OK;
}

tree_err_t tree_queue_pop(tree_queue_t *tree, struct event_t *out_event, k_timeout_t timeout)
{
    int ret;
    if (tree == NULL || out_event == NULL) {
        return TREE_ERR_NULL_PTR;
    }
    if (!tree->initialized) {
        return TREE_ERR_NOT_INIT;
    }

    ret = k_sem_take(&tree->sem, timeout);
    if (ret) {
        return TREE_ERR_TIMEOUT;
    }

    k_mutex_lock(&tree->mutex, K_FOREVER);

    if (tree->size == 0U) {
        k_mutex_unlock(&tree->mutex);
        return TREE_ERR_EMPTY;
    }

    *out_event = tree->heap[0];
    tree->size--;

    if (tree->size > 0U) {
        tree->heap[0] = tree->heap[tree->size];
        heap_bubble_down(tree, 0U);
    }

    k_mutex_unlock(&tree->mutex);

    return TREE_OK;
    
}


uint32_t tree_queue_size(const tree_queue_t* tree)
{
    if (tree == NULL) {
        return 0U;
    }
    return tree->size;
}
