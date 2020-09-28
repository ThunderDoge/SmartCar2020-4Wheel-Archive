/**
 * @file      filter.c
 * @brief     速度编码器用的滤波器函数
 * @details   
 * @author   ThunderDoge
 * @date      2020-9-26
 * @version   0.0
 * @par Copyright (c):  OnePointFive, the UESTC RoboMaster Team. 2019~2020 
 * Using encoding: utf-8
 */
#include "filter.h"

/**
 * @brief 滑动窗口滤波
 * 
 * @param input 待滤波数据数组，从input[0]开始取
 * @param window_wide 窗口宽度，
 * @return float 返回
 */
float filter_slide_window_mean(int16_t *input, uint16_t window_wide)
{
    int32_t sum;
    int16_t i;

    // 限制长度最大值，防止越界得太离谱
    if(window_wide > 20)
        window_wide = 20;

    sum=0;

    for(i=0;i<window_wide;i++)
    {
        sum += input[i];
    }

    return (float)(sum/((float)(window_wide)));
}

/**
 * @brief 滑动窗口指数滤波
 * 
 */
float filter_slide_window_exp_weight(int16_t *input, uint16_t window_wide, float exp_argument)
{
    int32_t sum,sum_of_weight;
    int16_t i;
    float weight;

    // 限制长度最大值，防止越界得太离谱
    if(window_wide > 20)
        window_wide = 20;

    sum=0,sum_of_weight=0;

    for(i=0;i<window_wide;i++)
    {
        weight = (float) pow(exp_argument,i);
        sum += input[i] / weight;   
        sum_of_weight += weight;    //等会归一化用
    }

    return (float)(sum/((float)(window_wide*sum_of_weight)));
}


