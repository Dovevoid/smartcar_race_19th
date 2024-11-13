/*
 * find_element.c
 *
 *  Created on: 2024年8月2日
 *      Author: Izzo
 */
#include "zf_common_headfile.h"
#include "headfile.h"
#include "find_element.h"

uint8 element_mode = 0;//决定进不进环,处不处理障碍 0进 1不进***


//直道参数
uint8 straight_flag = 0;
float straight_speed = 1.5;
uint8 is_open_upspeed = 0;

//拐点相关参数(标志位、行值、列值)
uint8 leftguaiflag_down=0, leftguai_column_down=0,leftguai_row_down=0;//左下
uint8 rightguaiflag_down=0, rightguai_column_down=0, rightguai_row_down=0;//右下
uint8 leftguaiflag_up=0,leftguai_column_up=0,leftguai_row_up=0;//左上
uint8 rightguaiflag_up=0,rightguai_column_up=0,rightguai_row_up=0;//右上
int16 left_flag[160]={0};
int16 left_row[160]={0};
int16 right_flag[160]={0};
int16 right_row[160]={0};

//十字参数
uint8 Lcrossing_flag=0;
uint8 Rcrossing_flag=0;
uint8 Mcrossing_flag=0;
uint8 Mcrossingcounter=0;
uint8 crossdistance=0;

//环岛相关参数
uint8 huandao_count=0;
uint8 zuohuandao_flag=0,youhuandao_flag=0; //左、右环岛判断标志
int16 zuohuandao_row;       //左环岛拐点行
int16 zuohuandao_lie;       //左环岛拐点列
int16 youhuandao_row;       //右环岛拐点行
int16 youhuandao_lie;       //右环岛拐点列
uint8 rightturncounter=0;   //右环岛处理的状态标志
uint8 leftturncounter=0;    //左环岛处理的状态标志
uint8 rightgocounter=0;
uint8 leftgocounter=0;
float huandistance=0;       //入环相关参数***

//路障参数
uint8 luzhang_flag,zuoluzhang_flag=0,youluzhang_flag=0;
uint8 right_lucounter=0;
uint8 left_lucounter=0;
float luzhangdistance=0;
float luzhangpoint=10;          //避障相关系数***
float luzhangangel=15;          //避障相关角度***
float huiangel=0;               //回归赛道角度***
float huipoint=120;             //决定从识别到路障到完全脱离路障状态的距离参数，防止路障后面紧接着特殊弯可能产生误判***

//斑马线参数
uint8 bm_flag;
uint8 bm_judge_flag = 0;
uint8 bmcounter=0;
float bmdistance=0;
float bmpoint;

//出发参数
uint8 start_flag=0;
float start_distance = 50;      //出发相关参数***

//赛道全宽，用于补线，不同车辆不一样
int16 width[120] = {0,2,4,6,8,10,11,12,13,14,
                    14,15,16,17,18,19,20,21,22,23,
                    24,25,26,27,28,29,30,31,32,33,
                    34,35,36,37,38,39,40,41,42,43,
                    44,45,46,47,48,49,50,51,52,53,
                    54,55,56,57,58,59,60,61,62,63,
                    64,65,66,67,68,69,70,71,72,73,
                    74,75,76,77,78,79,80,81,82,83,
                    84,85,86,87,88,89,90,91,92,93,
                    94,95,96,97,98,99,100,101,102,103,
                    104,105,106,107,108,109,110,111,112,113,
                    114,115,116,117,118,119,120,121,122,124};

//找元素
void find_element()
{
    start();
    if(distance>50)
    {
        find_guai();

        Lcrossing_judge();
        if(Lcrossing_flag)
        {
            Lcrossing();
        }

        Rcrossing_judge();
        if(Rcrossing_flag)
        {
            Rcrossing();
        }

        Mcrossing_judge();
        if(Mcrossing_flag)
        {
            Mcrossing();
        }

        huandao_judge();
        if(zuohuandao_flag)
        {
            //stop();
            if(element_mode==0)
            {
                leftturn_s();
            }
            else
            {
                leftgo();
            }
        }
        if(youhuandao_flag)
        {
            //stop();
            if(element_mode==0)
            {
                rightturn_s();
            }
            else
            {
                rightgo();
            }
        }

        luzhang_judge();
        if(luzhang_flag)
        {
            //stop();
            luzhang();
        }

        bm_judge();
        if(bm_judge_flag)
        {
            bmstop();
        }
    }
}

/***找拐点***/
void find_guai(void)
{
    rightguai_row_up=0;
    leftguai_row_up=0;
    rightguai_column_up=0;
    leftguai_column_up=0;
    leftguaiflag_up=0;
    rightguaiflag_up=0;

    int16 i=0;
    int16 j=0;

    rightguaiflag_down=0;
    rightguai_row_down=0;
    leftguaiflag_down=0;
    leftguai_row_down=0;


//先找下拐点，再找上拐点
    for(i=113;i>40;i--)
    {
        if(Findline.leftline[i] < 142
           && Findline.leftline[i] > Findline.leftline[i+2] &&Findline.leftline[i]>Findline.leftline[i+6]
           && Findline.leftline[i] > Findline.leftline[i-4] &&Findline.leftline[i]>Findline.leftline[i-8])
        {
        //i>45是为了控制拐点在42行到110行内
            if(Findline.leftline[i-6]-Findline.leftline[i+2]<4&&Findline.leftlineflag[i+4]==1&&Findline.leftlineflag[i]==1)
            {
                if(2*Findline.leftline[i]-Findline.leftline[i-6]-Findline.leftline[i+6]>6&&i>Findline.endline+5)//拐点要在顶点之前
                {//寻找左拐点
                    leftguaiflag_down=1;
                    leftguai_row_down=i;
                    leftguai_column_down=Findline.leftline[i];
                    break;
                }//找到左下拐点,屏幕上绿色
            }
        }
    }

    for(i=113;i>40;i--)
    {
        if(Findline.rightline[i]>28
           &&Findline.rightline[i]<Findline.rightline[i+2]&&Findline.rightline[i]<Findline.rightline[i+6]
           &&Findline.rightline[i]<Findline.rightline[i-4]&&Findline.rightline[i]<Findline.rightline[i-8])
        {//i>45是为了控制拐点在40行到110行内
            if(Findline.rightline[i-6]-Findline.rightline[i+2]>4&&Findline.rightlineflag[i+4]==1&&Findline.rightlineflag[i]!=0)
            {
                if(Findline.rightline[i+6]+Findline.rightline[i-6]-2*Findline.rightline[i]>6&&i>Findline.endline+5)//拐点要在顶点之前
                {//寻找右拐点
                    rightguaiflag_down=1;
                    rightguai_row_down=i;
                    rightguai_column_down=Findline.rightline[i];
                    break;
                }//找到右下拐点,屏幕上黄色

            }
        }
    }

    //找上拐点
    for(j=8;j<=148;j++)
    {
        for(i=70;i>40;i--)
        {
            if(mt9v03x_image_new[i+20][j]==255&&mt9v03x_image_new[i+6][j] ==255&&mt9v03x_image_new[i+8][j] ==255&&mt9v03x_image_new[i+4][j] == 255&&mt9v03x_image_new[i+2][j] == 255&&mt9v03x_image_new[i][j] == 0&&mt9v03x_image_new[i-2][j]==0&&mt9v03x_image_new[i-6][j] ==0&&mt9v03x_image_new[i-10][j] ==0&&mt9v03x_image_new[i-20][j]==0)
            {
                left_flag[j]=1;
                left_row[j]=i;
                break;
            }
        }
    }
    for(j=8;j<=152;j++)
    {
        if(left_flag[j]==1&&left_flag[j-6]==1&&left_flag[j+6]==1&&Findline.leftlineflag[left_row[j]+2]==0&&mt9v03x_image_new[left_row[j]][j-4]==0 )
        {
            if((left_row[j]-left_row[j+6])/6-(left_row[j]-left_row[j-6])/6>0.6)
            {
                leftguaiflag_up=1;
                leftguai_row_up=left_row[j];
                leftguai_column_up=j;
                break;
            }
        }
    }

    for(uint8 j=152;j>=12;j--)
    {
        for(uint8 i=70;i>40;i--)
        {
            //找黑白跳变点
            if(mt9v03x_image_new[i+20][j]==255&&mt9v03x_image_new[i+6][j] ==255&&mt9v03x_image_new[i+8][j] ==255&&mt9v03x_image_new[i+4][j] == 255&&mt9v03x_image_new[i+2][j] == 255&&mt9v03x_image_new[i][j] == 0&&mt9v03x_image_new[i-2][j]==0&&mt9v03x_image_new[i-6][j] ==0&&mt9v03x_image_new[i-10][j] ==0&&mt9v03x_image_new[i-20][j]==0)
            {
                right_flag[j]=1;
                right_row[j]=i;
                break;
            }
         }
     }
     for(uint8 j=152;j>=8;j--)
     {
         if(right_flag[j]==1&&right_flag[j-6]==1&&right_flag[j+6]==1&&Findline.rightlineflag[right_row[j]+2]==0&&mt9v03x_image_new[right_row[j]-4][j]==0 )
         {
             if(-(right_row[j]-right_row[j+6])/6+(right_row[j]-right_row[j-6])/6>0.6)
             {
                 rightguaiflag_up=1;
                 rightguai_row_up=right_row[j];
                 rightguai_column_up=j;
                 break;
             }
         }
     }
}

/*******************************************************************************************************************************************************************/
void Lcrossing_judge(void)//左弯道斜入十字
{
    if(rightguaiflag_down
       &&rightguai_row_down>60&&rightguai_row_down<110
       &&rightguai_column_down>60
       &&!youhuandao_flag   //防止在环岛里发生误判
       &&!zuohuandao_flag
       &&!youluzhang_flag
       &&!zuoluzhang_flag
       )
    {
        uint8 loseline=0;
        for(uint8 i=rightguai_row_down-30; i<=rightguai_row_down+10;i++)
        {
            if(Findline.leftlineflag[i]==0)
            {
                loseline++;
            }
        }
        if(loseline>=35)
        {
            Lcrossing_flag=1;
            qianzhan=80;
            //bee_time=200;
        }
    }
    else
    {
        Lcrossing_flag=0;
        qianzhan=qianzhan_init;
    }
    if(leftguaiflag_down)
    {
        Lcrossing_flag=0;
    }
}

void Lcrossing(void) //十字补线
{
    uint8 rightloserow=119;
    for(uint8 i=rightguai_row_down;i<119;i++)
    {
        if(Findline.rightlineflag[i]==0)
        {
            rightloserow=i;
            break;
        }
    }
    uint8 k=(159-rightguai_column_down)/(rightloserow-rightguai_row_down);//根据图像最右下角坐标和右下拐点算出斜率
    for(uint8 x = min(30,Findline.endline);x<=rightguai_row_down;x++ )//从右下拐点开始拟合
    {
        Findline.rightline[x]=rightguai_column_down-(rightguai_row_down-x)/k/4;
        Findline.midline[x]=(Findline.rightline[x]+Findline.leftline[x])/2;
    }
}

void Rcrossing_judge(void)//右弯道斜入十字
{
    if(leftguaiflag_down
       &&leftguai_row_down>60&&leftguai_row_down<110
       &&leftguai_column_down<80
       &&!youhuandao_flag   //防止在环岛里发生误判
       &&!zuohuandao_flag
       &&!youluzhang_flag
       &&!zuoluzhang_flag
       )
    {
        uint8 loseline=0;
        for(uint8 i=leftguai_row_down-30; i<=leftguai_row_down+10;i++)
        {
            if(Findline.rightlineflag[i]==0)
            {
                loseline++;
            }
        }
        if(loseline>=35)
        {
            Rcrossing_flag=1;
            qianzhan=80;
            //bee_time=200;
        }
    }
    else
    {
        Rcrossing_flag=0;
        qianzhan=qianzhan_init;
    }
    if(rightguaiflag_down)
    {
        Rcrossing_flag=0;
    }
}

void Rcrossing(void)  //十字补线
{
    uint8 leftloserow=119;
    for(uint8 i=leftguai_row_down;i<119;i++)
    {
        if(Findline.leftlineflag[i]==0)
        {
            leftloserow=i;
            break;
        }
    }
    uint8 t=(leftguai_column_down-1)/(leftloserow-leftguai_row_down);//根据图像最左下角坐标和左下拐点算出斜率
    for(uint8 y = min(30,Findline.endline);y<=leftguai_row_down;y++ )//从左下拐点开始拟合
    {
        Findline.leftline[y]=leftguai_column_down+(leftguai_row_down-y)/t/4;
        Findline.midline[y]=(Findline.rightline[y]+Findline.leftline[y])/2;
    }
}

void Mcrossing_judge(void)
{
    if(Findline.l_lose>40&&Findline.r_lose>40&&Rcrossing_flag==0&&Lcrossing_flag==0
       &&!youhuandao_flag
       &&!zuohuandao_flag
       &&!youluzhang_flag
       &&!zuoluzhang_flag
       )
    {
        Mcrossing_flag=1;
        qianzhan=80;
        crossdistance=distance;
        bee_time=200;
    }
    else
    {
        Mcrossing_flag=0;
        qianzhan=qianzhan_init;
    }
}

void Mcrossing(void)  //十字补线
{
    if(Mcrossingcounter==0)
    {
        for(uint8 i=100;i>50;i--)
            Findline.midline[i]=80;
        if(distance-crossdistance>=20)
        {
            Mcrossing_flag=0;
            Mcrossingcounter=0;
            for(uint8 i=100;i>50;i--)
                Findline.midline[i]=Findline.midline[i]+0;
        }
    }
}

/***环岛***/
void huandao_judge(void)//环岛判断
{
    if(!youhuandao_flag && !zuohuandao_flag
       &&!luzhang_flag&&!youluzhang_flag && !zuoluzhang_flag     //如果在路障状态里就不用再去判断了
       && rightguaiflag_down && rightguai_row_down>=40//50
       &&!leftguaiflag_down) //右环岛判断
    {
        //bee_time=200;
        if(youhuandao_judge(rightguai_row_down))//右环岛特征细致判断
        {
            bee_time=200;
            youhuandao_row=rightguai_row_down;//右环岛拐点行
            youhuandao_flag=1;
            zuohuandao_row=0;
            zuohuandao_lie=0;
            zuohuandao_flag=0;
            gyro.TurnAngle_Integral=0; //用陀螺仪出环
        }
    }

    if(!zuohuandao_flag && !youhuandao_flag
       //&&!luzhang_flag&&!youluzhang_flag && !zuoluzhang_flag     //如果在路障状态里就不用再去判断了
       && leftguaiflag_down && leftguai_row_down>=40
       && !rightguaiflag_down)//左环岛判断
    {
        //stop();
        if(zuohuandao_judge(leftguai_row_down) )//左环岛特征细致判断
        {
            //stop();
            bee_time=200;
            zuohuandao_row=rightguai_row_down;//左环岛拐点行
            zuohuandao_flag=1;
            youhuandao_row=0;
            youhuandao_lie=0;
            youhuandao_flag=0;
            gyro.TurnAngle_Integral=0; //用陀螺仪出环
        }
    }
}

uint8 zuohuandao_judge(int16 row)//左环岛细致检测
{
    uint8 flag=0,non_lose_line=0;
    //跳变点上方赛道变宽
    if((Findline.rightline[row-5]-Findline.leftline[row-5]) > (Findline.rightline[row]-Findline.leftline[row])
      &&(Findline.rightline[row-5]-Findline.leftline[row-5]) > width[row]-2
      &&Findline.rightlineflag[row-5]!=0
      &&Findline.rightlineflag[row-10]!=0
      &&youhuandao_flag==0&&zuohuandao_flag==0)
    {
        {
            for (uint8 i=row;i>=row-40 && i >= 0;i--)
            {
                if(Findline.leftlineflag[i]!=0)
                {
                    non_lose_line++;
                }
            }
            if(non_lose_line>=5)
            {
                flag=1;
            }
        }
        if(row<70)
        {
            flag=0;
        }
    }


    return flag;
}

uint8 youhuandao_judge(int16 row)//右环岛细致检测
{
    uint8 flag=0,non_lose_line=0;
    //跳变点上方赛道变宽
    if((Findline.rightline[row-5]-Findline.leftline[row-5]) > (Findline.rightline[row]-Findline.leftline[row])
      &&(Findline.rightline[row-5]-Findline.leftline[row-5]) > width[row]-2
      &&Findline.leftlineflag[row-5]!=0
      &&Findline.leftlineflag[row-10]!=0
      &&youhuandao_flag==0&&zuohuandao_flag==0)
    {
        {
            for (uint8 i=row;i>=row-40 && i >= 0;i--)
            {
                if(Findline.rightlineflag[i]!=0)
                {
                    non_lose_line++;
                }
            }

                if(non_lose_line>=5)
                {
                    flag=1;
                }
        }
        if(row<70)
        {
            flag=0;
        }
    }

    return flag;
}

void leftturn_s(void) //进环岛左转程序
{
    if(leftturncounter==0)
    {
        for(uint8 i=100;i>50;i--)
            Findline.midline[i]=Findline.rightline[i]-((int16)width[i])/2;
        if(Findline.leftlineflag[72] == 0&&Findline.leftlineflag[73] == 0&&Findline.leftlineflag[74] == 0
           &&Findline.leftlineflag[75] == 0&&Findline.leftlineflag[76] == 0&&Findline.leftlineflag[77] == 0
           &&Findline.leftlineflag[78] == 0&&Findline.leftlineflag[79] == 0&&Findline.leftlineflag[80] == 0)
        {
            leftturncounter=1;
            gyro.TurnAngle_Integral=0;
            speed.Stan =speed.Init+0.2;
            huandistance=distance;
            //qianzhan=90;
        }
    }

    if(leftturncounter==1)
    {
        //stop();
        for(uint8 i=100;i>50;i--)
            Findline.midline[i]=Findline.rightline[i]-((int16)width[i])/2;
        if(
           (Findline.leftlineflag[70] == 1&&Findline.leftlineflag[71] == 1&&Findline.leftlineflag[72] == 1
           &&Findline.leftlineflag[75] == 1&&Findline.leftlineflag[76] == 1&&Findline.leftlineflag[77] == 1)
           //||distance-huandistance>50
           )
        {
            leftturncounter=2;
            gyro.TurnAngle_Integral=0;
            qianzhan=60;
        }
    }

//    if(leftturncounter==2)
//    {
//        //stop();
//        for(uint8 i=100;i>50;i--)
//            Findline.midline[i]=Findline.rightline[i]-((int16)width[i])/2;
//        if(Findline.l_lose>50
//          &&Findline.leftlineflag[78] == 0&&Findline.leftlineflag[80] == 0&&Findline.leftlineflag[82] == 0
//            )
//        {
//            leftturncounter=3;
//            gyro.TurnAngle_Integral=0;
//        }
//    }

    if(leftturncounter==2)
    {
        //stop();
        for(uint8 i=100;i>50;i--)
        {
            if(Findline.leftline[i]+((int16)width[i])/1.2-20<=159&&Findline.leftline[i]+((int16)width[i])/1.2-20>=0)
            {
                Findline.midline[i]=Findline.leftline[i]+((int16)width[i])/1.2-20 ; //-40
                if(qianzhan > Findline.endline + 2)
                {
                    for(uint8 i=100;i>70;i--)
                        Findline.midline[i]=Findline.leftline[i]+((int16)width[i])/1.2-20 ;
                }
                else
                {
                    Findline.midline[Findline.endline + 2]=Findline.leftline[Findline.endline + 2]+((int16)width[Findline.endline + 2])/1.2-20;
                }
            }
        }
        if(gyro.TurnAngle_Integral<=-20)
        {
            leftturncounter=4;
            qianzhan=qianzhan_init;
            //speed.Stan =1.1;
        }
    }

    if(leftturncounter==4)
    {
        //stop();
        for(uint8 i=100;i>50;i--)
        {
            Findline.midline[i]=Findline.midline[i]+0;
        }
        if(gyro.TurnAngle_Integral<=-240&&rightguaiflag_down)
        {
            leftturncounter=5;
        }
    }

    if(leftturncounter==5)
    {
        //stop();
        for(uint8 i=100;i>50;i--)
        {
            if(i>Findline.endline)
            {
            Findline.midline[i]=Findline.leftline[i]+((int16)width[i])/1.2-15;
            }
        }
        if(gyro.TurnAngle_Integral<=-320)
        {
            leftturncounter=6;
            huandistance=distance;
        }
    }

    if(leftturncounter==6)
    {
        for(uint8 i=100;i>50;i--)
            Findline.midline[i]=Findline.rightline[i]-((int16)width[i])/2;
        if(distance-huandistance>100)
        {
            leftturncounter=7;
        }
    }

    if(leftturncounter==7)
    {
        for(uint8 i=100;i>50;i--)
            Findline.midline[i]=Findline.midline[i]+0;
        speed.Stan =speed.Init;
        qianzhan=qianzhan_init;
        leftturncounter=0;
        zuohuandao_flag = 0;
    }
}

void rightturn_s(void) //进环岛右转程序
{
    if(rightturncounter==0)
    {
        for(uint8 i=100;i>50;i--)
            Findline.midline[i]=Findline.leftline[i]+((int16)width[i])/2;
        if(Findline.rightlineflag[72] == 0&&Findline.rightlineflag[73] == 0&&Findline.rightlineflag[74] == 0
           &&Findline.rightlineflag[75] == 0&&Findline.rightlineflag[76] == 0&&Findline.rightlineflag[77] == 0
           &&Findline.rightlineflag[78] == 0&&Findline.rightlineflag[79] == 0&&Findline.rightlineflag[80] == 0)
        {
            rightturncounter=1;
            gyro.TurnAngle_Integral=0;
            speed.Stan =speed.Init+0.2;
            //huandistance=distance;
        }
    }

    if(rightturncounter==1)
    {
        //stop();
        for(uint8 i=100;i>50;i--)
            Findline.midline[i]=Findline.leftline[i]+((int16)width[i])/2;
        if((Findline.rightlineflag[70] == 1&&Findline.rightlineflag[71] == 1&&Findline.rightlineflag[72] == 1
           &&Findline.rightlineflag[75] == 1&&Findline.rightlineflag[76] == 1&&Findline.rightlineflag[77] == 1)
           //||distance-huandistance>ruhuanpoint
           )
        {
            rightturncounter=2;
            gyro.TurnAngle_Integral=0;
            qianzhan=60;
        }
    }

//    if(rightturncounter==2)
//    {
//        //stop();
//        for(uint8 i=100;i>50;i--)
//            Findline.midline[i]=Findline.leftline[i]+((int16)width[i])/2;
//        if(Findline.r_lose>30
//           &&rightguaiflag_up
//           //&&Findline.rightlineflag[88] == 0&&Findline.rightlineflag[90] == 0&&Findline.rightlineflag[92] == 0
//           )
//        {
//            rightturncounter=3;
//            gyro.TurnAngle_Integral=0;
//        }
//    }

    if(rightturncounter==2)
    {
        //stop();
        for(uint8 i=100;i>50;i--)
        {
            if(Findline.rightline[i]-((int16)width[i])/1.2+20>=0&&Findline.rightline[i]-((int16)width[i])/1.2+20<=159)
            {
                Findline.midline[i]=Findline.rightline[i]-((int16)width[i])/1.2+20 ;//+40
                if(qianzhan > Findline.endline + 2)
                {
                    for(uint8 i=100;i>70;i--)
                        Findline.midline[i]=Findline.rightline[i]-((int16)width[i])/1.2+20 ;
                }
                else
                {
                    Findline.midline[Findline.endline + 2]=Findline.rightline[Findline.endline + 2]-((int16)width[Findline.endline + 2])/1.2+20;
                }
            }
        }
        if(gyro.TurnAngle_Integral>=20)//30
        {
            rightturncounter=4;
            //qianzhan=70;
            qianzhan=qianzhan_init;
            //speed.Stan =speed.Init+0.2;
        }
    }

    if(rightturncounter==4)
    {
        for(uint8 i=100;i>50;i--)
        {
            Findline.midline[i]=Findline.midline[i]+0;
        }
        if(gyro.TurnAngle_Integral>=240&&leftguaiflag_down)
        {
            rightturncounter=5;
        }
    }

    if(rightturncounter==5)
    {
        //stop();
        for(uint8 i=100;i>50;i--)
        {
            if(i>Findline.endline)
            {
                Findline.midline[i]=Findline.rightline[i]-((int16)width[i])/1.2+15;//1.3
            }
        }
        if(gyro.TurnAngle_Integral>=320)
        {
            rightturncounter=6;
            huandistance=distance;
        }
    }

    if(rightturncounter==6)
    {
        //stop();
        for(uint8 i=100;i>50;i--)
            Findline.midline[i]=Findline.leftline[i]+((int16)width[i])/2;
        if(distance-huandistance>100)
        {
            rightturncounter=7;
        }
    }

    if(rightturncounter==7)
    {
        //stop();
        for(uint8 i=100;i>50;i--)
            Findline.midline[i]=Findline.midline[i]+0;
        speed.Stan =speed.Init;
        qianzhan=qianzhan_init;
        rightturncounter=0;
        youhuandao_flag = 0;
    }
}

void rightgo(void) //不进环
{
    if(rightgocounter==0)
    {
        for(uint8 i=100;i>50;i--)
            Findline.midline[i]=Findline.leftline[i]+((int16)width[i])/2;
        if(Findline.rightlineflag[72] == 0&&Findline.rightlineflag[73] == 0&&Findline.rightlineflag[74] == 0
           &&Findline.rightlineflag[75] == 0&&Findline.rightlineflag[76] == 0&&Findline.rightlineflag[77] == 0
           &&Findline.rightlineflag[78] == 0&&Findline.rightlineflag[79] == 0&&Findline.rightlineflag[80] == 0)
        {
            rightgocounter=1;
            huandistance=distance;
        }
    }

    if(rightgocounter==1)
    {
        for(uint8 i=100;i>50;i--)
            Findline.midline[i]=Findline.leftline[i]+((int16)width[i])/2;
        if(distance-huandistance>=150)
        {
            for(uint8 i=100;i>50;i--)
                Findline.midline[i]=Findline.midline[i]+0;
            rightgocounter=0;
            youhuandao_flag = 0;
        }
    }
}

void leftgo(void) //不进环
{
    if(leftgocounter==0)
    {
        for(uint8 i=100;i>50;i--)
            Findline.midline[i]=Findline.rightline[i]-((int16)width[i])/2;
        if(Findline.leftlineflag[72] == 0&&Findline.leftlineflag[73] == 0&&Findline.leftlineflag[74] == 0
           &&Findline.leftlineflag[75] == 0&&Findline.leftlineflag[76] == 0&&Findline.leftlineflag[77] == 0
           &&Findline.leftlineflag[78] == 0&&Findline.leftlineflag[79] == 0&&Findline.leftlineflag[80] == 0)
        {
            leftgocounter=1;
            huandistance=distance;
        }
    }

    if(leftgocounter==1)
    {
        for(uint8 i=100;i>50;i--)
            Findline.midline[i]=Findline.rightline[i]-((int16)width[i])/2;
        if(distance-huandistance>=150)
        {
            for(uint8 i=100;i>50;i--)
                Findline.midline[i]=Findline.midline[i]+0;
            leftgocounter=0;
            zuohuandao_flag = 0;
        }
    }
}

/***路障***/
void luzhang_judge(void)//路障判断
{

    if(!luzhang_flag&&!youluzhang_flag && !zuoluzhang_flag     //如果在路障状态里就不用再去判断了
       &&!zuohuandao_flag && !youhuandao_flag   //如果在环岛里就不用去判断路障了
       &&!bm_judge_flag
       &&widthlose>12
       )
    {
        //speed.Stan =0.8;
        uint8 zuolu=0;
        uint8 youlu=0;
        for(int a=80;a>50;a--)
        {
            if(zuoluzhang_judge(a))
            {
                zuolu++;
                if(zuolu>5)
                {
                    luzhang_flag=1;
                    youluzhang_flag=0;
                    zuoluzhang_flag=1;
                    luzhangdistance=distance;
                    gyro.TurnAngle_Integral=0;
                    stop_flag=0;
                    break;
                }
            }

            if(youluzhang_judge(a))
            {
                youlu++;
                if(youlu>5)
                {
                    luzhang_flag=1;
                    youluzhang_flag=1;
                    zuoluzhang_flag=0;
                    luzhangdistance=distance;
                    gyro.TurnAngle_Integral=0;
                    stop_flag=0;
                    break;
                }
            }
        }
    }
}

uint8 zuoluzhang_judge(int16 row)
{
    uint8 flag=0;
    if(Findline.leftline[row-10]-Findline.leftline[row]>15      //左边线突变
       &&Findline.leftline[row-10]-Findline.leftline[row]<45    //突变量不能太大，否则十字处会误判
       //&&abs(Findline.leftline[row-40]-Findline.leftline[row-10])<5
       &&Findline.leftlineflag[row] == 1&&Findline.rightlineflag[row] == 1)
    {
        flag=1;
    }

    return flag;
}

uint8 youluzhang_judge(int16 row)
{
    uint8 flag=0;
    if(
       Findline.rightline[row]-Findline.rightline[row-10]>15      //右边线突变
       &&Findline.rightline[row]-Findline.rightline[row-10]<45    //突变量不能太大，否则十字处会误判
//       &&abs(Findline.rightline[row-10]-Findline.rightline[row-40])<5
       &&Findline.leftlineflag[row] == 1&&Findline.rightlineflag[row] == 1
       )
    {
        flag=1;
    }

    return flag;
}

void luzhang(void)
{
    if(zuoluzhang_flag)
    {
        //bee_time=200;
        if(left_lucounter==0)
        {
            if(gyro.TurnAngle_Integral>=luzhangangel&&distance-luzhangdistance>luzhangpoint)//
            {
                left_lucounter=1;
                //gyro.PitchAngle_Integral=0;
            }
        }
        if(left_lucounter==1)
        {
            if(gyro.TurnAngle_Integral<-huiangel)//||distance-luzhangdistance>huipoint
            {
                left_lucounter=2;
            }
        }
        if(left_lucounter==2)
        {
            //stop();
            if(distance-luzhangdistance>huipoint)
            {
                speed.Stan =speed.Init;
                luzhang_flag=0;
                zuoluzhang_flag=0;
                left_lucounter=0;
                stop_flag=1;
            }
        }
    }

    if(youluzhang_flag)
    {
        if(right_lucounter==0)
        {
            if(gyro.TurnAngle_Integral<=-luzhangangel&&distance-luzhangdistance>luzhangpoint)//
            {
                right_lucounter=1;
                //gyro.PitchAngle_Integral=0;
            }
        }
        if(right_lucounter==1)
        {
            if(gyro.TurnAngle_Integral>=huiangel)//||distance-luzhangdistance>huipoint
            {
                right_lucounter=2;
            }
        }
        if(right_lucounter==2)
        {
            if(distance-luzhangdistance>huipoint)
            {
                 speed.Stan =speed.Init;
                 luzhang_flag=0;
                 youluzhang_flag=0;
                 right_lucounter=0;
                 stop_flag=1;
            }
        }
    }
}

/***斑马线***/
void bm_judge(void) //判断到斑马线函数
{
    if (bm_judge_flag ==0 )
    {
        //元素优先级最高
        if (!zuohuandao_flag && !youhuandao_flag &&distance > 1000)
        {
            uint8 jumpcount = 0;
            uint8 jumpcount2 = 0;
            for (uint8 j=19;j<=149;j++ ) //数跳变点19-149
            {
                //前瞻前10行
                if (    (mt9v03x_image_new[qianzhan][j]==255&&mt9v03x_image_new[qianzhan][j+1]==0)    ||
                        (mt9v03x_image_new[qianzhan][j]==0&&mt9v03x_image_new[qianzhan][j+1]==255)
                    )
                {
                    jumpcount ++;
                }
                //前瞻前五行
                if (    (mt9v03x_image_new[qianzhan-5][j]==255&&mt9v03x_image_new[qianzhan-5][j+1]==0)    ||
                        (mt9v03x_image_new[qianzhan-5][j]==0&&mt9v03x_image_new[qianzhan-5][j+1]==255)
                    )
                {
                    jumpcount2 ++;
                }
            }
            if ((jumpcount >= 8 && jumpcount2 >= 8))
            {
                //表明找到入库点
                bm_judge_flag = 1;
                bee_time = 200;
                //记录编码器距离
                luzhang_flag=0;
                youluzhang_flag=0;
                zuoluzhang_flag=0;
                bmdistance=distance;
            }
        }
    }
}

void bmstop(void)
{
    if(bmcounter==0)
    {
        //改前瞻
        qianzhan = 50;
        if(distance-bmdistance > 25)
        {
            bmcounter=1;
        }
    }
    if(bmcounter==1)
    {
        qianzhan = qianzhan_init;
        if(distance-bmdistance > 50)
        {
            bmcounter=2;
            stop();
        }
    }
}

void start(void)
{
    if(start_flag==0)
    {
        if(distance > start_distance)
        {
            start_flag=1;
        }
    }
}

void speed_decision(void)
{

    if(!luzhang_flag&&!youluzhang_flag && !zuoluzhang_flag)
    {
        int16 err_sum = 0;
        float avr_sum = 0;
        uint8 count = 0;
        if(Findline.endline <= 20)
        {
            //统计前瞻到25行的平均误差
            for (uint8 i = qianzhan-15;i> 25;i = i-2)
            {
                count ++;
                err_sum += (Findline.midline[i]-80);
            }
            avr_sum = fabs(err_sum /count);

            //对平均误差进行分段，设置速度
            if(avr_sum<15){straight_flag = 1;speed.Stan = straight_speed;}
            else if(avr_sum < 20){straight_flag = 0;speed.Stan = speed.Init+0.05;}
            else {
                straight_flag = 0;
                speed.Stan = speed.Init;
            }
        }

        else {
            straight_flag = 0;
            speed.Stan = speed.Init;
        }
    }

}
