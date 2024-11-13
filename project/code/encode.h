/*
 * encode.h
 *
 *  Created on: 2024年7月31日
 *      Author: Dove
 */

#ifndef ENCODE_H_
#define ENCODE_H_

//定义定时器9输入编码器AB相
#define ENCODER                     (TIM9_ENCOEDER)
#define ENCODER_A                 (TIM9_ENCOEDER_MAP3_CH1_D9)
#define ENCODER_B                 (TIM9_ENCOEDER_MAP3_CH2_D11)

#define SPEED_F 200.0f          //速度获取频率,注意和中断时间匹配
#define meter_count 11500.0f    //编码器一米的计数
extern float speed_avr;         //计算得到的速度
extern float distance;          //行驶的距离

void encode_Init(void);
void GetSpeed(void);

#endif /* ENCODE_H_ */
