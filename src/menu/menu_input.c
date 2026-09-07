/*
 * 菜单输入中间层 — 实现 (全中断驱动, 零轮询)
 *
 * ── 与 Linux input 子系统的对应 ──
 *   Zephyr INPUT_CALLBACK_DEFINE  →  Linux evdev 原始事件
 *   k_work_delayable              →  Linux timer + workqueue
 *   notify(action)                →  应用层消费语义事件
 *
 * 手势识别:
 *   - 单击: 无双击动作的键 → 按下立即触发
 *          有双击动作的键 → 延迟 400ms (用 k_work), 窗口内无第二次按下才触发
 *   - 双击: 窗口内同键再按 → 取消延迟单击, 触发双击
 *   - 长按: 按下后 800ms 仍按住 → k_work 触发长按动作; 提前释放则取消
 */

#include "menu_input.h"
#include <sys/_intsup.h>
#include <zephyr/input/input.h>
#include <zephyr/kernel.h>
#include "key/key.h"

/* ---- 状态回调函数，目前只有一个 ---- */
static menu_action_handler_t s_handler_st;

void menu_input_callback(enum key_id_e key_id_em,enum key_event_e event)
{
	switch (key_id_em) {
		case KEY_0_em:
		{
			if (event == KEY_EVENT_SINGLE_em) {
				s_handler_st(MENU_ACTION_UP_em);
			}
			else if (event == KEY_EVENT_REPEAT_em) {
				s_handler_st(MENU_ACTION_UP_em);
			}
		}
		break;
		case KEY_1_em:
		{
			if (event == KEY_EVENT_SINGLE_em) {
				s_handler_st(MENU_ACTION_DOWN_em);
			}
			else if (event == KEY_EVENT_REPEAT_em) {
				s_handler_st(MENU_ACTION_DOWN_em);
			}
		}
		break;
		case KEY_2_em:
		{
			if (event == KEY_EVENT_SINGLE_em) {
				s_handler_st(MENU_ACTION_ENTER_em);
			}
			else if (event == KEY_EVENT_DOUBLE_em) {
				s_handler_st(MENU_ACTION_TOGGLE_em);
			}
			else if (event == KEY_EVENT_LONG_em) {
				s_handler_st(MENU_ACTION_RESET_em);
			}
		}
		break;
		case KEY_3_em:
		{
			if (event == KEY_EVENT_SINGLE_em) {
				s_handler_st(MENU_ACTION_BACK_em);
			}
		}
		break;
		default:
		break;
	}
}

KEY_SUBSCRIBE_DEFINE(menu_input, menu_input_callback, "menu_input");

void menu_input_register_handler(menu_action_handler_t handler)
{
	s_handler_st = handler;
}
