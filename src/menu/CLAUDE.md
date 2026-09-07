# Menu — 树形菜单系统（Zephyr 移植版）

## 分层架构

```
Zephyr input (原始 press/release)
    │
    ▼
menu_input.c/h  ──  手势识别 + 动作映射  ──  6 个语义动作
                       (单击/双击/长按)       (UP/DOWN/ENTER/BACK/TOGGLE/RESET)
    │
    ▼
menu.c/h  ──  纯菜单状态机
              导航、光标滚动、滚动列表、编辑模式、值增减、显示
    │
    ▼
OLED_Menu.c/h  ──  菜单树定义 + 叶子 draw 函数
                   用户只负责声明树结构和写叶子画法
```

**核心原则**：每层只做自己的事。menu.c 不知道物理按键和毫秒阈值，OLED_Menu.c 不知道导航逻辑，menu_input.c 不知道菜单树结构。

## 文件清单

| 文件 | 层 | 职责 |
|------|-----|------|
| `menu_input.h` | 输入中间层 | 定义 `enum menu_action_e`（6 个动作）、handler typedef、3 个 API |
| `menu_input.c` | 输入中间层 | `INPUT_CALLBACK_DEFINE` 订阅原始事件 → 手势识别 → `key_map[]` 映射为动作 → 通知 menu.c |
| `menu.h` | 菜单核心 | `menu_base_t`/`menu_node_t` 数据结构、`container_of` 下转型、`Create_Menu_*` 构建宏、框架 API |
| `menu.c` | 菜单核心 | 树构建、动作处理（`s_menu_action_handler`）、光标滚动、编辑模式、cfb 显示 |
| `OLED_Menu.h` | 用户层 | `g_root` extern、`menu_oled_init_v()` 声明 |
| `OLED_Menu.c` | 用户层 | 菜单树构建（`Create_Menu_*`）+ 叶子 draw 函数（cfb API） |

## menu_input — 按键映射与手势

### 手势阈值

- 双击：同一键 400ms 内再次按下
- 长按：按住超过 800ms（在 `menu_input_poll()` 中检测，主循环每 30ms 调用一次）

### 按键 → 动作映射表 (`s_key_map_pst[]`)

改按键布局只改这个表：

```c
static const struct key_mapping_t s_key_map_pst[] = {
    { INPUT_KEY_0,  MENU_ACTION_UP_em,      NO_ACTION,       NO_ACTION },
    { INPUT_KEY_1,  MENU_ACTION_DOWN_em,    NO_ACTION,       NO_ACTION },
    { INPUT_KEY_2,  MENU_ACTION_ENTER_em,   MENU_ACTION_TOGGLE_em, MENU_ACTION_RESET_em },
    { INPUT_KEY_3,  MENU_ACTION_BACK_em,    NO_ACTION,       NO_ACTION },
};
```

三列依次是：单击动作、双击动作、长按动作。`NO_ACTION`（0xFF）= 该手势无动作。

### `menu_input_poll()` 的必要性

单击和双击可以纯在中断回调里判断，但长按需要"按住不放且持续 800ms"——这需要周期检查。所以主循环里必须周期性调用 `menu_input_poll()`。当前主循环用 `k_sleep(K_MSEC(30))`，每秒约 33 次轮询，长按响应延迟 ≤ 30ms。

## menu — 树形菜单核心

### 数据结构

两层 struct 继承，与 Linux 内核 `struct device` 同款模式：

```c
struct menu_base_t {              // 基类 — 对外暴露的最小接口
    const char *name;
};

struct menu_node_t {              // 派生类 — 框架内部完整实现
    struct menu_base_t base;      // 基类嵌入（必须是第一个成员）
    enum menu_kind_e kind;
    void (*draw)(struct menu_node_t *self);
    struct menu_node_t *parent, *first_child, *next;
    uint8_t cursor, view_offset;
    int32_t *data;  float *data_f;
    // ...
};
```

`container_of` 下转型：`to_menu_node(base_ptr)` 从基类指针恢复完整节点指针。

### 节点类型

| 类型 | 特征 | 显示方式 |
|------|------|---------|
| `MENU_FOLDER` | `first_child != NULL` | 框架自动显示子项列表 + 光标 `>>` |
| `MENU_LEAF` | `first_child == NULL` | 调用 `draw()` 自定义显示 |

### 文件夹通用显示布局（128×64 OLED）

```
(0, 0)   标题 (font 0: 10×16)
(0, 18)  子项 row 0  (font 0: 10×16, 行距 16)
(0, 34)  子项 row 1
(0, 50)  子项 row 2
```

子项标记：`[+]` 文件夹，`[#]` 可编辑叶子，` > ` 普通叶子。
超过 `MENU_VISIBLE_ROWS`（3）项时自动滚动，右侧显示 `^` / `v`。

### 编辑模式

```
叶子页面 → 单击 ENTER → 进入编辑
  → UP/DOWN 修改值
  → 双击 ENTER 切换步进（s:0.1 → s:1 → s:10 → s:100 → s:0.01 循环）
  → 长按 ENTER 恢复默认值
  → BACK 退出编辑
```

屏幕底部显示：当前值 + 步进（如 `s:1`）+ 模式（`EDIT` / `[ENT]`）。

### 导航模型

```
s_current_pst = 当前视图节点
  有子节点 → s_menu_folder_draw() 自动显示子项列表
  无子节点 → draw() 自定义显示

UP/DOWN → cursor ± 1（循环），自动更新 view_offset（滚动）
ENTER   → 进入第 cursor 个子项（压入 s_nav_stack）
BACK    → 弹出导航栈 / 返回父节点
```

### 刷新机制

事件驱动，无轮询。两个触发源：
1. 按键动作 → `s_dirty_b = true`
2. 外部数据更新 → `menu_request_refresh(base)` → 只有当前显示那个节点时才置脏

## OLED_Menu — 菜单树定义

### 构建 API（`Create_Menu_*` 宏）

约定：`me` 传结构体本身（不传指针），宏内部 `&(me)` 取地址。

| 宏 | 用途 |
|----|------|
| `Create_Menu_Folder(parent, me, name)` | 文件夹 |
| `Create_Menu_Leaf(parent, me, name, draw_fn)` | 普通叶子 |
| `Create_Menu_Leaf_Int(parent, me, name, draw_fn, ptr)` | int32 可编辑 |
| `Create_Menu_Leaf_Range(parent, me, name, draw_fn, ptr, min, max)` | int32 可编辑 + 范围 |
| `Create_Menu_Leaf_Float(parent, me, name, draw_fn, ptr)` | float 可编辑 |
| `Create_Menu_Leaf_Float_Range(parent, me, name, draw_fn, ptr, min, max)` | float 可编辑 + 范围 |

`menu_create_item()` 返回 `struct menu_base_t *`，保存此指针用于 `menu_request_refresh()`。

### 叶子 draw 函数规范

- 使用 `cfb_print()` / `cfb_framebuffer_set_font()`（Zephyr cfb API）
- draw 函数内不要调用 `cfb_framebuffer_clear()` 或 `cfb_framebuffer_finalize()`（由 `menu_task_v` 统一管理）
- 底部值 + 编辑状态由 `menu_task_v` 在 draw 之后自动绘制
- 函数内不要有阻塞操作或 `k_sleep`

### 外部数据源实时刷新

```c
// 创建时保存 base 指针
g_battery_pst = Create_Menu_Leaf(&g_select, g_battery, "Battery", s_draw_battery);

// 数据更新后请求刷新（只在当前页面匹配时才触发）
void on_battery_data_ready(void) {
    menu_request_refresh(g_battery_pst);
}
```

## 初始化流程（main.c）

```c
cfb_framebuffer_init(dev);     // 分配帧缓冲
menu_init_v();                  // 注册菜单动作处理器
menu_oled_init_v();             // 构建菜单树

while (1) {
    menu_task_v();              // 脏标记驱动刷新显示
    menu_input_poll();          // 长按检测
    k_sleep(K_MSEC(30));
}
```

## 添加新菜单项的步骤

1. 在 `OLED_Menu.c` 声明 `static struct menu_node_t g_xxx;`
2. 写 `static void s_draw_xxx(struct menu_node_t *self)` —— 用 cfb API
3. 在 `s_build_menu_tree()` 里用 `Create_Menu_*` 挂到树上
4. 如果是可编辑数据：声明 `static int32_t/float s_xxx = 默认值;`，用 `Create_Menu_Leaf_Range/Float` 绑定

## 改按键布局

只改 `menu_input.c` 里的 `s_key_map_pst[]` 表。DTS 里 `zephyr,code` 对应 `INPUT_KEY_N`，映射到此表的 `code_us` 字段。

## 注意事项

1. **`cfb_framebuffer_set_font` 是全局的** — 切换字体会影响后续所有 `cfb_print`。`s_menu_folder_draw` 统一用 font 0（10×16）。
2. **`cfb_print` 的 x,y 是像素坐标** — cfb 内部处理了 tile 偏移，不需要对齐到 8 的倍数。
3. **`INIT_LIST_HEAD(&me->list)` 已注释** — `list` 字段在 Zephyr 移植中暂未使用，后续如需链表操作可用 `<zephyr/sys/slist.h>`。
4. **`__int32_t` / `__uint8_t` 已替换为 `int32_t` / `uint8_t`** — 旧代码是 Keil/ARMCC 扩展，Zephyr gcc 用标准 `<stdint.h>`。
5. **`DRIVER_INIT()` 已移除** — Zephyr 下改为 main() 中手动调用 `menu_oled_init_v()`。
6. **draw 函数中不要阻塞** — 叶子 draw 在 `menu_task_v()` 上下文中执行，阻塞会影响按键响应和显示刷新。
