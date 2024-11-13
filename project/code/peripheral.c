/*
 * peripheral.c
 *
 *  Created on: 2024��7��26��
 *      Author: Dove
 */

#include "zf_common_headfile.h"
#include "peripheral.h"

uint16_t bee_time;


/***AD������ʼ��***/
void ADC_MyInit(void)
{
    adc_init(ADC2_IN9_B1, ADC_12BIT);
}

/***��ѹ����***/
float ADC_Convert(void)
{
    uint16 voltage_adc = 0;
    float voltage_now = 0;
    voltage_adc = adc_convert(ADC2_IN9_B1);
    // ������ɿƼ�ѧϰ�����ѹ���� ���Ӧ���ǵ�ص�ѹ 1/11 ��ô������� 36.3V
    voltage_now = (float)11 * 3.3 * voltage_adc / 4095 ;
    // ���ڵ��������� ADC��Ȿ��Ҳ����һЩ��� ���������Ӧ��ֵ���ܸ�ʵ�ʵ�ѹ�н϶����
    // 0.2Ϊʵ�����
    return (voltage_now+0.2);
}

/***��������ʼ��***/
void bee_Init(void)
{
   gpio_init(bee_gpio, GPO, 0, GPIO_PIN_CONFIG) ;
}

/***��������***/
void bee(void)
{
    if(bee_time)
    {
        bee_time--;
        gpio_set_level(bee_gpio,1);
    }
    else
    {
        gpio_set_level(bee_gpio,0);
    }
}


/***���ߴ��ڳ�ʼ��***/
void Wireless_Uart_Init(void)
{
    if(wireless_uart_init())                                                    // �ж��Ƿ�ͨ����ʼ��
    {
        while(1)                                                                // ��ʼ��ʧ�ܾ����������ѭ��
        {
            bee_time = 50;
            system_delay_ms(100);                                               // �������б�ʾ�쳣
        }
    }
    wireless_uart_send_byte('\r');
    wireless_uart_send_byte('\n');
    wireless_uart_send_string("SEEKFREE wireless uart demo.\r\n");              // ��ʼ������ ���������Ϣ
}


/***���ߴ������������***/
void Wireless_Uart_Sendfloat(float data)
{
    float data_receive = data;
    char strValue[20];

    sprintf(strValue, "%0.2f", data_receive);       //��������ַ���

    wireless_uart_send_string (strValue);
    wireless_uart_send_string(".\r\n");             //����س�
}


