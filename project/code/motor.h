/*
 * motor.h
 *
 *  Created on: 2024年7月31日
 *      Author: Dove
 */

#ifndef MOTOR_H_
#define MOTOR_H_

//无刷电机
//右
#define BrushlessMotorRight_PWM (TIM4_PWM_MAP1_CH2_D13)
//左
#define BrushlessMotorLeft_PWM (TIM4_PWM_MAP1_CH1_D12)
//下
#define BrushlessMotorDown_PWM (TIM1_PWM_MAP3_CH1_E9)
//后
#define BrushlessMotorBehind_PWM (TIM1_PWM_MAP3_CH2_E11)

//有刷电机
//左
#define Right_Dir (A0)
#define Right_PWM (TIM5_PWM_MAP0_CH2_A1)
//右
#define Left_Dir (A3)
#define Left_PWM (TIM5_PWM_MAP0_CH3_A2)

//模式设置
typedef enum
{
    MOTORTEST = 1,
    STOP = 2,
    CLOSELOOP = 3,
    OPENLOOP = 4,
}CarMode;

typedef struct
{
    CarMode SMode;  //不加S会报一个很奇怪的错误
}CarInfotypedef;

extern CarInfotypedef CarInfo;
//起飞标志位
extern uint8_t allout_flag;
//控制气垫无刷
extern float BrushlessMotorDown_PWM_out;

void Motor_Init(void);
int16 PWM_Limit(float PWM,int16 max);
void ALL_Out(void);


#endif /* MOTOR_H_ */
