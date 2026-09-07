#include "euler.h"
#include "mpu6050.h"
#include <math.h>
#include <stdint.h>

#define RAD_TO_DEG 57.295779513082320876798154814105


#if  ACC_ANGLE
float angle_acc_f;
#endif



void euler_update(void)
{
    mpu6050_sample();
    
    struct mpu6050_vec3_t acc_st = {0};
    struct mpu6050_vec3_t gyro_st = {0};

    mpu6050_get_accel(&acc_st);
    mpu6050_get_gyro(&gyro_st);


    // rtU.gyrox= gyro_st.x_db;
    // rtU.gyroy = gyro_st.y_db;
    // rtU.accx = acc_st.x_db;
    // rtU.accy = acc_st.y_db;
    // rtU.accz = acc_st.z_db;
    
#if ACC_ANGLE
    angle_acc_f = -atan2(acc_st.x_db, acc_st.z_db) * RAD_TO_DEG;   //往前倾的时候x,z 都是正的
#endif
    // angle_gryo_f = pitch_pre_f + gyro_st.y_db * 0.001 * RAD_TO_DEG; //gyro.y_db 却是负的，因此这样

    // float Alpha = 0.05;
    // pitch_pre_f = Alpha * angle_acc_f + (1 - Alpha) * angle_gryo_f

}

