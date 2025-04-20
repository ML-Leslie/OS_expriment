#include "os_type.h"
#include <string.h>
template<typename T>
void swap(T &x, T &y) {
    T z = x;
    x = y;
    y = z;
}


void itos(char *numStr, uint32 num, uint32 mod) {
    // 只能转换2~26进制的整数
    if (mod < 2 || mod > 26 || num < 0) {
        return;
    }

    uint32 length, temp;

    // 进制转换
    length = 0;
    while(num) {
        temp = num % mod;
        num /= mod;
        numStr[length] = temp > 9 ? temp - 10 + 'A' : temp + '0';
        ++length;
    }

    // 特别处理num=0的情况
    if(!length) {
        numStr[0] = '0';
        ++length;
    }

    // 将字符串倒转，使得numStr[0]保存的是num的高位数字
    for(int i = 0, j = length - 1; i < j; ++i, --j) {
        swap(numStr[i], numStr[j]);
    }
    
    numStr[length] = '\0';
}


// 将浮点数转换为字符串
void ftos(char *buffer, double value, int precision) {
    int int_part = (int)value; // 整数部分
    double frac_part = value - int_part; // 小数部分

    // 转换整数部分
    itos(buffer, int_part, 10);

    // 添加小数点
    int len = 0;
    while (buffer[len] != '\0') len++;
    buffer[len++] = '.';

    // 转换小数部分
    for (int i = 0; i < precision; ++i) {
        frac_part *= 10;
        int digit = (int)frac_part;
        buffer[len++] = '0' + digit;
        frac_part -= digit;
    }

    buffer[len] = '\0';
}

// 科学记数法
void etos(char *str, double value, int precision)
{
    // 处理0的特殊情况
    if (value == 0.0)
    {
        char zeroStr[] = "0.000000e+00";
        int i = 0;
        while (zeroStr[i])
        {
            str[i] = zeroStr[i];
            i++;
        }
        str[i] = '\0';
        return;
    }

    // 处理符号
    bool negative = false;
    if (value < 0)
    {
        negative = true;
        value = -value;
    }

    // 计算幂次
    int exponent = 0;
    while (value >= 10.0)
    {
        value /= 10.0;
        exponent++;
    }
    while (value < 1.0 && value != 0.0)
    {
        value *= 10.0;
        exponent--;
    }

    // 格式化主要部分
    int idx = 0;
    if (negative)
        str[idx++] = '-';
    
    // 添加整数部分
    int intPart = (int)value;
    str[idx++] = '0' + intPart;
    str[idx++] = '.';
    
    // 小数部分
    value -= intPart;
    for (int i = 0; i < precision; i++)
    {
        value *= 10;
        int digit = (int)value;
        str[idx++] = '0' + digit;
        value -= digit;
    }

    // 添加e符号和指数
    str[idx++] = 'e';
    if (exponent >= 0)
        str[idx++] = '+';
    else
    {
        str[idx++] = '-';
        exponent = -exponent;
    }

    // 添加两位指数
    if (exponent < 10)
        str[idx++] = '0';
    
    // 转换指数为字符串
    char expStr[3];
    itos(expStr, exponent, 10);
    
    // 复制指数部分
    int j = 0;
    while (expStr[j])
        str[idx++] = expStr[j++];
        
    str[idx] = '\0';
}