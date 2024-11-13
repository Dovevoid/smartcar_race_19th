
#include "zf_common_headfile.h"
#include "headfile.h"
#include "isr.h"

#define TASK_ENABLE 0
#define IPS200_TYPE     (IPS200_TYPE_SPI)                                 // ˫������ ���������� ����궨����д IPS200_TYPE_PARALLEL8
                                                                                // �������� SPI ������ ����궨����д IPS200_TYPE_SPI

int main (void)
{
    clock_init(SYSTEM_CLOCK_120M);      // ��ʼ��оƬʱ�� ����Ƶ��Ϊ 120MHz
    debug_init();                       // ��ر��������������ڳ�ʼ��MPU ʱ�� ���Դ���

    // �˴���д�û����� ���������ʼ�������

    /***�����ʼ������***/
        CarInfo.SMode = CLOSELOOP;    //���ģʽѡ��  CLOSELOOP STOP OPENLOOP
        PID_init();
        Motor_Init();                  //�����ʼ��
        encode_Init();
        gyroscopeInit();               //�����ǳ�ʼ��
        key_init(5);                   //������ʼ��
        bee_Init();                    //��������ʼ��
        ADC_MyInit();                  //ADC��ʼ��
        ips200_init(IPS200_TYPE);      //IPS��Ļ��ʼ��
        ips200_show_string(10, 0, "ips200 init.");
        system_delay_ms(200);
        mt9v03x_init();                //����ͷ��ʼ��
        ips200_show_string(10, 0, "mt9v03x init.");
        system_delay_ms(200);
        ips200_clear();

        // �����������ʹ�����ߴ��ڽ����շ�
//        Wireless_Uart_Init();
//        seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIRELESS_UART);
//        // ��ʼ���������ʾ�����Ľṹ��
//        seekfree_assistant_oscilloscope_struct oscilloscope_data;
//        oscilloscope_data.channel_num = 2;

        PIT_Init();                    //�����жϳ�ʼ��
    // �˴���д�û����� ���������ʼ�������

    while(1)
    {
        // �˴���д��Ҫѭ��ִ�еĴ���

        /*20msִ��һ��*/
        if(taskNum[2] == TASK_ENABLE)
        {
            taskNum[2] = 20;
            fps_cnt[2]++;
            if(mt9v03x_finish_flag)
            {
                Image_deal(3);        //�Ҷ�ͼ����
                mt9v03x_finish_flag = 0;
                findline();           //��ͨѲ��
                find_element();       //����Ԫ��
                caculate_err();       //ƫ���˲�
                if( is_open_upspeed ){speed_decision();}
//                sendimg_binary_CHK((const uint8 *)mt9v03x_image_new, MT9V03X_W, MT9V03X_H,0,30);
                /*�ڻ�����*/
//            oscilloscope_data.data[0] = Diraction_Out;
//            oscilloscope_data.data[1] = Findline.err[0];
//                oscilloscope_data.data[2] = Force_loop.OutPut ;
//            seekfree_assistant_oscilloscope_send(&oscilloscope_data);

                /*�ٶȻ�����*/
//                oscilloscope_data.data[0] = speed_avr;
//                oscilloscope_data.data[1] = Speed_Control;
//                seekfree_assistant_oscilloscope_send(&oscilloscope_data);
            }
        }


        /*200msִ��һ��*/
        if (taskNum[1] == TASK_ENABLE)  //UI������ʾ
        {
            taskNum[1] = 200;
            UI_DisplayMenu();
        }

        // �˴���д��Ҫѭ��ִ�еĴ���
    }
}

