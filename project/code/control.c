/*
 * control.c
 *
 *  Created on: 2024��7��31��
 *      Author: Dove
 */
#include "zf_common_headfile.h"
#include "headfile.h"


/****Speed***/
float Speed_Control = 0;
StandInfotypedef SpeedLoop;
Speed_struct speed;
/***�ڻ�***/
DirInnerInfotypedef DirInner;
float Diraction_Out = 0;
/***�⻷***/
DirOutterInfotypedef DirOutter;
/***������***/
DirOutterInfotypedef Force_loop;
float error_temp;

void PID_init(void)
{
    //�ٶ�
    speed.Init =1.2;//1.3
    speed.Stan = speed.Init;
    //�ٶȻ�
    SpeedLoop.BaseKP = 8;
    SpeedLoop.BaseKI = 0.4;
    SpeedLoop.BaseKD = 0;

    //ת��
    DirOutter.BaseKP = 3.2;//4.2 3.2
    DirOutter.BaseKD =1.2;//1.2

    DirInner.BaseKP = 3.2;
    DirInner.BaseKD = 0.8;

    //������
    Force_loop.BaseKP = 6;//8
    Force_loop.BaseKD = 4.5;//4.5

}

/*****************�ٶȻ�*************************/
void Speed_Loop(void)
{
    static float ki_max = 2.0;  //�������޷�ֵ

    SpeedLoop.ErrorMaxLimit = 1.2;
    SpeedLoop.ErrorMinLimit = -1.2;

    SpeedLoop.OutPutMaxLimit = 3500;
    SpeedLoop.OutPutMinLimit = -3500;

    SpeedLoop.KP = SpeedLoop.BaseKP;
    SpeedLoop.KI = SpeedLoop.BaseKI;
    SpeedLoop.KD = SpeedLoop.BaseKD;

    SpeedLoop.Error = (speed.Stan - speed_avr) ;//�ٶ�ƫ�����
    SpeedLoop.Error = FloatRangeProtect(SpeedLoop.Error,SpeedLoop.ErrorMaxLimit,SpeedLoop.ErrorMinLimit);//�ٶ�ƫ���޷�

    SpeedLoop.ErrorFifo[2] = SpeedLoop.ErrorFifo[1];
    SpeedLoop.ErrorFifo[1] = SpeedLoop.ErrorFifo[0];
    SpeedLoop.ErrorFifo[0] = SpeedLoop.Error;//P��

    SpeedLoop.ErrorDtFifo[2] = SpeedLoop.ErrorDtFifo[1];
    SpeedLoop.ErrorDtFifo[1] = SpeedLoop.ErrorDtFifo[0];
    SpeedLoop.ErrorDtFifo[0] = SpeedLoop.ErrorFifo[0] - SpeedLoop.ErrorFifo[1];//΢����

    if(fabs(SpeedLoop.Error)<0.25)
    {
        SpeedLoop.Integ += SpeedLoop.Error;//������
    }
    SpeedLoop.Integ = FloatRangeProtect(SpeedLoop.Integ,ki_max,-ki_max);//�Ի����������޷�

    if(youhuandao_flag||zuohuandao_flag)
    {
        SpeedLoop.OutPut = SpeedLoop.KP * SpeedLoop.Error + SpeedLoop.KI * SpeedLoop.Integ + SpeedLoop.KD * (SpeedLoop.ErrorDtFifo[0] * 0.6 + SpeedLoop.ErrorDtFifo[1] * 0.3+SpeedLoop.ErrorDtFifo[2] * 0.1) ;
    }
    else
    {
        SpeedLoop.OutPut = (SpeedLoop.KP - 4*error_temp/20)* SpeedLoop.Error + SpeedLoop.KI * SpeedLoop.Integ + SpeedLoop.KD * (SpeedLoop.ErrorDtFifo[0] * 0.6 + SpeedLoop.ErrorDtFifo[1] * 0.3+SpeedLoop.ErrorDtFifo[2] * 0.1) ;
    }
    SpeedLoop.OutPut =  SpeedLoop.OutPut * Speedloop_Large;//�Ŵ��������

    SpeedLoop.OutPut = FloatRangeProtect(SpeedLoop.OutPut,SpeedLoop.OutPutMaxLimit,SpeedLoop.OutPutMinLimit);//������������޷�

    Speed_Control = ( 0.85 * SpeedLoop.OutPut + 0.15 * Speed_Control);

}

/********************ͣ����**************************/
void stop(void)
{
    speed.Stan = 0;
    CarInfo.SMode = STOP;
    BrushlessMotorDown_PWM_out = 0;
}

/*****************�޷�����*************************/
float FloatRangeProtect(float data, float max_out, float min_out)
{
  data=(data>=max_out)?max_out:data;
  data=(data<=min_out)?min_out:data;

  return data;
}

/*****************���ٶȻ�************************/
void Direction_Inner(void)
{
    uint8 i;
    //�洢�ϴ�ƫ��
    DirInner.ErrorLast = DirInner.Error;
    DirInner.Error = DirOutter.InnerInput - CarAngle.Yawrate;//0- CarAngle.Yawrate;


    for(i=9;i>0;i--)
        DirInner.ErrorFifo[i] = DirInner.ErrorFifo[i-1];
    DirInner.ErrorFifo[0] = DirInner.Error;

    for(i=3;i>0;i--)
        DirInner.ErrorDtTemp[i] = DirInner.ErrorDtTemp[i-1];
    DirInner.ErrorDtTemp[0] = DirInner.ErrorFifo[0]-DirInner.ErrorFifo[3];

    //PID
    {
        DirInner.InOut = DirInner.BaseKP * (0.85*DirInner.ErrorFifo[0] +0.15*DirInner.ErrorFifo[0])+
                         DirInner.BaseKD * (DirInner.ErrorDtTemp[0] * 0.8 + DirInner.ErrorDtTemp[1] * 0.15 + DirInner.ErrorDtTemp[2] * 0.05);
    }

    for(i=3;i>0;i--)
        DirInner.OutTemp[i] = DirInner.OutTemp[i-1];
    DirInner.OutTemp[0] = DirInner.InOut*DirInnerloop_Large;;

    DirInner.OutPut = DirInner.OutTemp[0]*0.8 + DirInner.OutTemp[1]*0.2;

    //�������
    DirInner.OutPut = FloatRangeProtect(DirInner.OutPut, 6000, -6000);
    Diraction_Out = DirInner.OutPut;

}


/***�����⻷***/
void Direction_Outter(void)
{
    uint8 i;
    for(i=9;i>0;i--)
        DirOutter.ErrorFifo[i] = DirOutter.ErrorFifo[i-1];

    DirOutter.ErrorFifo[0] =Findline.err[0];

    DirOutter.ErrorFifo[0] = DirOutter.ErrorFifo[0] * 0.4 + DirOutter.ErrorFifo[1] * 0.3+ DirOutter.ErrorFifo[2] * 0.2 + DirOutter.ErrorFifo[3] * 0.1;

    for(i=3;i>0;i--)
        DirOutter.ErrorDtTemp[i] = DirOutter.ErrorDtTemp[i-1];
    DirOutter.ErrorDtTemp[0] = DirOutter.ErrorFifo[0] - DirOutter.ErrorFifo[3];

    //PID
    DirOutter.OutPut = DirOutter.BaseKP *DirOutter.ErrorFifo[0]  +
                       DirOutter.BaseKD * (DirOutter.ErrorDtTemp[0] * 0.6 + DirOutter.ErrorDtTemp[1] * 0.3 + DirOutter.ErrorDtTemp[2] * 0.1);

    //��������
    DirOutter.Expect_Curvature = DirOutter.OutPut/100*PI/180;

    DirOutter.Curvature_Correct = DirOutter.Expect_Curvature;

    /*���������������*/
    DirOutter.Curvature_Final = DirOutter.Curvature_Correct*180/PI*100;

    //������ʽ�еĸ�����Ҫȷ�����⻷�ķ���һ��
    DirOutter.InnerInput = -DirOutter.Curvature_Final;

}


/***��ֹ��Ư���ṩ������***/
void CentripetalForce(void)
{
    uint8 i;
    for(i=9;i>0;i--)
        Force_loop.ErrorFifo[i] = Force_loop.ErrorFifo[i-1];
    Force_loop.ErrorFifo[0] =Findline.err[0];
//    ƫ���޷�
    Force_loop.ErrorFifo[0]  = FloatRangeProtect(Force_loop.ErrorFifo[0] ,35,-35);

    Force_loop.ErrorFifo[0] = Force_loop.ErrorFifo[0] * 0.4 + Force_loop.ErrorFifo[1] * 0.3+ Force_loop.ErrorFifo[2] * 0.2 + Force_loop.ErrorFifo[3] * 0.1;

    for(i=3;i>0;i--)
        Force_loop.ErrorDtTemp[i] = Force_loop.ErrorDtTemp[i-1];
    Force_loop.ErrorDtTemp[0] = Force_loop.ErrorFifo[0] - Force_loop.ErrorFifo[3];

    //��̬�ı�Pֵ
    error_temp = abs(Force_loop.ErrorFifo[0]);
    if (error_temp >= 20) {
        error_temp = 20;
    }

    //������ʽ
    if(youhuandao_flag||zuohuandao_flag)
    {
        Force_loop.OutPut = (Force_loop.BaseKP+0.8) *Force_loop.ErrorFifo[0]  +
                        Force_loop.BaseKD * (Force_loop.ErrorDtTemp[0] * 0.6 + Force_loop.ErrorDtTemp[1] * 0.3 + Force_loop.ErrorDtTemp[2] * 0.1);
    }
    else
    {
        Force_loop.OutPut = (Force_loop.BaseKP + 2*error_temp/20)*Force_loop.ErrorFifo[0]  +
                            Force_loop.BaseKD * (Force_loop.ErrorDtTemp[0] * 0.6 + Force_loop.ErrorDtTemp[1] * 0.3 + Force_loop.ErrorDtTemp[2] * 0.1);
    }
    //������������޷�
        Force_loop.OutPut = FloatRangeProtect(Force_loop.OutPut,250,-250);

}




