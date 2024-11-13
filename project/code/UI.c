/*
 * UI.c
 *
 *  Created on: 2024年2月28日
 *      Author: Dove
 */

#include "zf_common_headfile.h"
#include "headfile.h"
#include "isr.h"


UI_CLASS ui =
{
    0,0
};

//记录显示页面的长度，方便切换光标
uint8_t dis_len[8] ={8,9,9,2,2,2,2,4};

char ui_Menu[10][17] =
{
    "   Showimage    ",
    "   Para         ",
    "   element      ",
    "   encoder      ",
    "   Angle        ",
    "   OUT_PID      ",
    "   IN_PID       ",
    "   Motor_PID    ",
    "   Voltage      ",
    "                "
};

char ui_para[10][17] =
{
    "   huipoint     ",
    "   lu_point     ",
    "   hui_angle    ",
    "   lu_angle     ",
    "    mode        ",
    "   huan_point   ",
    "   zd_flag      ",
    "   zd_speed     ",
    "   SPEED_SET:   ",
    "   start_dis    ",
};

char ui_element[10][17] =
{
    "   element_1:   ",
    "   element_2:   ",
    "   element_3:   ",
    "   element_4:   ",
    "   element_5:   ",
    "   element_6:   ",
    "   element_7:   ",
    "   element_8:   ",
    "   element_9:   ",
    "   element_10:  "
};

char ui_encoder[10][17] =
{
    "                ",
    "    speed:      ",
    "    distance:   ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                "
};

char ui_angle[10][17] =
{
    "   SPEED_Z:     ",
    "   Angle_Z:     ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                "
};

char ui_OUTPD[10][17] =
{
    "   OUTP:        ",
    "                ",
    "   OUTD:        ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                "
};

char ui_INPD[10][17] =
{
    "   INP:         ",
    "                ",
    "   IND:         ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                "
};

char ui_MotorPID[10][17] =
{
    "   MotorP:      ",
    "                ",
    "   MotorI:      ",
    "                ",
    "   MotorD:      ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                "
};


/***UI显示界面底层函数***/
void UI_DisplayPages(char strings[10][17])
{
    if (ui.page!=9) {
        for (uint8 i = 0; i < 10; i++)
        {
            if (i == ui.cursor)
            {
                strings[i][2] = '>';}
            else
            {
                strings[i][2] = ' ';}
            ips200_show_string(3, i*22, strings[i]);
        }

    }
}


/***UI显示主界面***/
uint8 img_flag=0;

void UI_DisplayMenu(void)
{
    if(ui.page == 0 )
    {
        UI_DisplayPages(ui_Menu);
        ips200_show_float(120, 178, ADC_Convert(), 2, 2);
    }
    //参数调节
    else if(ui.page == 1)
    {
        UI_DisplayPages(ui_para);
        ips200_show_float(130, 0,  huipoint, 2, 1);
        ips200_show_float(130, 22, luzhangpoint, 2, 1);
        ips200_show_float(130, 44, huiangel, 2, 1);
        ips200_show_float(130, 66,  luzhangangel, 2, 1);
        ips200_show_uint(130, 88, element_mode, 2);

//        ips200_show_float(130, 110, is_open_upspeed, 1,1);
//        ips200_show_float(130, 132,  straight_speed, 1, 2);
        ips200_show_float(130, 154,  speed.Init, 1, 2);

        ips200_show_float(130, 176, start_distance, 2, 1);

    }
    //元素类型记录
    else if(ui.page == 2)
    {
        UI_DisplayPages(ui_element);
//        ips200_show_uint(150, 0, roadType[0], 3);
//        ips200_show_uint(150, 22, roadType[1], 3);
//        ips200_show_uint(150, 44, roadType[2], 3);
//        ips200_show_uint(150, 66, roadType[3], 3);
//        ips200_show_uint(150, 88, roadType[4], 3);
//        ips200_show_uint(150, 110, roadType[5], 3);
//        ips200_show_uint(150, 132, roadType[6], 3);
//        ips200_show_uint(150, 154, roadType[7], 3);
//        ips200_show_uint(150, 176, roadType[8], 3);
//        ips200_show_uint(150, 198, roadType[9], 3);

    }

    //编码器
    else if(ui.page == 3)
    {
        UI_DisplayPages(ui_encoder);
        ips200_show_float(130, 22, speed_avr, 2, 2);
        ips200_show_float(130, 44, distance,8, 2);
    }

    //陀螺仪
    else if(ui.page == 4)
    {
        UI_DisplayPages(ui_angle);
        ips200_show_float(130, 0,  CarAngle.Yawrate, 2, 2);
        ips200_show_float(130, 22,  gyro.TurnAngle_Integral, 3, 2);
    }

    //外环PD
    else if(ui.page == 5)
    {
        UI_DisplayPages(ui_OUTPD);
        ips200_show_float(130, 0, DirOutter.BaseKP, 2, 2);
        ips200_show_float(130, 44, DirOutter.BaseKD, 2, 2);
    }
    //内环PD
    else if(ui.page == 6)
    {
        UI_DisplayPages(ui_INPD);
        ips200_show_float(130, 0,  DirInner.BaseKP, 3, 2);
        ips200_show_float(130, 44, DirInner.BaseKD, 2, 2);
    }
    //速度环
    else if(ui.page == 7)
    {
        UI_DisplayPages(ui_MotorPID);
        ips200_show_float(130, 0,  SpeedLoop.BaseKP, 2, 2);
        ips200_show_float(130, 44, SpeedLoop.BaseKI, 2, 2);
        ips200_show_float(130, 88, SpeedLoop.BaseKD, 2, 2);
    }

    /************显示图像***************/
    else if(ui.page == 9)
    {
        img_show();
    }

}



/***UI按键功能***/
void keyScan(void)
{
    /*************按键1短按*****************/
    if(key_get_state(KEY_1) == KEY_SHORT_PRESS)
    {
        ui.cursor++;
        //光标越界刷新
        if(ui.cursor > dis_len[ui.page])
        {
            ips200_clear();
            ui.cursor= 0;
        }
    }

    /*************按键2短按*****************/
    if(key_get_state(KEY_2) == KEY_SHORT_PRESS && ui.page != 0)
    {
        ui.page = 0;
        ui.cursor = 0;
        ips200_clear();
    }

    /*************按键2长按*****************/
    if(key_get_state(KEY_2) == KEY_LONG_PRESS && ui.page == 0)
    {
        if(ui.cursor==0)
        {
            ui.page = 9; // 9对应的是显示图像
            ips200_clear();
        }
        else {
            ui.page = ui.cursor;
            ui.cursor = 0;
            ips200_clear();
        }
    }
//
//
//    /*************按键3长按*****************/
//    if(key_get_state(KEY_3) == KEY_LONG_PRESS && ui.page == 2)
//    {
//        //保存元素类型、参数
//        save_roadtype();
//        bee_time = 50;
//    }
//
//    if(key_get_state(KEY_3) == KEY_LONG_PRESS && (ui.page != 2) )
//    {
//        RoadType_init();
//        bee_time = 50;
//    }
//
//
//
    /*************按键4长按*****************/
    if(key_get_state(KEY_4) == KEY_LONG_PRESS )
    {
        if (allout_flag ==0) {
            allout_flag = 1;//起飞
            bee_time = 50;
        } else {
            allout_flag = 0;//降落
            bee_time = 50;
        }
    }
//
//    /*************按键3短按*****************/
    if(key_get_state(KEY_3) == KEY_SHORT_PRESS)
    {
        if(ui.page == 1 && ui.cursor == 0)
        {
            num_add(&huipoint, 5);
        }

        if(ui.page == 1 && ui.cursor == 1)
        {
            num_add(&luzhangpoint, 5);
        }
        if(ui.page == 1 && ui.cursor == 2)
        {
            num_add(&huiangel, 1);
        }
        if(ui.page == 1 && ui.cursor == 3)
        {
            num_add(&luzhangangel, 1);
        }

        if(ui.page == 1 && ui.cursor == 4)
        {
            element_mode = element_mode +1;
        }

//        if(ui.page == 1 && ui.cursor == 5)
//        {
//            num_add(&ruhuanpoint, 5);
//        }


        if(ui.page == 1 && ui.cursor ==6)
        {
            is_open_upspeed = is_open_upspeed +1;
        }

        if(ui.page == 1 && ui.cursor == 7)
        {
            num_add(&straight_speed,0.1);
        }
       //加速度
        if(ui.page == 1 && ui.cursor ==8)
        {
            num_add(&speed.Init, 0.03);
        }

        if(ui.page == 1 && ui.cursor == 9)
        {
            num_add(&start_distance, 10);
        }

    }
    /*************按键4短按*****************/
    if(key_get_state(KEY_4) == KEY_SHORT_PRESS)
    {
        if(ui.page == 1 && ui.cursor == 0)
        {
            num_sub(&huipoint, 5);
        }

        if(ui.page == 1 && ui.cursor == 1)
        {
            num_sub(&luzhangpoint, 5);
        }
        if(ui.page == 1 && ui.cursor == 2)
        {
            num_sub(&huiangel, 1);
        }
        if(ui.page == 1 && ui.cursor == 3)
        {
            num_sub(&luzhangangel, 1);
        }

        if(ui.page == 1 && ui.cursor == 4)
        {
            element_mode = element_mode -1;
        }

        //        if(ui.page == 1 && ui.cursor == 5)
            //        {
            //            num_sub(&ruhuanpoint, 5);
        //        }

        if(ui.page == 1 && ui.cursor ==6)
        {
            is_open_upspeed = is_open_upspeed -1;
        }

        if(ui.page == 1 && ui.cursor == 7)
        {
            num_sub(&straight_speed,0.1);
        }

        if(ui.page == 1 && ui.cursor ==8)
        {
            num_sub(&speed.Init, 0.03);
        }

        if(ui.page == 1 && ui.cursor == 9)
        {
            num_sub(&start_distance, 10);
        }


    }
}

/***显示图像***/

void img_show(void)
{
//    ips200_clear();
    ips200_show_string(190,0,"FPS");
    ips200_show_uint(190, 20, (fps_cnt[0]), 2);     //帧率显示
    ips200_show_gray_image(5, 5,(const uint8 *)mt9v03x_image , MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H,threshold);       //二值化图像显示
////    ips200_show_gray_image(0, 0, mt9v03x_image_new, MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, 0);

    for(int i = 119+5; i >= 5 && i > 1; i--)
    {
       ips200_draw_point(Findline.midline[i-5] + 5, i, RGB565_RED);     //中线显示 红色
       ips200_draw_point(Findline.rightline[i-5] + 5, i, RGB565_RED);      //右边线显示 蓝色
       ips200_draw_point(Findline.leftline[i-5] + 5, i, RGB565_RED);     //左边线显示 紫色
    }
    ips200_draw_line(5, Findline.endline+5, 20, Findline.endline+5, RGB565_RED);        //最终行显示红色
    ips200_draw_line(5, qianzhan+5, 30, qianzhan+5, RGB565_GREEN);      //前瞻显示 绿色
    if(leftguaiflag_up)
    {
        for(int i=-3;i<=3;i++)
        {
            ips200_draw_point(leftguai_column_up + 5+i, leftguai_row_up-1+5, RGB565_BLUE);
            ips200_draw_point(leftguai_column_up + 5+i, leftguai_row_up-2+5, RGB565_BLUE);
            ips200_draw_point(leftguai_column_up + 5+i, leftguai_row_up-3+5, RGB565_BLUE);
            ips200_draw_point(leftguai_column_up + 5+i, leftguai_row_up+5, RGB565_BLUE);
            ips200_draw_point(leftguai_column_up + 5+i, leftguai_row_up+1+5, RGB565_BLUE);
            ips200_draw_point(leftguai_column_up + 5+i, leftguai_row_up+2+5, RGB565_BLUE);
            ips200_draw_point(leftguai_column_up + 5+i, leftguai_row_up+3+5, RGB565_BLUE);
        }
    }
    if(leftguaiflag_down)
    {
        for(int i=-3;i<=3;i++)
        {
            ips200_draw_point(leftguai_column_down + 5+i, leftguai_row_down-1+5, RGB565_GREEN);
            ips200_draw_point(leftguai_column_down + 5+i, leftguai_row_down-2+5, RGB565_GREEN);
            ips200_draw_point(leftguai_column_down + 5+i, leftguai_row_down-3+5, RGB565_GREEN);
            ips200_draw_point(leftguai_column_down + 5+i, leftguai_row_down+5, RGB565_GREEN);
            ips200_draw_point(leftguai_column_down + 5+i, leftguai_row_down+1+5, RGB565_GREEN);
            ips200_draw_point(leftguai_column_down + 5+i, leftguai_row_down+2+5, RGB565_GREEN);
            ips200_draw_point(leftguai_column_down + 5+i, leftguai_row_down+3+5, RGB565_GREEN);
        }
    }
    if(rightguaiflag_up)
    {
        for(int i=-3;i<=3;i++)
        {
            ips200_draw_point(rightguai_column_up + 5+i, rightguai_row_up-1+5, RGB565_PURPLE);
            ips200_draw_point(rightguai_column_up + 5+i, rightguai_row_up-2+5, RGB565_PURPLE);
            ips200_draw_point(rightguai_column_up + 5+i, rightguai_row_up-3+5, RGB565_PURPLE);
            ips200_draw_point(rightguai_column_up + 5+i, rightguai_row_up+5, RGB565_PURPLE);
            ips200_draw_point(rightguai_column_up + 5+i, rightguai_row_up+1+5, RGB565_PURPLE);
            ips200_draw_point(rightguai_column_up + 5+i, rightguai_row_up+2+5, RGB565_PURPLE);
            ips200_draw_point(rightguai_column_up + 5+i, rightguai_row_up+3+5, RGB565_PURPLE);
        }
    }
    if(rightguaiflag_down)
    {
        for(int i=-3;i<=3;i++)
        {
            ips200_draw_point(rightguai_column_down + 5+i, rightguai_row_down-1+5, RGB565_YELLOW);
            ips200_draw_point(rightguai_column_down + 5+i, rightguai_row_down-2+5, RGB565_YELLOW);
            ips200_draw_point(rightguai_column_down + 5+i, rightguai_row_down-3+5, RGB565_YELLOW);
            ips200_draw_point(rightguai_column_down + 5+i, rightguai_row_down+5, RGB565_YELLOW);
            ips200_draw_point(rightguai_column_down + 5+i, rightguai_row_down+1+5, RGB565_YELLOW);
            ips200_draw_point(rightguai_column_down + 5+i, rightguai_row_down+2+5, RGB565_YELLOW);
            ips200_draw_point(rightguai_column_down + 5+i, rightguai_row_down+3+5, RGB565_YELLOW);
        }
    }
    ips200_show_string(0,160,"ERR");
    ips200_show_float(44, 160, Findline.err[0], 2, 2);

    ips200_show_string(0,180,"LH");
    ips200_show_uint (20, 180, zuohuandao_flag, 2);
    ips200_show_uint (40, 180, leftturncounter, 2);
    ips200_show_string(60,180,"RH");
    ips200_show_uint (80, 180, youhuandao_flag, 2);
    ips200_show_uint (100, 180, rightturncounter, 2);
    ips200_show_string(120,180,"BM");
    ips200_show_uint (140, 180, bm_judge_flag, 2);

    ips200_show_string(0,200,"LC");
    ips200_show_uint (20, 200, Lcrossing_flag, 2);
    ips200_show_string(60,200,"RC");
    ips200_show_uint (80, 200, Rcrossing_flag, 2);
    ips200_show_string(120,200,"MC");
    ips200_show_uint (140, 200, Mcrossing_flag, 2);

    ips200_show_string(0,220,"LL");
    ips200_show_uint (20, 220, zuoluzhang_flag, 2);
    ips200_show_string(60,220,"RL");
    ips200_show_uint (80, 220, youluzhang_flag, 2);

    ips200_show_string(0,240,"Llose");
    ips200_show_int (50, 240,  Findline.l_lose, 3);
    ips200_show_string(80,240,"Rlose");
    ips200_show_int (130, 240, Findline.r_lose, 3);
    ips200_show_string(0,260,"Wlose");
    ips200_show_uint(50, 260, widthlose, 2);
}



void num_add(float* num, float step)
{
    *num += step;
}


void num_sub(float* num, float step)
{
    *num -= step;
    if(*num<0)
        *num = 0;
}
