/*
 * 菜单输入中间层 — 手势识别 + 动作映射
 *
 * 职责:
 *   1. 订阅 Zephyr input 子系统原始按键事件
 *   2. 识别单击 / 双击 / 长按手势
 *   3. 将 "KEYx + 单击" 映射为菜单语义动作 (UP/DOWN/ENTER/BACK/TOGGLE/RESET)
 *
 * 分层:
 *   Zephyr input (evdev) → menu_input (本层) → menu.c (菜单状态机)
 */

#ifndef __MENU_INPUT_H
#define __MENU_INPUT_H

#include <stdint.h>
#include <sys/_intsup.h>

/* 菜单语义动作 — menu.c 只认识这 6 个, 不关心物理按键或手势细节 */
enum menu_action_e {
	MENU_ACTION_UP_em,      /* 上移光标 / 增加值 */
	MENU_ACTION_DOWN_em,    /* 下移光标 / 减小值 */
	MENU_ACTION_ENTER_em,   /* 进入子项 / 切换编辑模式 */
	MENU_ACTION_BACK_em,    /* 退出编辑 / 返回上级 */
	MENU_ACTION_TOGGLE_em,  /* 双击 ENTER → 循环切换步进值 */
	MENU_ACTION_RESET_em,   /* 长按 ENTER → 恢复默认值 */
};

typedef void (*menu_action_handler_t)(enum menu_action_e action);

/* 初始化: 初始化 k_work (长按/延迟单击定时器) */

/* 注册菜单动作处理函数 (menu.c 在 menu_init_v 里调用) */
void menu_input_register_handler(menu_action_handler_t handler);

#endif /* __MENU_INPUT_H */
