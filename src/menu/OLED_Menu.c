/*
 * OLED 菜单树定义 (Zephyr 移植版)
 *
 * 树结构:
 *   g_root (Main)
 *   └─ [Test]
 *      ├─ Hello        ← 叶子: 显示 "Hello World!"
 *      └─ Value        ← 可编辑 int32 (0~100, 步进切换)
 */

#include "OLED_Menu.h"
#include "blinky.h"
#include "encode.h"
#include "menu.h"
#include "mess/value_to_str.h"
#include <stdio.h>
#include <zephyr/device.h>
#include <zephyr/display/cfb.h>
#include <mpu6050/mpu6050.h>

/* OLED 设备 (与 menu.c 共用同一个) */
static const struct device *s_oled_pst =
    DEVICE_DT_GET(DT_NODELABEL(ssd1306));

#define RAD_TO_DEG 57.295779513082320876798154814105
/* ================================================================
 * 叶子 draw 函数
 * ================================================================ */

static void s_draw_speed_a(struct menu_node_t *self)
{
    cfb_framebuffer_set_font(s_oled_pst, 1);
    cfb_print(s_oled_pst, self->base.name, 0, 0);
    cfb_framebuffer_set_font(s_oled_pst, 0);
    char target_speed_pc[10];
    
    
    if (self->data) {
        rtU.target_speed = *(self->data);
    }
    float_to_str(target_speed_pc, sizeof(target_speed_pc),rtU.target_speed, 2);
    cfb_print(s_oled_pst, target_speed_pc, 20, 30);
}

static void s_draw_mpu6050(struct menu_node_t* self)
{
    char num[16];   /* 放数值 */
    char buf[32];   /* 放最终结果 */
    cfb_framebuffer_set_font(s_oled_pst, 0);
    cfb_print(s_oled_pst, self->base.name, 0, 0);
    struct mpu6050_vec3_t mpu6050_vec3_st = {0};
    mpu6050_get_accel(&mpu6050_vec3_st);

    float_to_str(num, sizeof(num), mpu6050_vec3_st.x_db, 2);
    snprintf(buf, sizeof(buf), "ax:%s", num);   /* ✅ num 和 buf 分开 */
    cfb_print(s_oled_pst, buf, 0, 10);

    /* ay */
    float_to_str(num, sizeof(num), mpu6050_vec3_st.y_db, 2);
    snprintf(buf, sizeof(buf), "ay:%s", num);   /* ✅ num 和 buf 分开 */
    cfb_print(s_oled_pst, buf, 64, 10);

    /* az */
    float_to_str(num, sizeof(num), mpu6050_vec3_st.z_db, 2);
    snprintf(buf, sizeof(buf), "az:%s", num);   /* ✅ num 和 buf 分开 */
    cfb_print(s_oled_pst, buf, 0, 20);

    mpu6050_get_gyro(&mpu6050_vec3_st);
    
    float_to_str(num, sizeof(num), mpu6050_vec3_st.x_db, 2);
    snprintf(buf, sizeof(buf), "gx:%s", num);
    cfb_print(s_oled_pst, buf, 64, 20);

    /* gy */
    float_to_str(num, sizeof(num), mpu6050_vec3_st.y_db, 2);
    snprintf(buf, sizeof(buf), "gy:%s", num);
    cfb_print(s_oled_pst, buf, 0, 30);

    /* gz */
    float_to_str(num, sizeof(num), mpu6050_vec3_st.z_db, 2);
    snprintf(buf, sizeof(buf), "gz:%s", num);
    cfb_print(s_oled_pst, buf, 64, 30);

}


static void s_draw_euler(struct menu_node_t* self)
{
    char num[16];   /* 放数值 */
    char buf[32];   /* 放最终结果 */
    cfb_framebuffer_set_font(s_oled_pst, 1);
    cfb_print(s_oled_pst, self->base.name, 0, 0);
    cfb_framebuffer_set_font(s_oled_pst, 0);
                   
    // struct euler_t euler_st = {0};
    // euler_copy(&euler_st);

    float_to_str(num, sizeof(num), rtY.pitch , 2);
    snprintf(buf, sizeof(buf), "pitch:%s", num);   /* ✅ num 和 buf 分开 */
    cfb_print(s_oled_pst, buf, 0, 20);

    float_to_str(num, sizeof(num), rtY.roll , 2);
    snprintf(buf, sizeof(buf), "roll:%s", num);   /* ✅ num 和 buf 分开 */
    cfb_print(s_oled_pst, buf, 64, 20);

    // float_to_str(num, sizeof(num), rtY.Heading * RAD_TO_DEG, 2);
    // snprintf(buf, sizeof(buf), "yaw:%s", num);   /* ✅ num 和 buf 分开 */
    // cfb_print(s_oled_pst, buf, 0, 30);

}
/* ================================================================
 * 可编辑数据
 * ================================================================ */

static int32_t s_speed_a_l = 0;

/* ================================================================
 * 节点实例
 * ================================================================ */

struct menu_node_t g_root;
static struct menu_node_t s_simulink_st;
static struct menu_node_t s_speed_a_st;

static struct menu_node_t g_mpu6050_raw_st;
struct menu_base_t* g_mpu6050_raw_oled_pst;

static struct menu_node_t g_mpu6050_euler_st;
struct menu_base_t* g_mpu6050_euler_oled_pst;

static void s_draw_encoder(struct menu_node_t* self)
{
    char num[10];   /* 放数值 */
    char buf[32];   /* 放最终结果 */
    cfb_framebuffer_set_font(s_oled_pst, 1);
    cfb_print(s_oled_pst, self->base.name, 0, 0);
    cfb_framebuffer_set_font(s_oled_pst, 0);

    struct encoder_data_t encoder_data_a_st = {0};
    struct encoder_data_t encoder_data_b_st = {0};
    encoder_get_data(g_encoder_a_pst,&encoder_data_a_st);
    encoder_get_data(g_encoder_b_pst,&encoder_data_b_st);
    
    

    float_to_str(num, sizeof(num), encoder_data_a_st.rpm_f, 2);
    snprintf(buf, sizeof(buf), "v_a:%s",num);   /* ✅ num 和 buf 分开 */
    cfb_print(s_oled_pst, buf, 0, 20);

    snprintf(buf, sizeof(buf), "p_a:%d", encoder_data_a_st.position_l);   /* ✅ num 和 buf 分开 */
    cfb_print(s_oled_pst, buf, 64, 20);

    float_to_str(num, sizeof(num), encoder_data_b_st.rpm_f, 2);
    snprintf(buf, sizeof(buf), "v_b:%s",num );   /* ✅ num 和 buf 分开 */
    cfb_print(s_oled_pst, buf, 0, 30);

    snprintf(buf, sizeof(buf), "p_b:%d", encoder_data_b_st.position_l);   /* ✅ num 和 buf 分开 */
    cfb_print(s_oled_pst, buf, 64, 30);
    
}
static struct menu_node_t g_encode_st;
struct menu_base_t* g_encode_oled_pst;
/* ================================================================
 * 构建菜单树
 * ================================================================ */

static int s_build_menu_tree(void)
{
    Create_Menu_Folder(NULL, g_root, "Main");
    Create_Menu_Folder(&g_root, s_simulink_st, "simulink");
    g_mpu6050_raw_oled_pst = Create_Menu_Leaf(&g_root,g_mpu6050_raw_st, "mpu6050", s_draw_mpu6050);
    g_mpu6050_euler_oled_pst =  Create_Menu_Leaf(&g_root,g_mpu6050_euler_st, "euler", s_draw_euler);
    g_encode_oled_pst =  Create_Menu_Leaf(&s_simulink_st,g_encode_st, "encoder", s_draw_encoder);
    
    Create_Menu_Leaf_Range(&s_simulink_st, s_speed_a_st,
                           "speed_a", s_draw_speed_a,
                           &s_speed_a_l, -50, 50);
    menu_set_default_int(&s_speed_a_st, 0);

    return 0;
}

/* ================================================================
 * 初始化 (menu_init_v 已由 menu.c 处理动作注册)
 * ================================================================ */

// void menu_oled_init_v(void)
// {
//     s_build_menu_tree();
// }
SYS_INIT(s_build_menu_tree, APPLICATION, 10);

