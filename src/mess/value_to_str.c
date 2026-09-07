#include "value_to_str.h"
#include "stdio.h"
#include <stdint.h>

int float_to_str(char *buf, size_t buf_size, float value, uint8_t decimal_places)
{
    if (buf == NULL || buf_size < 2) {
        return -1;
    }

    /* 限制小数位数 */
    if (decimal_places > 6) {
        decimal_places = 6;
    }

    size_t pos = 0;

    /* 处理负数 */
    if (value < 0.0f) {
        buf[pos++] = '-';
        value = -value;
    }

    /* 计算缩放因子 */
    size_t scale = 1;
    for (uint8_t i = 0; i < decimal_places; i++) {
        scale *= 10;
    }

    /* 四舍五入 */
    float rounded = value * (float)scale + 0.5f;
    size_t total = (size_t)rounded;

    size_t integer = total / scale;
    size_t fraction = total % scale;

    /* 写入整数部分 */
    char temp[16];
    int i = 0;

    if (integer == 0) {
        temp[i++] = '0';
    } else {
        while (integer > 0) {
            temp[i++] = '0' + (integer % 10);
            integer /= 10;
        }
    }

    /* 检查缓冲区 */
    if (pos + i >= buf_size) return -1;

    /* 反转写入 */
    for (int j = i - 1; j >= 0; j--) {
        buf[pos++] = temp[j];
    }

    /* 写入小数部分 */
    if (decimal_places > 0) {
        if (pos >= buf_size - 1) return -1;
        buf[pos++] = '.';

        /* 从高位到低位写入 */
        for (int j = decimal_places - 1; j >= 0; j--) {
            size_t divisor = 1;
            for (int k = 0; k < j; k++) divisor *= 10;

            size_t digit = fraction / divisor;
            fraction %= divisor;

            if (pos >= buf_size - 1) return -1;
            buf[pos++] = '0' + digit;
        }
    }

    buf[pos] = '\0';
    return (int)pos;
}


int int32_to_str(char *buf, size_t buf_size, int32_t value)
{
    if (buf == NULL || buf_size < 2) {
        return -1;
    }

    size_t pos = 0;

    /* 处理 INT32_MIN（-2147483648），避免取绝对值溢出 */
    if (value == INT32_MIN) {
        const char *min_str = "-2147483648";
        for (int i = 0; min_str[i] != '\0'; i++) {
            if (pos >= buf_size - 1) return -1;
            buf[pos++] = min_str[i];
        }
        buf[pos] = '\0';
        return (int)pos;
    }

    /* 处理负数 */
    if (value < 0) {
        buf[pos++] = '-';
        value = -value;
    }

    /* 提取各位数字（逆序） */
    char temp[12];
    int i = 0;

    if (value == 0) {
        temp[i++] = '0';
    } else {
        while (value > 0) {
            temp[i++] = '0' + (value % 10);
            value /= 10;
        }
    }

    /* 检查缓冲区 */
    if (pos + i >= buf_size) return -1;

    /* 反转写入 */
    for (int j = i - 1; j >= 0; j--) {
        buf[pos++] = temp[j];
    }

    buf[pos] = '\0';
    return (int)pos;
}


/**
 * @brief 从字符串中解析浮点数
 * @param str 输入字符串，例如 "kp:1.25" 或 "ki:0.01, kd:0.5"
 * @param array_pf 输出浮点数数组
 * @param arr_size_uc 数组最大容量
 * @return 成功解析的浮点数个数
 */
uint8_t str_to_float(const char *str, float *array_pf, uint8_t arr_size_uc)
{
    if (str == NULL || array_pf == NULL || arr_size_uc == 0) {
        return 0;
    }

    uint8_t count = 0;

    while (*str != '\0' && count < arr_size_uc)
    {
        /* 跳过非数字、非正负号、非小数点的字符 */
        while (*str != '\0' && *str != '-' && *str != '+' && *str != '.' && (*str < '0' || *str > '9')) {
            str++;
        }
        if (*str == '\0') break;

        /* 处理符号 */
        int32_t sign = 1;
        if (*str == '-') { sign = -1; str++; }
        else if (*str == '+') { str++; }

        /* 检查是否为有效数字开头 */
        if (*str < '0' || *str > '9') {
            /* 可能是单独的 '.'，跳过继续查找 */
            if (*str == '.') {
                str++;
            }
            continue;
        }

        /* 解析整数部分 */
        int32_t int_part = 0;
        while (*str >= '0' && *str <= '9') {
            int_part = int_part * 10 + (*str - '0');
            str++;
        }

        /* 解析小数部分 */
        float frac_part = 0.0f;
        float frac_divisor = 1.0f;
        if (*str == '.') {
            str++;
            while (*str >= '0' && *str <= '9') {
                frac_part = frac_part * 10.0f + (float)(*str - '0');
                frac_divisor *= 10.0f;
                str++;
            }
        }

        /* 组合结果 */
        float val = (float)int_part + (frac_part / frac_divisor);
        val *= (float)sign;

        array_pf[count++] = val;
    }

    return count;
}

uint8_t str_to_int16(const char *str, int16_t *array_ps, uint8_t arr_size_uc)
{
    if (str == NULL || array_ps == NULL || arr_size_uc == 0) {
        return 0;
    }

    uint8_t count = 0;
    
    while (*str != '\0' && count < arr_size_uc) 
    {

        while (*str != '\0' && *str != '-' && *str != '+' && (*str < '0' || *str > '9')) {
            str++;
        }
        if (*str == '\0') break;


        int32_t sign = 1;
        if (*str == '-') { sign = -1; str++; }
        else if (*str == '+') { str++; }


        if (*str < '0' || *str > '9') continue;

        int32_t val = 0;
        while (*str >= '0' && *str <= '9') {
            val = val * 10 + (*str - '0');
            str++;
        }
        val *= sign;

        if (val > INT16_MAX) val = INT16_MAX;       // 32767
        else if (val < INT16_MIN) val = INT16_MIN;  // -32768

        array_ps[count++] = (int16_t)val;
    }
    return count;
}