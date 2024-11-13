/*
 * gyro.h
 *
 *  Created on: 2024Äê8ÔÂ1ÈÕ
 *      Author: Dove
 */

#ifndef GYRO_H_
#define GYRO_H_

typedef struct
{
  float Yawrate;

}CarAgnleInfotypedef;


typedef struct
{
    float TurnAngle_Integral;
    float offset;
} GYRO_CLASS;

extern CarAgnleInfotypedef CarAngle;
extern GYRO_CLASS gyro;


void LPF_1_db(float hz, float time, float in, float *out);
void Get_Attitude(void);
void gyroscopeGetParameter(uint16 num);
void gyroscopeInit(void);


#endif /* GYRO_H_ */
