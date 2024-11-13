/*
 * encode.c
 *
 *  Created on: 2024年7月31日
 *      Author: Dove
 */

#include  "zf_common_headfile.h"
# include "headfile.h"

int16 encode_count = 0;
float speed_avr = 0.0;
float distance = 0.0;


/***编码器初始化***/
void encode_Init(void)
{
    encoder_quad_init(ENCODER, ENCODER_A, ENCODER_B);                     // 初始化编码器模块与引脚 正交解码编码器模式
}


/***编码器获取数据***/
//注意：放在中断中执行，暂定5ms
void GetSpeed(void)
{
    static int16 count_filter[4];

    encode_count = encoder_get_count(ENCODER);      // 获取编码器计数
    encoder_clear_count(ENCODER);                   // 清空编码器计数

    //滤波
    for (uint8 i = 3; i > 0; i--)
    {
        count_filter[i] = count_filter[i - 1];
    }
    count_filter[0]= encode_count;
    float count_Afterfilter;
    count_Afterfilter = 0.4*count_filter[0]+0.3*count_filter[1]+0.2*count_filter[2]+0.1*count_filter[3];

    //应用速度一阶滤波  车速(M/S)=（计数值*计数频率/一米计数值）
    speed_avr = speed_avr*0.15 + 0.85*count_Afterfilter* SPEED_F / meter_count;

    //限速
    if (speed_avr > 2)
    {
        speed_avr = 2;
    }

    //计数值累加得距离（单位cm），根据实际调整系数meter_count
    distance += count_Afterfilter/meter_count*100;

}

