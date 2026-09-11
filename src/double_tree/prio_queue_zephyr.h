/**
 * @file    prio_queue_zephyr.h
 * @brief   Binary heap priority queue for event dispatch (Zephyr RTOS).
 *          Events with higher priority are dequeued first.
 *          When priorities are equal, FIFO order is preserved
 *          via sequence number.
 * @note    Thread-safe. ISR-safe push variant provided.
 * @author  兆鸣嵌入式
 */

#ifndef PRIO_QUEUE_ZEPHYR_H
#define PRIO_QUEUE_ZEPHYR_H

#include <stdint.h>
#include <zephyr/kernel.h>
#include "event.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ===== Configuration Macros ===== */

/** Maximum number of events the queue can hold. */
#ifndef PQ_MAX_CAPACITY
#define PQ_MAX_CAPACITY  (64U)
#endif

/* ===== Error Codes ===== */

typedef enum {
    PQ_OK             =  0,   /**< Success. */
    PQ_ERR_NULL_PTR   = -1,   /**< NULL pointer argument. */
    PQ_ERR_FULL       = -2,   /**< Queue is full. */
    PQ_ERR_EMPTY      = -3,   /**< Queue is empty. */
    PQ_ERR_TIMEOUT    = -4,   /**< Operation timed out. */
    PQ_ERR_NOT_INIT   = -5,   /**< Queue not initialized. */
} pq_err_t;

/* ===== Event Type ===== */

/**
 * @brief Event structure stored in the priority queue.
 *
 * @note  Adapt this struct to your application needs,
 *        or #include your existing event definition
 *        and remove this one.
 */
// struct event_t {
//     uint32_t id;        /**< Event identifier. */
//     uint32_t prio;      /**< Priority (higher = more urgent). */
//     uint32_t param;     /**< User-defined parameter. */
//     uint32_t seq;       /**< Sequence number (set internally). */
// };

/* ===== Opaque Type ===== */

typedef struct prio_queue prio_queue_t;

/* ===== Public API ===== */

/**
 * @brief  Create and initialize a priority queue instance.
 * @return Pointer to the initialized queue, or NULL on failure.
 * @note   Uses a static internal instance (no heap fragmentation).
 *         Safe to call only once at system startup.
 * @author 兆鸣嵌入式
 */
prio_queue_t *prio_queue_create(void);

/**
 * @brief  Push an event into the priority queue (thread context).
 * @param  pq      Queue instance (must not be NULL).
 * @param  event   Pointer to the event to enqueue (must not be NULL).
 * @param  timeout Max time to wait if queue is full (e.g. K_FOREVER).
 * @return PQ_OK on success, error code otherwise.
 * @note   Thread-safe. Blocks if queue is full until space
 *         is available or timeout expires.
 * @author 兆鸣嵌入式
 */
pq_err_t prio_queue_push(prio_queue_t *pq,
                         const struct event_t *event,
                         k_timeout_t timeout);

/**
 * @brief  Push an event from ISR context (lock-free).
 * @param  pq      Queue instance (must not be NULL).
 * @param  event   Pointer to the event to enqueue (must not be NULL).
 * @return PQ_OK on success, PQ_ERR_FULL if queue is full.
 * @note   Must only be called from ISR context.
 *         Uses irq_lock/unirq to protect heap operations.
 * @author 兆鸣嵌入式
 */
pq_err_t prio_queue_push_from_isr(prio_queue_t *pq,
                                  const struct event_t *event);

/**
 * @brief  Pop the highest-priority event (thread context).
 * @param  pq        Queue instance (must not be NULL).
 * @param  out_event Pointer to receive the dequeued event
 *                   (must not be NULL).
 * @param  timeout   Max time to wait if queue is empty
 *                   (e.g. K_FOREVER).
 * @return PQ_OK on success, error code otherwise.
 * @note   Thread-safe. Blocks if queue is empty until an
 *         event is available or timeout expires.
 * @author 兆鸣嵌入式
 */
pq_err_t prio_queue_pop(prio_queue_t *pq,
                        struct event_t *out_event,
                        k_timeout_t timeout);

/**
 * @brief  Get the current number of events in the queue.
 * @param  pq  Queue instance (must not be NULL).
 * @return Number of queued events, or 0 if pq is NULL.
 * @note   Returns a snapshot; value may change immediately
 *         after return on a 32-bit platform (atomic read).
 * @author 兆鸣嵌入式
 */
uint32_t prio_queue_size(const prio_queue_t *pq);

#ifdef __cplusplus
}
#endif

#endif /* PRIO_QUEUE_ZEPHYR_H */
