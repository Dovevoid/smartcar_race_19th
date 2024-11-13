/*
 * motor.c
 *
 *  Created on: 2024��7��31��
 *      Author: Dove
 */

#include  "zf_common_headfile.h"
#include  "headfile.h"

CarInfotypedef CarInfo;
uint8_t allout_flag = 0;
float BrushlessMotorDown_PWM_out = 0;


/***�����ʼ��***/
void Motor_Init(void)
{
    //��ʼ����������
    gpio_init(Right_Dir, GPO, 0, GPIO_PIN_CONFIG) ;
    gpio_init(Left_Dir, GPO, 0, GPIO_PIN_CONFIG) ;

    //��ʼ��PWM
    //ʹ��ʾ�� pwm_init(PWM1_CH1_A8, 50, 5000);
    //��ʼ��PWM1 ͨ��1 ʹ������A8  ���PWMƵ��50HZ   ռ�ձ�Ϊ�ٷ�֮ 5000/PWM_DUTY_MAX*100
    //PWM_DUTY_MAX��zf_pwm.h�ļ��� Ĭ��Ϊ10000
    pwm_init(Right_PWM, 20000, 0);
    pwm_init(Left_PWM, 20000, 0);

    BrushlessMotorDown_PWM_out = 775;//775
    pwm_init(BrushlessMotorRight_PWM, 50, 0);
    pwm_init(BrushlessMotorLeft_PWM, 50, 0);
    pwm_init(BrushlessMotorDown_PWM, 50, 0);
//    pwm_init(BrushlessMotorBehind_PWM, 50, 0);

}

/***��ˢ����޷�***/
int16 Brush_PWM_Limit(float PWM,int16 max)
{
    if(PWM > -max && PWM < max)
        return (int16)PWM;
    else if( PWM >=  max)
        return max;
    else
        return -max;
}

/***��ˢ����޷�***/
//ֵ��500 �� 1000 ֮��
int16 Brushless_PWM_Limit(float PWM)
{
    if(PWM > 500 && PWM < 1000)
        return (int16)PWM;
    else if( PWM >=  1000)
        return 1000;
    else
        return 500;
}


/***����������***/
void ALL_Out(void)
{
    static float BrushlessRight_out,BrushlessLeft_out,BrushlessBehind_out,BrushRight_out,BrushLeft_out;

    if(CarInfo.SMode <= MOTORTEST)//������ԣ����Ե������ת
    {
        BrushlessRight_out = 0;
        BrushlessLeft_out  = 0;
        BrushlessBehind_out = 0;
        BrushRight_out = 1000;
        BrushLeft_out = 1000;
    }

    else if(CarInfo.SMode <= STOP)//ƽ��ģʽ
    {
        BrushlessRight_out = 0;
        BrushlessLeft_out  = 0;
        BrushlessBehind_out = 0;
        BrushRight_out = 0;
        BrushLeft_out = 0;
    }

    else if(CarInfo.SMode <= CLOSELOOP)//�ջ�ģʽ
    {
        //β����ˢ���
//        BrushlessBehind_out =0;// Speed_Control;
        //β����ˢ���
        if (straight_flag) {
            BrushlessBehind_out =725;// Speed_Control;
        } else {
            BrushlessBehind_out =0;// Speed_Control;
        }

        //��ˢ������  ��������ҵ����һ����ֵ��Ҫ����
        {
            BrushRight_out = Speed_Control+DirInner.OutPut;//+DirInner.OutPut;//
            BrushLeft_out = Speed_Control-DirInner.OutPut;//-DirInner.OutPut;//425+
        }

        //����Ư��ˢ������
        BrushlessRight_out = 600 -Force_loop.OutPut;
        BrushlessLeft_out  = 600 + Force_loop.OutPut;

//        BrushlessRight_out = 0;
//        BrushlessLeft_out  = 0;

    }

    else if(CarInfo.SMode <= OPENLOOP)//����ģʽ
    {
        BrushlessRight_out = 0;
        BrushlessLeft_out  = 0;
        BrushlessBehind_out = 0;
        BrushRight_out = 0;
        BrushLeft_out = 0;
    }

    //�����޷�����
    BrushlessRight_out =  Brushless_PWM_Limit(BrushlessRight_out);
    BrushlessLeft_out =   Brushless_PWM_Limit(BrushlessLeft_out);
    BrushlessBehind_out = Brushless_PWM_Limit(BrushlessBehind_out);
    BrushRight_out = Brush_PWM_Limit((float)BrushRight_out,5000);
    BrushLeft_out  = Brush_PWM_Limit((float)BrushLeft_out,5000);

    //��ˢ���
    if(BrushRight_out<0)
    {
        gpio_set_level(Right_Dir,1);
        pwm_set_duty(Right_PWM,(uint32)-BrushRight_out);

    }
    else
    {
        gpio_set_level(Right_Dir,0);
        pwm_set_duty(Right_PWM,(uint32)BrushRight_out);
    }


    if(BrushLeft_out < 0)
    {
        gpio_set_level(Left_Dir,1);
        pwm_set_duty(Left_PWM,(uint32)-BrushLeft_out);

    }
    else
    {
        gpio_set_level(Left_Dir,0);
        pwm_set_duty(Left_PWM,(uint32)BrushLeft_out);
    }


    //��ˢ���
    pwm_set_duty(BrushlessMotorRight_PWM,(uint32)BrushlessRight_out);
    pwm_set_duty(BrushlessMotorLeft_PWM,(uint32)BrushlessLeft_out);
//    pwm_set_duty(BrushlessMotorBehind_PWM,(uint32)BrushlessBehind_out);


    //����ˢ���
    if (CarInfo.SMode == CLOSELOOP ||CarInfo.SMode == STOP ||CarInfo.SMode == MOTORTEST)
    {
        pwm_set_duty(BrushlessMotorDown_PWM,(uint32)BrushlessMotorDown_PWM_out);
    }

}

