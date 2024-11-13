/*
 * gyro.c
 *
 *  Created on: 2024��8��1��
 *      Author: Dove
 */

#include "zf_common_headfile.h"
#include "headfile.h"

CarAgnleInfotypedef CarAngle;
GYRO_CLASS gyro={0};

float GRYZ;
double yrateMax,yrateMin,yrateaverage;

/***��ȡ��̬***/
void Get_Attitude(void)
{
    //��ȡ����������
    imu660ra_get_gyro();
    //һ�׵�ͨ�˲���0.002Ϊ����ʱ��
    LPF_1_db(35,0.002,imu660ra_gyro_transition(imu660ra_gyro_z),&GRYZ);

    //��ֵ�˲�
    CarAngle.Yawrate =  0.15*CarAngle.Yawrate+0.85*GRYZ;

    //���ٶȻ��ֵõ�ƫ���ǣ����´����Ǵ����˲�����Ʈ������
    if(CarAngle.Yawrate<yrateMax && CarAngle.Yawrate>yrateMin)
    {
        gyro.offset=0;
    }
    else
    {   //��Ʈ����ͳ�ƹ��ɣ���ʱ����С����ֹ������¶�ȡ���������ƽ��ֵ��ת���Ͽ������£�����0.6��������Ư��ƽ��ֵ��
        gyro.offset=yrateaverage;
    }

    gyro.TurnAngle_Integral -=   (CarAngle.Yawrate - gyro.offset )/ 1000;//500 500hz����Ƶ�ʣ�����Ϊ ��ÿ2ms�ٻ���

    if(gyro.TurnAngle_Integral>360) gyro.TurnAngle_Integral-=360;
    else if(gyro.TurnAngle_Integral<-360) gyro.TurnAngle_Integral+=360;

}

/***һ�׵�ͨ�˲�***/
void LPF_1_db(float hz, float time, float in, float *out)
{
    *out += ( 1 / ( 1 + 1 / ( hz * 6.28f * time ) ) ) * ( in - *out );
}

/***��ȡ����***/
void gyroscopeGetParameter(uint16 num)
{
  double  max=-1000,min=1000,sum=0;
  for(long int i = 0; i < num* 4; i++)
  {
      imu660ra_get_gyro();
      LPF_1_db(35,0.002,imu660ra_gyro_transition(imu660ra_gyro_z),&GRYZ);

      CarAngle.Yawrate =  0.15*CarAngle.Yawrate+0.85*GRYZ;

      if(max<CarAngle.Yawrate)
          max=CarAngle.Yawrate;
      if(min>CarAngle.Yawrate)
          min=CarAngle.Yawrate;
      sum+=CarAngle.Yawrate;
  }

  yrateMax = max;
  yrateMin = min;
  yrateaverage = sum/num/4;
}

/***��ʼ������***/
void gyroscopeInit(void)
{
    imu660ra_init();
    gyroscopeGetParameter(65535);//�����˲������ֵ����Сֵ���ֵ���� ����Ҫ����ֹ����

}

