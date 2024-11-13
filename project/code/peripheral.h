/*
 * peripheral.h
 *
 *  Created on: 2024��7��26��
 *      Author: Dove
 */

#ifndef PERIPHERAL_H_
#define PERIPHERAL_H_

//����������
#define bee_gpio (C13)
//���Ʒ���ʱ��
extern uint16_t bee_time;

void ADC_MyInit(void);
float ADC_Convert(void);

void bee_Init(void);
void bee(void);

void Wireless_Uart_Init(void);
void Wireless_Uart_Sendfloat(float data);






#endif /* PERIPHERAL_H_ */
