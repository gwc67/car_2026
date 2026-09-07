#ifndef __ENCODE_H
#define __ENCODE_H

#include <stdint.h>

struct encoder_base_t
{
    const char* name;
};

struct encoder_data_t
{
    int32_t position_l;  //当前位置，累计量
    float rpm_f;       //转速     
};

int encoder_read(struct encoder_base_t* me);
void encoder_update_all(void);
int encoder_get_data(struct encoder_base_t* me, struct encoder_data_t* out);

extern struct encoder_base_t* g_encoder_a_pst;
extern struct encoder_base_t* g_encoder_b_pst;

#endif
