#include "mpu6050.h"
#include "OLED_Menu.h"
#include "zephyr/device.h"
#include "zephyr/drivers/sensor.h"
#include "zephyr/init.h"
#include "zephyr/kernel.h"
#include "zephyr/logging/log.h"
#include "zephyr/syscalls/kernel.h"
#include "zephyr/syscalls/sensor.h"
#include <math.h>
#include <sys/errno.h>


LOG_MODULE_REGISTER(mpu6050,LOG_LEVEL_INF);


#define SAMPLE_TIME 200

static const struct device* mpu6050_dev_pst;

static struct mpu6050_vec3_t s_gyro_bias;




static int s_mpu6050_init(void)
{
    mpu6050_dev_pst = DEVICE_DT_GET(DT_NODELABEL(mpu6050));
    if (!device_is_ready(mpu6050_dev_pst)) {
        return -ENODEV;
    }
    return 0;
}
SYS_INIT(s_mpu6050_init,APPLICATION, 10);

int mpu6050_sample(void)
{
    menu_request_refresh(g_mpu6050_raw_oled_pst);
    return sensor_sample_fetch(mpu6050_dev_pst);
}

int mpu6050_get_accel(struct mpu6050_vec3_t* out)
{
    struct sensor_value buf[3];
    int ret = sensor_channel_get(mpu6050_dev_pst, SENSOR_CHAN_ACCEL_XYZ, buf);
    if (ret == 0 && out) {
        out->x_f = sensor_value_to_float(&buf[0]);
        out->y_f = sensor_value_to_float(&buf[1]);
        out->z_f = sensor_value_to_float(&buf[2]);
    }
    return ret;
}

int mpu6050_get_gyro(struct mpu6050_vec3_t *out)
{
    struct sensor_value buf[3];
    int ret = sensor_channel_get(mpu6050_dev_pst, SENSOR_CHAN_GYRO_XYZ, buf);
    if (ret == 0 && out) {
        out->x_f = sensor_value_to_float(&buf[0]);
        out->y_f = sensor_value_to_float(&buf[1]);
        out->z_f = sensor_value_to_float(&buf[2]);;

        out->x_f += 0.05f;
        out->y_f += 0.01f;
        out->z_f += 0.07f;  //零漂
    }
    return ret;
}

int mpu6050_get_temp(float *out)
{
    struct sensor_value temp_st = {0};
    int ret = sensor_channel_get(mpu6050_dev_pst, SENSOR_CHAN_DIE_TEMP, &temp_st);
    *out = sensor_value_to_float(&temp_st);
    return ret;
}

int mpu6050_callibrate(void)
{
    struct mpu6050_vec3_t temp;

    
    LOG_INF("mpu6050_begin_callibrate");

    for (int i = 0; i < SAMPLE_TIME; i++) {
        mpu6050_get_gyro(&temp);

        s_gyro_bias.x_f += temp.x_f;
        s_gyro_bias.y_f += temp.y_f;
        s_gyro_bias.z_f += temp.z_f;
        k_sleep(K_MSEC(10));
    }

    s_gyro_bias.x_f /= SAMPLE_TIME;
    s_gyro_bias.y_f /= SAMPLE_TIME;
    s_gyro_bias.z_f /= SAMPLE_TIME;

    LOG_INF("mpu6050_callibrate_finish");

    
}
