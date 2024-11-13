/*
 * peripheral.h
 *
 *  Created on: 2024年7月26日
 *      Author: Dove
 */

#ifndef PERIPHERAL_H_
#define PERIPHERAL_H_

//蜂鸣器引脚
#define bee_gpio (C13)
//控制蜂鸣时间
extern uint16_t bee_time;

void ADC_MyInit(void);
float ADC_Convert(void);

void bee_Init(void);
void bee(void);

void Wireless_Uart_Init(void);
void Wireless_Uart_Sendfloat(float data);






#endif /* PERIPHERAL_H_ */
