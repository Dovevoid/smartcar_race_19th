/*
 * encode.c
 *
 *  Created on: 2024��7��31��
 *      Author: Dove
 */

#include  "zf_common_headfile.h"
# include "headfile.h"

int16 encode_count = 0;
float speed_avr = 0.0;
float distance = 0.0;


/***��������ʼ��***/
void encode_Init(void)
{
    encoder_quad_init(ENCODER, ENCODER_A, ENCODER_B);                     // ��ʼ��������ģ�������� �������������ģʽ
}


/***��������ȡ����***/
//ע�⣺�����ж���ִ�У��ݶ�5ms
void GetSpeed(void)
{
    static int16 count_filter[4];

    encode_count = encoder_get_count(ENCODER);      // ��ȡ����������
    encoder_clear_count(ENCODER);                   // ��ձ���������

    //�˲�
    for (uint8 i = 3; i > 0; i--)
    {
        count_filter[i] = count_filter[i - 1];
    }
    count_filter[0]= encode_count;
    float count_Afterfilter;
    count_Afterfilter = 0.4*count_filter[0]+0.3*count_filter[1]+0.2*count_filter[2]+0.1*count_filter[3];

    //Ӧ���ٶ�һ���˲�  ����(M/S)=������ֵ*����Ƶ��/һ�׼���ֵ��
    speed_avr = speed_avr*0.15 + 0.85*count_Afterfilter* SPEED_F / meter_count;

    //����
    if (speed_avr > 2)
    {
        speed_avr = 2;
    }

    //����ֵ�ۼӵþ��루��λcm��������ʵ�ʵ���ϵ��meter_count
    distance += count_Afterfilter/meter_count*100;

}

