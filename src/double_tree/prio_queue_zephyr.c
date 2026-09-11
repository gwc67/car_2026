/**
 * @file    prio_queue_zephyr.c
 * @brief   Binary heap priority queue implementation (Zephyr RTOS).
 * @author  兆鸣嵌入式
 */

#include "prio_queue_zephyr.h"
#include <zephyr/kernel.h>
#include "event.h"
/* ===== Internal Structure ===== */

struct prio_queue {
    struct event_t  heap[PQ_MAX_CAPACITY];
    uint32_t           size;
    uint32_t           seq_counter;
    struct k_mutex     mutex;       /** 确保当前只有一个线程写入 */  
    struct k_sem       sem;         /**< Counts available events. */
    bool               initialized; /**< Guard against double init. */
};

/* ===== Static Instance ===== */

static struct prio_queue s_pq_instance;

/* ===== Internal Helpers ===== */

/**
 * @brief  Compare two events by priority.
 * @return true if @p a has higher priority than @p b.
 *
 * Higher prio value wins. On equal prio, lower seq wins (FIFO).
 */
static inline bool is_higher_prio(const struct event_t *a,
                                  const struct event_t *b)
{
    if (a->prio > b->prio) {
        return true;
    }
    if (a->prio == b->prio && a->seq < b->seq) {
        return true;
    }
    return false;
}

/**
 * @brief Swap two events in the heap array.
 */
static void heap_swap(struct event_t *a, struct event_t *b)
{
    struct event_t tmp = *a;
    *a = *b;
    *b = tmp;
}

/**
 * @brief Bubble up: restore heap property after insertion.
 * @param pq   Queue instance.
 * @param idx  Index of the newly inserted element.
 */
static void heap_bubble_up(struct prio_queue *pq, uint32_t idx)
{
    while (idx > 0U) {
        uint32_t parent = (idx - 1U) / 2U;

        if (is_higher_prio(&pq->heap[idx], &pq->heap[parent])) {
            heap_swap(&pq->heap[idx], &pq->heap[parent]);
            idx = parent;
        } else {
            break;
        }
    }
}

/**
 * @brief Bubble down: restore heap property after removal.
 * @param pq   Queue instance.
 * @param idx  Index to start bubbling from (usually root = 0).
 */
static void heap_bubble_down(struct prio_queue *pq, uint32_t idx)
{
    while (1) {
        uint32_t left  = (2U * idx) + 1U;
        uint32_t right = (2U * idx) + 2U;
        uint32_t highest = idx;

        if (left < pq->size &&
            is_higher_prio(&pq->heap[left], &pq->heap[highest])) {
            highest = left;
        }
        if (right < pq->size &&
            is_higher_prio(&pq->heap[right], &pq->heap[highest])) {
            highest = right;
        }

        if (highest != idx) {
            heap_swap(&pq->heap[idx], &pq->heap[highest]);
            idx = highest;
        } else {
            break;
        }
    }
}

/* ===== Public API ===== */

prio_queue_t *prio_queue_create(void)
{
    struct prio_queue *pq = &s_pq_instance;

    if (pq->initialized) {
        return pq;
    }

    pq->size        = 0U;
    pq->seq_counter = 0U;

    (void)k_mutex_init(&pq->mutex);
    (void)k_sem_init(&pq->sem, 0U,
                     (uint32_t)PQ_MAX_CAPACITY);

    pq->initialized = true;
    return pq;
}

pq_err_t prio_queue_push(prio_queue_t *pq,
                         const struct event_t *event,
                         k_timeout_t timeout)
{
    int ret;

    if (pq == NULL || event == NULL) {
        return PQ_ERR_NULL_PTR;
    }
    if (!pq->initialized) {
        return PQ_ERR_NOT_INIT;
    }

    /* Wait for space via mutex (acts as backpressure). */
    ret = k_mutex_lock(&pq->mutex, timeout);
    if (ret != 0) {
        return PQ_ERR_TIMEOUT;
    }

    if (pq->size >= (uint32_t)PQ_MAX_CAPACITY) {
        (void)k_mutex_unlock(&pq->mutex);
        return PQ_ERR_FULL;
    }

    pq->heap[pq->size]      = *event;
    pq->heap[pq->size].seq  = pq->seq_counter;
    pq->seq_counter++;

    heap_bubble_up(pq, pq->size);
    pq->size++;

    (void)k_mutex_unlock(&pq->mutex);

    /* Signal that one more event is available. */
    k_sem_give(&pq->sem);

    return PQ_OK;
}

pq_err_t prio_queue_push_from_isr(prio_queue_t *pq,
                                  const struct event_t *event)
{
    unsigned int key;

    if (pq == NULL || event == NULL) {
        return PQ_ERR_NULL_PTR;
    }

    /* Disable interrupts to protect heap in ISR context. */
    key = irq_lock();

    if (pq->size >= (uint32_t)PQ_MAX_CAPACITY) {
        irq_unlock(key);
        return PQ_ERR_FULL;
    }

    pq->heap[pq->size]      = *event;
    pq->heap[pq->size].seq  = pq->seq_counter;
    pq->seq_counter++;

    heap_bubble_up(pq, pq->size);
    pq->size++;

    irq_unlock(key);

    /* Wake up any thread blocked on pop(). */
    k_sem_give(&pq->sem);

    return PQ_OK;
}

pq_err_t prio_queue_pop(prio_queue_t *pq,
                        struct event_t *out_event,
                        k_timeout_t timeout)
{
    int ret;

    if (pq == NULL || out_event == NULL) {
        return PQ_ERR_NULL_PTR;
    }
    if (!pq->initialized) {
        return PQ_ERR_NOT_INIT;
    }

    /* Block until at least one event is available. */
    ret = k_sem_take(&pq->sem, timeout);
    if (ret != 0) {
        return PQ_ERR_TIMEOUT;
    }

    (void)k_mutex_lock(&pq->mutex, K_FOREVER);

    if (pq->size == 0U) {
        (void)k_mutex_unlock(&pq->mutex);
        return PQ_ERR_EMPTY;
    }

    /* Root (index 0) is always the highest priority. */
    *out_event = pq->heap[0];
    pq->size--;

    if (pq->size > 0U) {
        pq->heap[0] = pq->heap[pq->size];
        heap_bubble_down(pq, 0U);
    }

    (void)k_mutex_unlock(&pq->mutex);

    return PQ_OK;
}


//返回当前事件的数量
uint32_t prio_queue_size(const prio_queue_t *pq)
{
    if (pq == NULL) {
        return 0U;
    }
    return pq->size;
}
