#include "double_tree.h"
#include "event.h"
#include "zephyr/init.h"
#include "zephyr/kernel.h"
#include "zephyr/toolchain.h"

#define TIMER_10MS_PERIOD_MS (10U)
#define TIMER_500MS_PERIOD_MS (500U)
#define TIMER_1000MS_PERIOD_MS (1000U)

static struct k_work_delayable s_work_10ms;
static struct k_work_delayable s_work_500ms;
static struct k_work_delayable s_work_1000ms;

extern tree_queue_t * tree;


static void s_work_10ms_handler(struct k_work* work)
{
    struct event_t evt;
    ARG_UNUSED(work);

    evt.id = EVT_TIMER_10MS;
    evt.prio = EVT_PRIO_LOW;
    evt.param = 0U;
    evt.seq = 0U;

    tree_queue_push(tree, &evt, K_NO_WAIT);
    k_work_schedule(&s_work_10ms,K_MSEC(TIMER_10MS_PERIOD_MS));
    
}
static void s_work_500ms_handler(struct k_work* work)
{
    struct event_t evt;
    ARG_UNUSED(work);

    evt.id = EVT_TIMER_500MS;
    evt.prio = EVT_PRIO_HIGH;
    evt.param = 0U;
    evt.seq = 0U;

    tree_queue_push(tree, &evt, K_NO_WAIT);
    k_work_schedule(&s_work_500ms,K_MSEC(TIMER_500MS_PERIOD_MS));
    
}

static void s_work_1000ms_handler(struct k_work* work)
{
    struct event_t evt;
    ARG_UNUSED(work);

    evt.id = EVT_TIMER_1000MS;
    evt.prio = EVT_PRIO_LOW;
    evt.param = 0U;
    evt.seq = 0U;

    tree_queue_push(tree, &evt, K_NO_WAIT);
    k_work_schedule(&s_work_1000ms,K_MSEC(TIMER_1000MS_PERIOD_MS));
    
}

static int soft_timer_init(void)
{
    k_work_init_delayable(&s_work_10ms, s_work_10ms_handler);
    k_work_init_delayable(&s_work_500ms, s_work_500ms_handler);
    k_work_init_delayable(&s_work_1000ms, s_work_1000ms_handler);

    k_work_schedule(&s_work_10ms,K_MSEC(TIMER_10MS_PERIOD_MS));
    k_work_schedule(&s_work_500ms,K_MSEC(TIMER_500MS_PERIOD_MS));
    k_work_schedule(&s_work_1000ms,K_MSEC(TIMER_1000MS_PERIOD_MS));

    return 0;
}

SYS_INIT(soft_timer_init, APPLICATION, INIT_3);