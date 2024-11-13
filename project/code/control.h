/*
 * control.h
 *
 *  Created on: 2024年7月31日
 *      Author: Dove
 */

#ifndef CONTROL_H_
#define CONTROL_H_

#define Speedloop_Large 2500;
#define DirInnerloop_Large 50;

typedef struct{
  float Error;                          //速度偏差
  float Stan;                           //标准速度
  float Init;                           //默认速度
}Speed_struct;

typedef struct
{
    float ErrorMaxLimit;
    float ErrorMinLimit;
    float KP;
    float KI;
    float KD;
    float BaseKP;
    float BaseKI;
    float BaseKD;
    float Error;
    float ErrorFifo[5];
    float ErrorDtFifo[5];
    float Integ;
    float OutPut;
    float OutPutMaxLimit;
    float OutPutMinLimit;
}StandInfotypedef;

extern Speed_struct speed;
extern StandInfotypedef SpeedLoop;
extern float Speed_Control;

void Speed_Loop(void);
float FloatRangeProtect(float data, float max_out, float min_out);
void PID_init(void);
void stop(void);


typedef struct
{
    float BaseKP;
    float BaseKI;
    float BaseKD;
    float Error;
    float ErrorLast;
    float ErrorFifo[10];
    float ErrorDtTemp[4];
    float InOut;
    float OutTemp[4];
    float OutPut;
}DirInnerInfotypedef;

extern DirInnerInfotypedef DirInner;
extern float Diraction_Out;

void Direction_Inner(void);

typedef struct
{
    float InnerInput;
    float BaseKI;
    float BaseKP;
    float BaseKD;
    float ErrorDtTemp[4];
    float ErrorFifo[10];
    float OutPut;
    float Expect_Curvature;
    float Curvature_Final;
    float Curvature_Correct;
}DirOutterInfotypedef;

extern DirOutterInfotypedef DirOutter;
void Direction_Outter(void);

extern DirOutterInfotypedef Force_loop;
void CentripetalForce(void);
#endif /* CONTROL_H_ */
