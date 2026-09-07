#ifndef __MPU6050_H
#define __MPU6050_H



struct mpu6050_vec3_t{
    double x_db;
    double y_db;
    double z_db;
} ;

int mpu6050_sample(void);
int mpu6050_get_accel(struct mpu6050_vec3_t* out);
int mpu6050_get_gyro(struct mpu6050_vec3_t *out);
int mpu6050_get_temp(double *out);




#endif


