/*
 * gyro.c
 *
 *  Created on: 2024年8月1日
 *      Author: Dove
 */

#include "zf_common_headfile.h"
#include "headfile.h"

CarAgnleInfotypedef CarAngle;
GYRO_CLASS gyro={0};

float GRYZ;
double yrateMax,yrateMin,yrateaverage;

/***获取姿态***/
void Get_Attitude(void)
{
    //获取陀螺仪数据
    imu660ra_get_gyro();
    //一阶低通滤波，0.002为采样时间
    LPF_1_db(35,0.002,imu660ra_gyro_transition(imu660ra_gyro_z),&GRYZ);

    //均值滤波
    CarAngle.Yawrate =  0.15*CarAngle.Yawrate+0.85*GRYZ;

    //角速度积分得到偏航角，以下代码是窗口滤波对零飘做处理
    if(CarAngle.Yawrate<yrateMax && CarAngle.Yawrate>yrateMin)
    {
        gyro.offset=0;
    }
    else
    {   //零飘符合统计规律，长时间在小车静止的情况下读取数据以求得平均值，转动较快的情况下（大于0.6）将减掉漂移平均值。
        gyro.offset=yrateaverage;
    }

    gyro.TurnAngle_Integral -=   (CarAngle.Yawrate - gyro.offset )/ 1000;//500 500hz采样频率，换算为 度每2ms再积分

    if(gyro.TurnAngle_Integral>360) gyro.TurnAngle_Integral-=360;
    else if(gyro.TurnAngle_Integral<-360) gyro.TurnAngle_Integral+=360;

}

/***一阶低通滤波***/
void LPF_1_db(float hz, float time, float in, float *out)
{
    *out += ( 1 / ( 1 + 1 / ( hz * 6.28f * time ) ) ) * ( in - *out );
}

/***获取参数***/
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

/***初始化程序***/
void gyroscopeInit(void)
{
    imu660ra_init();
    gyroscopeGetParameter(65535);//窗口滤波的最大值，最小值与均值测量 ，需要车静止不动

}

