/*
 * 树形菜单系统 — 核心实现 (Zephyr 移植版)
 *
 * 功能:
 *   1. 树导航: UP/DOWN 循环, ENTER 进入, BACK 返回 (带光标恢复)
 *   2. 滚动列表: 超过 MENU_VISIBLE_ROWS 项时自动滚动
 *   3. 编辑模式: ENTER 进入 → UP/DOWN 改值 → TOGGLE 切步进 → BACK 退出
 *   4. int32 / float 双类型支持
 *
 * 按键: 通过 menu_input 中间层获取语义动作 (MENU_ACTION_UP 等),
 *       不感知物理按键或手势识别细节。
 * 显示: Zephyr cfb 字符帧缓冲 (cfb_print / cfb_framebuffer_finalize)。
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <zephyr/device.h>
#include <zephyr/display/cfb.h>

#include "menu.h"
#include "menu_input.h"
 
/* ================================================================
 * 静态变量
 * ================================================================ */

/* 步进值表 */
const float s_menu_steps[MENU_STEP_COUNT] = { 0.01f, 0.1f, 1.0f, 10.0f, 100.0f };

static struct menu_node_t *s_current_pst = NULL;
static volatile bool       s_dirty_b      = false;
static struct menu_node_t *s_nav_stack[8];
static int                 s_nav_top      = -1;
static bool                s_editing      = false;

/* OLED 显示设备 (cfb) */
static const struct device *s_oled_pst =
    DEVICE_DT_GET(DT_NODELABEL(ssd1306));

/* 前向声明 */
static void s_menu_folder_draw(struct menu_node_t *node);

/* ================================================================
 * 辅助函数
 * ================================================================ */

static bool s_has_data(struct menu_node_t *n)
{
    return (n->data_type == DATA_INT && n->data != NULL)
        || (n->data_type == DATA_FLOAT && n->data_f != NULL);
}

static float s_get_step(struct menu_node_t *n)
{
    return s_menu_steps[n->step_idx % MENU_STEP_COUNT];
}

static void s_adjust_plus(struct menu_node_t *n)
{
    float step = s_get_step(n);
    if (n->data_type == DATA_INT && n->data) {
        int32_t v = (int32_t)step;
        if (v < 1) v = 1;
        *n->data += v;
        if (n->data_max != 0 && *n->data > n->data_max)
            *n->data = n->data_max;
    } else if (n->data_type == DATA_FLOAT && n->data_f) {
        *n->data_f += step;
        if (n->data_max != 0 && *n->data_f > (float)n->data_max)
            *n->data_f = (float)n->data_max;
    }
}

static void s_adjust_minus(struct menu_node_t *n)
{
    float step = s_get_step(n);
    if (n->data_type == DATA_INT && n->data) {
        int32_t v = (int32_t)step;
        if (v < 1) v = 1;
        *n->data -= v;
        if (n->data_min != 0 && *n->data < n->data_min)
            *n->data = n->data_min;
    } else if (n->data_type == DATA_FLOAT && n->data_f) {
        *n->data_f -= step;
        if (n->data_min != 0 && *n->data_f < (float)n->data_min)
            *n->data_f = (float)n->data_min;
    }
}

/*
 * 手动格式化浮点数 (避免 %f — picolibc 默认不支持浮点 printf)
 * 用法: format_fixed(buf, size, 3.14f, 2) → "3.14"
 */
static void format_fixed(char *buf, int bufsize, float v, int decimals)
{
    bool neg = (v < 0.0f);
    if (neg) v = -v;

    int int_part = (int)v;
    int multiplier = 1;
    for (int i = 0; i < decimals; i++) multiplier *= 10;
    int frac_part = (int)((v - (float)int_part) * (float)multiplier + 0.5f);

    /* 处理四舍五入进位 (e.g. 0.996 → 1.00) */
    if (frac_part >= multiplier) {
        int_part++;
        frac_part -= multiplier;
    }

    if (neg) {
        snprintf(buf, bufsize, "-%d.%0*d", int_part, decimals, frac_part);
    } else {
        snprintf(buf, bufsize, "%d.%0*d", int_part, decimals, frac_part);
    }
}

static void s_format_value(struct menu_node_t *n, char *buf, uint8_t bufsize)
{
    if (n->data_type == DATA_INT && n->data) {
        snprintf(buf, bufsize, "%d", (int)*n->data);
    } else if (n->data_type == DATA_FLOAT && n->data_f) {
        float v = *n->data_f;
        if (v == (int)v)
            snprintf(buf, bufsize, "%d", (int)v);
        else
            format_fixed(buf, bufsize, v, 2);
    } else {
        buf[0] = '-'; buf[1] = '-'; buf[2] = '\0';
    }
}

static uint8_t s_count_children(struct menu_node_t *node)
{
    uint8_t n = 0;
    struct menu_node_t *c = node->first_child;
    while (c) { n++; c = c->next; }
    return n;
}

/* ================================================================
 * 树构建
 * ================================================================ */

struct menu_base_t *menu_create_item(struct menu_node_t *parent,
                                     struct menu_node_t *me,
                                     const char *name,
                                     enum menu_kind_e kind,
                                     void (*draw)(struct menu_node_t *))
{
    me->base.name = name;
    me->kind = kind;
    me->draw = draw;
    me->parent = parent;
    me->first_child = NULL;
    me->next = NULL;
    me->cursor = 0;
    me->view_offset = 0;

    me->data = NULL;
    me->data_f = NULL;
    me->data_type = DATA_INT;
    me->step_idx = 1;
    me->data_min = 0;
    me->data_max = 0;
    *me->data = 0;
    *me->data_f = 0.0f;
    /* INIT_LIST_HEAD(&me->list); -- Zephyr 下暂不需要 */

    if (parent) {
        if (!parent->first_child) {
            parent->first_child = me;
        } else {
            struct menu_node_t *p = parent->first_child;
            while (p->next) p = p->next;
            p->next = me;
        }
    }

    if (!s_current_pst)
        s_current_pst = me;

    return &me->base;
}

/* ================================================================
 * 菜单动作处理 — 纯菜单逻辑, 不感知物理按键或手势
 * ================================================================ */

static void s_menu_action_handler(enum menu_action_e action)
{
    if (!s_current_pst) return;

    struct menu_node_t *target;

    switch (action) {

    case MENU_ACTION_DOWN_em:
        if (s_editing && s_has_data(s_current_pst)) {
            s_adjust_minus(s_current_pst);
            s_dirty_b = true;
        } else if (s_current_pst->first_child) {
            uint8_t count = s_count_children(s_current_pst);
            if (count > 0) {
                s_current_pst->cursor =
                    (s_current_pst->cursor + 1) % count;
                if (s_current_pst->cursor >=
                    s_current_pst->view_offset + MENU_VISIBLE_ROWS)
                    s_current_pst->view_offset =
                        s_current_pst->cursor - MENU_VISIBLE_ROWS + 1;
                if (s_current_pst->cursor < s_current_pst->view_offset)
                    s_current_pst->view_offset = s_current_pst->cursor;
                s_dirty_b = true;
            }
        }
        break;

    case MENU_ACTION_UP_em:
        if (s_editing && s_has_data(s_current_pst)) {
            s_adjust_plus(s_current_pst);
            s_dirty_b = true;
        } else if (s_current_pst->first_child) {
            uint8_t count = s_count_children(s_current_pst);
            if (count > 0) {
                s_current_pst->cursor = (s_current_pst->cursor == 0)
                    ? count - 1
                    : s_current_pst->cursor - 1;
                if (s_current_pst->cursor < s_current_pst->view_offset)
                    s_current_pst->view_offset = s_current_pst->cursor;
                if (s_current_pst->cursor >=
                    s_current_pst->view_offset + MENU_VISIBLE_ROWS)
                    s_current_pst->view_offset =
                        s_current_pst->cursor - MENU_VISIBLE_ROWS + 1;
                s_dirty_b = true;
            }
        }
        break;

    case MENU_ACTION_ENTER_em:
        if (s_current_pst->first_child) {
            target = s_current_pst->first_child;
            for (uint8_t i = 0; i < s_current_pst->cursor && target; i++)
                target = target->next;
            if (target) {
                if (s_nav_top < 7)
                    s_nav_stack[++s_nav_top] = s_current_pst;
                s_current_pst = target;
                s_editing = false;
                s_dirty_b = true;
            }
        } else if (s_has_data(s_current_pst)) {
            s_editing = !s_editing;
            s_dirty_b = true;
        }
        break;

    case MENU_ACTION_BACK_em:
        if (s_editing) {
            s_editing = false;
            s_dirty_b = true;
        } else if (s_nav_top >= 0) {
            s_current_pst = s_nav_stack[s_nav_top--];
            s_dirty_b = true;
        } else if (s_current_pst->parent) {
            s_current_pst = s_current_pst->parent;
            s_dirty_b = true;
        }
        break;

    case MENU_ACTION_TOGGLE_em:
        if (s_editing && s_has_data(s_current_pst)) {
            s_current_pst->step_idx =
                (s_current_pst->step_idx + 1) % MENU_STEP_COUNT;
            s_dirty_b = true;
        }
        break;

    case MENU_ACTION_RESET_em:
        if (s_has_data(s_current_pst)) {
            menu_reset_to_default(s_current_pst);
            s_dirty_b = true;
        }
        break;
    }
}

/* ================================================================
 * 初始化 + API
 * ================================================================ */

int menu_init_v(void)
{
    cfb_framebuffer_init(s_oled_pst);                           //初始化OLED设备  
    menu_input_register_handler(s_menu_action_handler);
    s_dirty_b = true;
    return 0;
}

SYS_INIT(menu_init_v, APPLICATION, 10);


void menu_navigate_v(struct menu_node_t *target)
{
    if (!target) return;
    s_current_pst = target;
    s_dirty_b = true;
}

struct menu_node_t *menu_current_get_pst(void)
{
    return s_current_pst;
}

bool menu_leaf_is_editing(void)
{
    return s_editing;
}

void menu_reset_to_default(struct menu_node_t *n)
{
    if (!n) return;
    if (n->data_type == DATA_INT && n->data) {
        *n->data = n->data_default;
    } else if (n->data_type == DATA_FLOAT && n->data_f) {
        *n->data_f = n->data_f_default;
    }
}

void menu_request_refresh(struct menu_base_t *base)
{
    struct menu_node_t *me = to_menu_node(base);
    if (base && s_current_pst == me)
        s_dirty_b = true;
}

/* ================================================================
 * 显示 — Zephyr cfb
 * ================================================================ */

/* 辅助: 画单个字符 (cfb 没有单字符 API, 用 2 字节 buf) */
static void s_cfb_char(uint8_t x, uint8_t y, char ch)
{
    char buf[2] = { ch, '\0' };
    cfb_print(s_oled_pst, buf, x, y);
}

static void s_menu_folder_draw(struct menu_node_t *node)
{
    struct menu_node_t *child;
    uint8_t i, row;
    uint8_t total;

    /* 标题 (font 0: 10x16) */
    cfb_framebuffer_set_font(s_oled_pst, 0);
    cfb_print(s_oled_pst, node->base.name, 0, 0);

    /* 定位到 view_offset */
    child = node->first_child;
    for (i = 0; i < node->view_offset && child; i++)
        child = child->next;

    /* 子项 (font 0: 10x16, 行距 16) */
    total = s_count_children(node);
    row = 0;
    while (child && row < MENU_VISIBLE_ROWS) {
        uint8_t y = 18 + row * 16;
        uint8_t item_idx = node->view_offset + row;

        /* 光标 */
        if (item_idx == node->cursor)
            cfb_print(s_oled_pst, ">>", 0, y);
        else
            cfb_print(s_oled_pst, "  ", 0, y);

        /* 类型指示 */
        if (child->first_child)
            cfb_print(s_oled_pst, "[+]", 14, y);
        else if (s_has_data(child))
            cfb_print(s_oled_pst, "[#]", 14, y);
        else
            cfb_print(s_oled_pst, " > ", 14, y);

        cfb_print(s_oled_pst, child->base.name, 32, y);

        child = child->next;
        row++;
    }

    /* 滚动指示器 */
    if (total > MENU_VISIBLE_ROWS) {
        if (node->view_offset > 0)
            s_cfb_char(120, 18, '^');
        if (node->view_offset + MENU_VISIBLE_ROWS < total)
            s_cfb_char(120, 18 + (MENU_VISIBLE_ROWS - 1) * 16, 'v');
    }
}

void menu_task_v(void)
{
    if (!s_dirty_b || !s_current_pst) return;
    s_dirty_b = false;

    cfb_framebuffer_clear(s_oled_pst, false);

    if (s_current_pst->first_child) {
        s_menu_folder_draw(s_current_pst);
    } else {
        if (s_current_pst->draw) {
            s_current_pst->draw(s_current_pst);
        } else {
            cfb_framebuffer_set_font(s_oled_pst, 0);
            cfb_print(s_oled_pst, s_current_pst->base.name, 0, 24);
        }

        /* 底部: 值 + 步进 + 模式 */
        if (s_has_data(s_current_pst)) {
            cfb_framebuffer_set_font(s_oled_pst, 0);

            char buf[20];
            s_format_value(s_current_pst, buf, sizeof(buf));
            cfb_print(s_oled_pst, buf, 0, 48);

            if (s_editing) {
                char step_buf[12];
                float step = s_get_step(s_current_pst);
                if (step == (int)step)
                    snprintf(step_buf, sizeof(step_buf), "s:%d", (int)step);
                else {
                    /* 用 format_fixed 避免 %f */
                    char val_buf[8];
                    format_fixed(val_buf, sizeof(val_buf), step, 2);
                    snprintf(step_buf, sizeof(step_buf), "s:%s", val_buf);
                }
                cfb_print(s_oled_pst, step_buf, 60, 48);
                cfb_print(s_oled_pst, "EDIT", 100, 48);
            } else {
                cfb_print(s_oled_pst, "[ENT]", 72, 48);
            }
        }
    }

    cfb_framebuffer_finalize(s_oled_pst);
}
