#ifndef TB6612_H
#define TB6612_H

#include <stdint.h>

struct motor_base_t
{
    const char* name;
};

extern struct motor_base_t* g_motor_a_pst;
extern struct motor_base_t* g_motor_b_pst;
// /* 设置电机速度：-1000 ~ +1000，正负控制方向，0停止 */
int motor_set(struct motor_base_t* base, int16_t speed);
int motor_brake(struct motor_base_t* base);
extern volatile uint8_t  running_flag;

// /* 刹车 */
// int motor_brake(motor_id_t id);

#endif