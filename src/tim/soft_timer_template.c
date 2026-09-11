/**
 * @file    soft_timer_template.c
 * @brief   Zephyr soft timer event publisher template.
 *          Replaces the FreeRTOS pattern:
 *            xTimerCreate -> xTaskNotify -> task_timer_event -> pq_push
 *          With the Zephyr-idiomatic:
 *            k_work_delayable -> work handler -> prio_queue_push
 *
 * @note    This is a REFERENCE TEMPLATE. Do not compile as-is;
 *          adapt to your project structure.
 * @note    Each k_work_delayable reschedules itself in its handler.
 *          The work runs in the system workqueue thread context,
 *          so prio_queue_push() (which uses k_mutex) is safe.
 * @author  兆鸣嵌入式
 */

#include <zephyr/kernel.h>
#include "event.h"
#include "prio_queue_zephyr.h"

/* ===== Configuration ===== */

/** Timer period in milliseconds. */
#define SOFT_TIMER_10MS_PERIOD_MS    (10U)
#define SOFT_TIMER_500MS_PERIOD_MS   (500U)
#define SOFT_TIMER_1000MS_PERIOD_MS  (1000U)

/* ===== Forward Declarations ===== */

static void s_work_10ms_handler(struct k_work *work);
static void s_work_500ms_handler(struct k_work *work);
static void s_work_1000ms_handler(struct k_work *work);

/* ===== Work Items ===== */

/**
 * Each periodic event gets its own k_work_delayable.
 * The work handler runs in system workqueue thread context,
 * so it can safely call prio_queue_push().
 */
static struct k_work_delayable s_work_10ms;
static struct k_work_delayable s_work_500ms;
static struct k_work_delayable s_work_1000ms;

/* ===== External Dependencies ===== */

/**
 * Global priority queue instance.
 * Must be initialized via prio_queue_create() before
 * these soft timers start.
 */
extern prio_queue_t *g_event_queue_pst;

/* ===== Work Handlers ===== */

/**
 * @brief 10ms periodic event handler.
 *        Pushes EVT_TIMER_10MS and reschedules itself.
 * @note  Runs in system workqueue thread context.
 * @author 兆鸣嵌入式
 */
static void s_work_10ms_handler(struct k_work *work)
{
    struct event_t evt;

    ARG_UNUSED(work);

    evt.id    = EVT_TIMER_10MS;
    evt.prio  = EVT_PRIO_LOW;
    evt.param = 0U;
    evt.seq   = 0U;

    (void)prio_queue_push(g_event_queue_pst,
                          &evt, K_NO_WAIT);

    /* Reschedule for next period. */
    k_work_schedule(&s_work_10ms,
                    K_MSEC(SOFT_TIMER_10MS_PERIOD_MS));
}

/**
 * @brief 500ms periodic event handler.
 * @author 兆鸣嵌入式
 */
static void s_work_500ms_handler(struct k_work *work)
{
    struct event_t evt;

    ARG_UNUSED(work);

    evt.id    = EVT_TIMER_500MS;
    evt.prio  = EVT_PRIO_HIGH;
    evt.param = 0U;
    evt.seq   = 0U;

    (void)prio_queue_push(g_event_queue_pst,
                          &evt, K_NO_WAIT);

    k_work_schedule(&s_work_500ms,
                    K_MSEC(SOFT_TIMER_500MS_PERIOD_MS));
}

/**
 * @brief 1000ms periodic event handler.
 * @author 兆鸣嵌入式
 */
static void s_work_1000ms_handler(struct k_work *work)
{
    struct event_t evt;

    ARG_UNUSED(work);

    evt.id    = EVT_TIMER_1000MS;
    evt.prio  = EVT_PRIO_LOW;
    evt.param = 0U;
    evt.seq   = 0U;

    (void)prio_queue_push(g_event_queue_pst,
                          &evt, K_NO_WAIT);

    k_work_schedule(&s_work_1000ms,
                    K_MSEC(SOFT_TIMER_1000MS_PERIOD_MS));
}

/* ===== Initialization ===== */

/**
 * @brief  Initialize and start all soft timer work items.
 * @return 0 on success, negative errno on failure.
 * @note   Call after prio_queue_create() and event_bus_init().
 *         Uses SYS_INIT for automatic startup.
 * @author 兆鸣嵌入式
 */
static int soft_timer_init(void)
{
    k_work_init_delayable(&s_work_10ms,
                          s_work_10ms_handler);
    k_work_init_delayable(&s_work_500ms,
                          s_work_500ms_handler);
    k_work_init_delayable(&s_work_1000ms,
                          s_work_1000ms_handler);

    /* Start the first schedule. Each handler reschedules itself. */
    k_work_schedule(&s_work_10ms,
                    K_MSEC(SOFT_TIMER_10MS_PERIOD_MS));
    k_work_schedule(&s_work_500ms,
                    K_MSEC(SOFT_TIMER_500MS_PERIOD_MS));
    k_work_schedule(&s_work_1000ms,
                    K_MSEC(SOFT_TIMER_1000MS_PERIOD_MS));

    return 0;
}

/**
 * Auto-initialize at APPLICATION level, after event bus (INIT_1)
 * and priority queue are ready.
 */
SYS_INIT(soft_timer_init, APPLICATION, INIT_3);

/* =========================================================================
 * ALTERNATIVE: Using k_timer + k_work (closer to FreeRTOS pattern)
 * =========================================================================
 *
 * If you need timer callbacks in ISR context with deferred processing:
 *
 * static struct k_timer s_timer_500ms;
 * static struct k_work  s_work_500ms;
 *
 * static void s_timer_500ms_expiry(struct k_timer *timer)
 * {
 *     // ISR context — only signal, don't push directly
 *     k_work_submit(&s_work_500ms);
 * }
 *
 * static void s_work_500ms_handler(struct k_work *work)
 * {
 *     // Thread context — safe to push
 *     struct event_t evt = { .id = EVT_TIMER_500MS, ... };
 *     prio_queue_push(g_event_queue_pst, &evt, K_NO_WAIT);
 * }
 *
 * static int timer_alt_init(void)
 * {
 *     k_work_init(&s_work_500ms, s_work_500ms_handler);
 *     k_timer_init(&s_timer_500ms, s_timer_500ms_expiry, NULL);
 *     k_timer_start(&s_timer_500ms, K_MSEC(500), K_MSEC(500));
 *     return 0;
 * }
 *
 * NOTE: k_work_delayable (primary approach above) is simpler and
 * preferred unless you need the ISR callback for precise timing.
 *
 * =========================================================================
 * COMPARISON: FreeRTOS vs Zephyr
 * =========================================================================
 *
 * FreeRTOS:                              Zephyr:
 * ─────────                              ───────
 * xTimerCreate()                   →     k_work_init_delayable()
 * xTimerStart()                    →     k_work_schedule()
 * Timer callback (ISR ctx)         →     Work handler (thread ctx)
 * xTaskNotify(bit)                 →     Not needed (work = notification)
 * xTaskNotifyWait()                →     Handled by workqueue
 * Dedicated task_timer_event       →     System workqueue (shared)
 * pq_push() from task              →     prio_queue_push() from handler
 *
 * ADVANTAGES of k_work_delayable over FreeRTOS pattern:
 * - No dedicated thread needed (uses shared system workqueue)
 * - No manual notification bit management
 * - Work handler runs in thread context (safe for mutex-based APIs)
 * - Lower memory footprint (no extra thread stack)
 * - Self-rescheduling is a single line: k_work_schedule()
 *
 * =========================================================================
 * IMPORTANT NOTE about EVT_TIMER_10MS:
 * =========================================================================
 *
 * In the FreeRTOS code, NOTIFY_BIT_10MS is defined but no 10ms
 * xTimer is created. The 10ms event likely comes from the hardware
 * timer (TIM5 → sem → 1ms thread). If you need a pure software
 * 10ms timer, use the k_work_delayable approach above with
 * SOFT_TIMER_10MS_PERIOD_MS = 10.
 *
 * For hard-real-time 10ms events, prefer the hardware timer path
 * (TIM5 counter ISR) over software timers, as software timers
 * have jitter from workqueue scheduling.
 */
