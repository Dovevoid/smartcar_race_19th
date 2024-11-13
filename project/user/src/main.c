
#include "zf_common_headfile.h"
#include "headfile.h"
#include "isr.h"

#define TASK_ENABLE 0
#define IPS200_TYPE     (IPS200_TYPE_SPI)                                 // 双排排针 并口两寸屏 这里宏定义填写 IPS200_TYPE_PARALLEL8
                                                                                // 单排排针 SPI 两寸屏 这里宏定义填写 IPS200_TYPE_SPI

int main (void)
{
    clock_init(SYSTEM_CLOCK_120M);      // 初始化芯片时钟 工作频率为 120MHz
    debug_init();                       // 务必保留，本函数用于初始化MPU 时钟 调试串口

    // 此处编写用户代码 例如外设初始化代码等

    /***外设初始化代码***/
        CarInfo.SMode = CLOSELOOP;    //电机模式选择  CLOSELOOP STOP OPENLOOP
        PID_init();
        Motor_Init();                  //电机初始化
        encode_Init();
        gyroscopeInit();               //陀螺仪初始化
        key_init(5);                   //按键初始化
        bee_Init();                    //蜂鸣器初始化
        ADC_MyInit();                  //ADC初始化
        ips200_init(IPS200_TYPE);      //IPS屏幕初始化
        ips200_show_string(10, 0, "ips200 init.");
        system_delay_ms(200);
        mt9v03x_init();                //摄像头初始化
        ips200_show_string(10, 0, "mt9v03x init.");
        system_delay_ms(200);
        ips200_clear();

        // 设置逐飞助手使用无线串口进行收发
//        Wireless_Uart_Init();
//        seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIRELESS_UART);
//        // 初始化逐飞助手示波器的结构体
//        seekfree_assistant_oscilloscope_struct oscilloscope_data;
//        oscilloscope_data.channel_num = 2;

        PIT_Init();                    //周期中断初始化
    // 此处编写用户代码 例如外设初始化代码等

    while(1)
    {
        // 此处编写需要循环执行的代码

        /*20ms执行一次*/
        if(taskNum[2] == TASK_ENABLE)
        {
            taskNum[2] = 20;
            fps_cnt[2]++;
            if(mt9v03x_finish_flag)
            {
                Image_deal(3);        //灰度图像处理
                mt9v03x_finish_flag = 0;
                findline();           //普通巡线
                find_element();       //处理元素
                caculate_err();       //偏差滤波
                if( is_open_upspeed ){speed_decision();}
//                sendimg_binary_CHK((const uint8 *)mt9v03x_image_new, MT9V03X_W, MT9V03X_H,0,30);
                /*内环调试*/
//            oscilloscope_data.data[0] = Diraction_Out;
//            oscilloscope_data.data[1] = Findline.err[0];
//                oscilloscope_data.data[2] = Force_loop.OutPut ;
//            seekfree_assistant_oscilloscope_send(&oscilloscope_data);

                /*速度环调试*/
//                oscilloscope_data.data[0] = speed_avr;
//                oscilloscope_data.data[1] = Speed_Control;
//                seekfree_assistant_oscilloscope_send(&oscilloscope_data);
            }
        }


        /*200ms执行一次*/
        if (taskNum[1] == TASK_ENABLE)  //UI部分显示
        {
            taskNum[1] = 200;
            UI_DisplayMenu();
        }

        // 此处编写需要循环执行的代码
    }
}

