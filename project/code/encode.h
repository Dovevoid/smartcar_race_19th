/*
 * encode.h
 *
 *  Created on: 2024��7��31��
 *      Author: Dove
 */

#ifndef ENCODE_H_
#define ENCODE_H_

//���嶨ʱ��9���������AB��
#define ENCODER                     (TIM9_ENCOEDER)
#define ENCODER_A                 (TIM9_ENCOEDER_MAP3_CH1_D9)
#define ENCODER_B                 (TIM9_ENCOEDER_MAP3_CH2_D11)

#define SPEED_F 200.0f          //�ٶȻ�ȡƵ��,ע����ж�ʱ��ƥ��
#define meter_count 11500.0f    //������һ�׵ļ���
extern float speed_avr;         //����õ����ٶ�
extern float distance;          //��ʻ�ľ���

void encode_Init(void);
void GetSpeed(void);

#endif /* ENCODE_H_ */
