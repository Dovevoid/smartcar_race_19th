/*
 * peripheral.c
 *
 *  Created on: 2024年7月26日
 *      Author: Dove
 */

#include "zf_common_headfile.h"
#include "peripheral.h"

uint16_t bee_time;


/***AD采样初始化***/
void ADC_MyInit(void)
{
    adc_init(ADC2_IN9_B1, ADC_12BIT);
}

/***电压测量***/
float ADC_Convert(void)
{
    uint16 voltage_adc = 0;
    float voltage_now = 0;
    voltage_adc = adc_convert(ADC2_IN9_B1);
    // 根据逐飞科技学习主板分压计算 测得应该是电池电压 1/11 那么最大量程 36.3V
    voltage_now = (float)11 * 3.3 * voltage_adc / 4095 ;
    // 由于电阻会有误差 ADC检测本身也会有一些误差 所以这里对应数值可能跟实际电压有较多误差
    // 0.2为实测误差
    return (voltage_now+0.2);
}

/***蜂鸣器初始化***/
void bee_Init(void)
{
   gpio_init(bee_gpio, GPO, 0, GPIO_PIN_CONFIG) ;
}

/***蜂鸣器响***/
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


/***无线串口初始化***/
void Wireless_Uart_Init(void)
{
    if(wireless_uart_init())                                                    // 判断是否通过初始化
    {
        while(1)                                                                // 初始化失败就在这进入死循环
        {
            bee_time = 50;
            system_delay_ms(100);                                               // 蜂鸣器叫表示异常
        }
    }
    wireless_uart_send_byte('\r');
    wireless_uart_send_byte('\n');
    wireless_uart_send_string("SEEKFREE wireless uart demo.\r\n");              // 初始化正常 输出测试信息
}


/***无线串口输出浮点数***/
void Wireless_Uart_Sendfloat(float data)
{
    float data_receive = data;
    char strValue[20];

    sprintf(strValue, "%0.2f", data_receive);       //构造输出字符串

    wireless_uart_send_string (strValue);
    wireless_uart_send_string(".\r\n");             //输出回车
}


