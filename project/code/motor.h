/*
 * motor.h
 *
 *  Created on: 2024��7��31��
 *      Author: Dove
 */

#ifndef MOTOR_H_
#define MOTOR_H_

//��ˢ���
//��
#define BrushlessMotorRight_PWM (TIM4_PWM_MAP1_CH2_D13)
//��
#define BrushlessMotorLeft_PWM (TIM4_PWM_MAP1_CH1_D12)
//��
#define BrushlessMotorDown_PWM (TIM1_PWM_MAP3_CH1_E9)
//��
#define BrushlessMotorBehind_PWM (TIM1_PWM_MAP3_CH2_E11)

//��ˢ���
//��
#define Right_Dir (A0)
#define Right_PWM (TIM5_PWM_MAP0_CH2_A1)
//��
#define Left_Dir (A3)
#define Left_PWM (TIM5_PWM_MAP0_CH3_A2)

//ģʽ����
typedef enum
{
    MOTORTEST = 1,
    STOP = 2,
    CLOSELOOP = 3,
    OPENLOOP = 4,
}CarMode;

typedef struct
{
    CarMode SMode;  //����S�ᱨһ������ֵĴ���
}CarInfotypedef;

extern CarInfotypedef CarInfo;
//��ɱ�־λ
extern uint8_t allout_flag;
//����������ˢ
extern float BrushlessMotorDown_PWM_out;

void Motor_Init(void);
int16 PWM_Limit(float PWM,int16 max);
void ALL_Out(void);


#endif /* MOTOR_H_ */
