#include "zf_common_headfile.h"
#include "headfile.h"

float lastError;//偏差
uint8 stop_flag;//丢线信号
int16 lose_line = 0 ;

findline_TypeDef Findline = {0};
fp_TypeDef Findp= {0};
int16 qianzhan_init = 72;//70
int16 qianzhan = 72;//前瞻修改

float lastError;
float chuerr=21.25;  //出赛道幅度***
float huierr=18;     //回赛道幅度***

uint8 widthlose=0;

/*
函数名称：int my_abs(int value)
功能说明：求绝对值
参数说明：
函数返回：绝对值
修改时间：2022年9月8日
备    注：
example：  my_abs( x)；
 */
int my_abs(int value)
{
if(value>=0) return value;
else return -value;
}

int16 limit_a_b(int16 x, int a, int b)
{
    if(x<a) x = a;
    if(x>b) x = b;
    return x;
}

/*
函数名称：int16 limit(int16 x, int16 y)
功能说明：求x,y中的最小值
参数说明：
函数返回：返回两值中的最小值
修改时间：2022年9月8日
备    注：
example：  limit( x,  y)
 */
int16 limit1(int16 x, int16 y)
{
    if (x > y)             return y;
    else if (x < -y)       return -y;
    else                return x;
}



/*
函数名称：void get_start_point(uint8 start_row)
功能说明：寻找两个边界的边界点作为八邻域循环的起始点
参数说明：输入任意行数
函数返回：无
修改时间：2022年9月8日
备    注：
example：  get_start_point(image_h-2)
 */
uint8 start_point_l[2] = { 0 };//左边起点的x，y值
uint8 start_point_r[2] = { 0 };//右边起点的x，y值
uint8 get_start_point(uint8 start_row)
{
    uint8 i = 0,l_found = 0,r_found = 0;
    //清零
    start_point_l[0] = 0;//x
    start_point_l[1] = 0;//y

    start_point_r[0] = 0;//x
    start_point_r[1] = 0;//y

        //从中间往左边，先找起点
    for (i = image_w / 2; i > border_min; i--)
    {
        start_point_l[0] = i;//x
        start_point_l[1] = start_row;//y
        if (mt9v03x_image_new[start_row][i] == 255 && mt9v03x_image_new[start_row][i - 1] == 0)
        {
            //printf("找到左边起点image[%d][%d]\n", start_row,i);
            l_found = 1;
            break;
        }
    }

    for (i = image_w / 2; i < border_max; i++)
    {
        start_point_r[0] = i;//x
        start_point_r[1] = start_row;//y
        if (mt9v03x_image_new[start_row][i] == 255 && mt9v03x_image_new[start_row][i + 1] == 0)
        {
            //printf("找到右边起点image[%d][%d]\n",start_row, i);
            r_found = 1;
            break;
        }
    }

    if(l_found&&r_found)return 1;
    else {
        //printf("未找到起点\n");
        return 0;
    }
}

/*
函数名称：void search_l_r(uint16 break_flag, uint8(*image)[image_w],uint16 *l_stastic, uint16 *r_stastic,
                            uint8 l_start_x, uint8 l_start_y, uint8 r_start_x, uint8 r_start_y,uint8*hightest)

功能说明：八邻域正式开始找右边点的函数，输入参数有点多，调用的时候不要漏了，这个是左右线一次性找完。
参数说明：
break_flag_r            ：最多需要循环的次数
(*image)[image_w]       ：需要进行找点的图像数组，必须是二值图,填入数组名称即可
                       特别注意，不要拿宏定义名字作为输入参数，否则数据可能无法传递过来
*l_stastic              ：统计左边数据，用来输入初始数组成员的序号和取出循环次数
*r_stastic              ：统计右边数据，用来输入初始数组成员的序号和取出循环次数
l_start_x               ：左边起点横坐标
l_start_y               ：左边起点纵坐标
r_start_x               ：右边起点横坐标
r_start_y               ：右边起点纵坐标
hightest                ：循环结束所得到的最高高度
函数返回：无
修改时间：2022年9月25日
备    注：
example：
    search_l_r((uint16)USE_num,image,&data_stastics_l, &data_stastics_r,start_point_l[0],
                start_point_l[1], start_point_r[0], start_point_r[1],&hightest);
 */
#define USE_num image_h*3   //定义找点的数组成员个数按理说300个点能放下，但是有些特殊情况确实难顶，多定义了一点

 //存放点的x，y坐标
uint16 points_l[(uint16)USE_num][2] = { {  0 } };//左线
uint16 points_r[(uint16)USE_num][2] = { {  0 } };//右线
uint16 dir_r[(uint16)USE_num] = { 0 };//用来存储右边生长方向
uint16 dir_l[(uint16)USE_num] = { 0 };//用来存储左边生长方向
uint16 data_stastics_l = 0;//统计左边找到点的个数
uint16 data_stastics_r = 0;//统计右边找到点的个数
uint8 hightest = 0;//最高点
void search_l_r(uint16 break_flag, uint8(*image)[image_w], uint16 *l_stastic, uint16 *r_stastic, uint8 l_start_x, uint8 l_start_y, uint8 r_start_x, uint8 r_start_y, uint8*hightest)
{

    uint8 i = 0, j = 0;

    //左边变量
    uint8 search_filds_l[8][2] = { {  0 } };
    uint8 index_l = 0;
    uint8 temp_l[8][2] = { {  0 } };
    uint8 center_point_l[2] = {  0 };
    uint16 l_data_statics;//统计左边
    //定义八个邻域
    static int8 seeds_l[8][2] = { {0,  1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,  0},{1, 1}, };
    //{-1,-1},{0,-1},{+1,-1},
    //{-1, 0},       {+1, 0},
    //{-1,+1},{0,+1},{+1,+1},
    //这个是顺时针

    //右边变量
    uint8 search_filds_r[8][2] = { {  0 } };
    uint8 center_point_r[2] = { 0 };//中心坐标点
    uint8 index_r = 0;//索引下标
    uint8 temp_r[8][2] = { {  0 } };
    uint16 r_data_statics;//统计右边
    //定义八个邻域
    static int8 seeds_r[8][2] = { {0,  1},{1,1},{1,0}, {1,-1},{0,-1},{-1,-1}, {-1,  0},{-1, 1}, };
    //{-1,-1},{0,-1},{+1,-1},
    //{-1, 0},       {+1, 0},
    //{-1,+1},{0,+1},{+1,+1},
    //这个是逆时针

    l_data_statics = *l_stastic;//统计找到了多少个点，方便后续把点全部画出来
    r_data_statics = *r_stastic;//统计找到了多少个点，方便后续把点全部画出来

    //第一次更新坐标点  将找到的起点值传进来
    center_point_l[0] = l_start_x;//x
    center_point_l[1] = l_start_y;//y
    center_point_r[0] = r_start_x;//x
    center_point_r[1] = r_start_y;//y

        //开启邻域循环
    while (break_flag--)
    {

        //左边
        for (i = 0; i < 8; i++)//传递8F坐标
        {
            search_filds_l[i][0] = center_point_l[0] + seeds_l[i][0];//x
            search_filds_l[i][1] = center_point_l[1] + seeds_l[i][1];//y
        }
        //中心坐标点填充到已经找到的点内
        points_l[l_data_statics][0] = center_point_l[0];//x
        points_l[l_data_statics][1] = center_point_l[1];//y
        l_data_statics++;//索引加一

        //右边
        for (i = 0; i < 8; i++)//传递8F坐标
        {
            search_filds_r[i][0] = center_point_r[0] + seeds_r[i][0];//x
            search_filds_r[i][1] = center_point_r[1] + seeds_r[i][1];//y
        }
        //中心坐标点填充到已经找到的点内
        points_r[r_data_statics][0] = center_point_r[0];//x
        points_r[r_data_statics][1] = center_point_r[1];//y

        index_l = 0;//先清零，后使用
        for (i = 0; i < 8; i++)
        {
            temp_l[i][0] = 0;//先清零，后使用
            temp_l[i][1] = 0;//先清零，后使用
        }

        //左边判断
        for (i = 0; i < 8; i++)
        {
            if (image[search_filds_l[i][1]][search_filds_l[i][0]] == 0
                && image[search_filds_l[(i + 1) & 7][1]][search_filds_l[(i + 1) & 7][0]] == 255)
            {
                temp_l[index_l][0] = search_filds_l[(i)][0];
                temp_l[index_l][1] = search_filds_l[(i)][1];
                index_l++;
                dir_l[l_data_statics - 1] = (i);//记录生长方向
            }

            if (index_l)
            {
                //更新坐标点
                center_point_l[0] = temp_l[0][0];//x
                center_point_l[1] = temp_l[0][1];//y
                for (j = 0; j < index_l; j++)
                {
                    if (center_point_l[1] > temp_l[j][1])
                    {
                        center_point_l[0] = temp_l[j][0];//x
                        center_point_l[1] = temp_l[j][1];//y
                    }
                }
            }

        }
        if ((points_r[r_data_statics][0]== points_r[r_data_statics-1][0]&& points_r[r_data_statics][0] == points_r[r_data_statics - 2][0]
            && points_r[r_data_statics][1] == points_r[r_data_statics - 1][1] && points_r[r_data_statics][1] == points_r[r_data_statics - 2][1])
            ||(points_l[l_data_statics-1][0] == points_l[l_data_statics - 2][0] && points_l[l_data_statics-1][0] == points_l[l_data_statics - 3][0]
                && points_l[l_data_statics-1][1] == points_l[l_data_statics - 2][1] && points_l[l_data_statics-1][1] == points_l[l_data_statics - 3][1]))
        {
            //printf("三次进入同一个点，退出\n");
            break;
        }
        if (my_abs(points_r[r_data_statics][0] - points_l[l_data_statics - 1][0]) < 2
            && my_abs(points_r[r_data_statics][1] - points_l[l_data_statics - 1][1] < 2)
            )
        {
            //printf("\n左右相遇退出\n");
            *hightest = (points_r[r_data_statics][1] + points_l[l_data_statics - 1][1]) >> 1;//取出最高点
            //printf("\n在y=%d处退出\n",*hightest);
            break;
        }
        if ((points_r[r_data_statics][1] < points_l[l_data_statics - 1][1]))
        {
            //printf("\n如果左边比右边高了，左边等待右边\n");
            continue;//如果左边比右边高了，左边等待右边
        }
        if (dir_l[l_data_statics - 1] == 7
            && (points_r[r_data_statics][1] > points_l[l_data_statics - 1][1]))//左边比右边高且已经向下生长了
        {
            //printf("\n左边开始向下了，等待右边，等待中... \n");
            center_point_l[0] = points_l[l_data_statics - 1][0];//x
            center_point_l[1] = points_l[l_data_statics - 1][1];//y
            l_data_statics--;
        }
        r_data_statics++;//索引加一

        index_r = 0;//先清零，后使用
        for (i = 0; i < 8; i++)
        {
            temp_r[i][0] = 0;//先清零，后使用
            temp_r[i][1] = 0;//先清零，后使用
        }

        //右边判断
        for (i = 0; i < 8; i++)
        {
            if (image[search_filds_r[i][1]][search_filds_r[i][0]] == 0
                && image[search_filds_r[(i + 1) & 7][1]][search_filds_r[(i + 1) & 7][0]] == 255)
            {
                temp_r[index_r][0] = search_filds_r[(i)][0];
                temp_r[index_r][1] = search_filds_r[(i)][1];
                index_r++;//索引加一
                dir_r[r_data_statics - 1] = (i);//记录生长方向
                //printf("dir[%d]:%d\n", r_data_statics - 1, dir_r[r_data_statics - 1]);
            }
            if (index_r)
            {

                //更新坐标点
                center_point_r[0] = temp_r[0][0];//x
                center_point_r[1] = temp_r[0][1];//y
                for (j = 0; j < index_r; j++)
                {
                    if (center_point_r[1] > temp_r[j][1])
                    {
                        center_point_r[0] = temp_r[j][0];//x
                        center_point_r[1] = temp_r[j][1];//y
                    }
                }

            }
        }


    }


    //取出循环次数
    *l_stastic = l_data_statics;
    *r_stastic = r_data_statics;

}
/*
函数名称：void get_left(uint16 total_L)
功能说明：从八邻域边界里提取需要的边线
参数说明：
total_L ：找到的点的总数
函数返回：无
修改时间：2022年9月25日
备    注：
example： get_left(data_stastics_l );
 */
void get_left(uint16 total_L)
{
    uint8 i = 0;
    uint16 j = 0;
    uint8 h = 0;
    //初始化
    for (i = 0;i<image_h;i++)
    {
        Findline.leftline[i] = border_min;
    }
    h = image_h - 2;
    //左边
    for (j = 0; j < total_L; j++)
    {
        //printf("%d\n", j);
        if (points_l[j][1] == h)
        {
            Findline.leftline[h] = points_l[j][0]+1;
        }
        else continue; //每行只取一个点，没到下一行就不记录
        h--;
        if (h == 0)
        {
            break;//到最后一行退出
        }
    }
}
/*
函数名称：void get_right(uint16 total_R)
功能说明：从八邻域边界里提取需要的边线
参数说明：
total_R  ：找到的点的总数
函数返回：无
修改时间：2022年9月25日
备    注：
example：get_right(data_stastics_r);
 */
void get_right(uint16 total_R)
{
    uint8 i = 0;
    uint16 j = 0;
    uint8 h = 0;
    for (i = 0; i < image_h; i++)
    {
        Findline.rightline[i] = border_max;//右边线初始化放到最右边，左边线放到最左边，这样八邻域闭合区域外的中线就会在中间，不会干扰得到的数据
    }
    h = image_h - 2;
    //右边0
    for (j = 0; j < total_R; j++)
    {
        if (points_r[j][1] == h)
        {
            Findline.rightline[h] = points_r[j][0] - 1;
        }
        else continue;//每行只取一个点，没到下一行就不记录
        h--;
        if (h == 0)break;//到最后一行退出
    }
}

//定义膨胀和腐蚀的阈值区间
#define threshold_max   255*5//此参数可根据自己的需求调节
#define threshold_min   255*2//此参数可根据自己的需求调节
void image_filter(uint8(*mt9v03x_image_new)[image_w])//形态学滤波，简单来说就是膨胀和腐蚀的思想
{
    uint16 i, j;
    uint32 num = 0;


    for (i = 1; i < image_h - 1; i++)
    {
        for (j = 1; j < (image_w - 1); j++)
        {
            //统计八个方向的像素值
            num =
                    mt9v03x_image_new[i - 1][j - 1] + mt9v03x_image_new[i - 1][j] + mt9v03x_image_new[i - 1][j + 1]
                + mt9v03x_image_new[i][j - 1] + mt9v03x_image_new[i][j + 1]
                + mt9v03x_image_new[i + 1][j - 1] + mt9v03x_image_new[i + 1][j] + mt9v03x_image_new[i + 1][j + 1];


            if (num >= threshold_max && mt9v03x_image_new[i][j] == 0)
            {

                mt9v03x_image_new[i][j] = 255;//白  可以搞成宏定义，方便更改

            }
            if (num <= threshold_min && mt9v03x_image_new[i][j] == 255)
            {

                mt9v03x_image_new[i][j] = 0;//黑

            }

        }
    }

}

/*
函数名称：void image_draw_rectan(uint8(*ov7725_uart_image_dec)[image_w])
功能说明：给图像画一个黑框
参数说明：uint8(*image)[image_w] 图像首地址
函数返回：无
修改时间：2022年9月8日
备    注：
example： image_draw_rectan(ov7725_uart_image_dec);
 */
void image_draw_rectan(uint8(*image)[image_w])
{

    uint8 i = 0;
    for (i = 0; i < image_h; i++)
    {
        image[i][0] = 0;
        image[i][1] = 0;
        image[i][image_w - 1] = 0;
        image[i][image_w - 2] = 0;

    }
    for (i = 0; i < image_w; i++)
    {
        image[0][i] = 0;
        image[1][i] = 0;
        //image[image_h-1][i] = 0;

    }
}

/*
函数名称：void findline(void)
功能说明：最终处理函数
参数说明：无
函数返回：无
修改时间：2022年9月8日
备    注：
example： image_process();
 */
void findlineb(void)
{
    uint16 i;
    uint8 hightest = 0;//定义一个最高行，tip：这里的最高指的是y值的最小
    /*这是离线调试用的*/
    //image_filter(&ov7725_uart_image_dec);//滤波
    image_draw_rectan(&mt9v03x_image_new[160]);//预处理
    //清零
    data_stastics_l = 0;
    data_stastics_r = 0;
    if (get_start_point(image_h - 2))//找到起点了，再执行八领域，没找到就一直找
    {
        //printf("正在开始八邻域\n");
        search_l_r((uint16)USE_num, &mt9v03x_image_new[160], &data_stastics_l, &data_stastics_r, start_point_l[0], start_point_l[1], start_point_r[0], start_point_r[1], &hightest);
        //printf("八邻域已结束\n");
        // 从爬取的边界线内提取边线 ， 这个才是最终有用的边线
        get_left(data_stastics_l);
        get_right(data_stastics_r);
    }
    for (i = hightest; i < image_h-1; i++)
        {
            Findline.midline[i] = (Findline.leftline[i] + Findline.rightline[i])/2;//求中线
            //求中线最好最后求，不管是补线还是做状态机，全程最好使用一组边线，中线最后求出，不能干扰最后的输出
        }

        EmergencyStop();
}

/**********************************根据图像寻找中线，基础，中间往两边************************************/
void findline(void)
{

    int16 i = 0, j = 0;
    //参数初始化
    lose_line = 0 ;
    Findline.midline[119] = 80;
    Findline.leftline[119] = 0;
    Findline.rightline[119] = 159;

    Findline.leftstartpoint = 0;//记录开始有左右边线时候的行值
    Findline.rightstartpoint = 0;

    Findline.l_lose=0;
    Findline.r_lose=0;

    //普通巡线
    {
        for(i = 118; i > 0; i--)
        {
            Findline.midline[i] = 0;
            Findline.leftline[i] = 0;
            Findline.rightline[i] = 159;
            Findline.leftlineflag[i] = 0;
            Findline.rightlineflag[i] = 0;
            Findline.endline = 0;

            //寻找左跳变点
            if(Findline.midline[i + 1] > 80)j = Findline.midline[i + 1] > 155 ? 155 : Findline.midline[i + 1];
            else j = Findline.midline[i + 1] < 4 ? 4 : Findline.midline[i + 1];  //先找到中间位置，从中间向左边开始找

            for(; j > 3; j--)
            {
                if(mt9v03x_image_new[i][j] == 255 && mt9v03x_image_new[i][j - 1] == 255 && mt9v03x_image_new[i][j - 2] == 0 && mt9v03x_image_new[i][j - 3] == 0)
                {
                    if(Findline.leftstartpoint == 0)
                        Findline.leftstartpoint = i;
                    Findline.leftline[i] = j - 1;
                    Findline.leftlineflag[i] = 1;
                    break;
                }
            }

            //寻找右跳变点
            if(Findline.midline[i + 1] > 80)j = Findline.midline[i + 1] > 155 ? 155 : Findline.midline[i + 1];
            else j = Findline.midline[i + 1] < 4 ? 4 : Findline.midline[i + 1];
            for(j = Findline.midline[i + 1]; j < 156; j++)
            {
                if(mt9v03x_image_new[i][j] == 255 && mt9v03x_image_new[i][j + 1] == 255 && mt9v03x_image_new[i][j + 2] == 0 && mt9v03x_image_new[i][j + 3] == 0)
                {
                    if(Findline.rightstartpoint == 0)
                        Findline.rightstartpoint = i;
                    Findline.rightline[i] = j + 1;
                    Findline.rightlineflag[i] = 1;
                    break;
                }
            }

            //存储中线数组
            Findline.midline[i] = (Findline.leftline[i] + Findline.rightline[i]) / 2;

            //检测赛道宽度用
            //load_width[i] = -Findline.leftline[i] + Findline.rightline[i];

            //边线都没找到
            if((Findline.leftlineflag[i] == 0) && (Findline.rightlineflag[i] == 0))
            {
                lose_line++;
                Findline.midline[i] = Findline.midline[i + 1];
            }
            else if(lose_line > 0)
                lose_line--;

            if(Findline.leftlineflag[i] == 0)
                Findline.l_lose++;

            if(Findline.rightlineflag[i] == 0)
                Findline.r_lose++;

            //找顶点(中线消失点)
            if(Findline.midline[i + 1] > 80)j = Findline.midline[i + 1] > 155 ? 155 : Findline.midline[i + 1];
            else j = Findline.midline[i + 1] < 4 ? 4 : Findline.midline[i + 1];
            if( mt9v03x_image_new[i - 1][j] == 0 && (mt9v03x_image_new[i - 1][j - 2] == 0 && mt9v03x_image_new[i - 1][j + 2] == 0)  &&  mt9v03x_image_new[i - 2][j] == 0 && (mt9v03x_image_new[i - 2][j - 2] == 0 && mt9v03x_image_new[i - 2][j + 2] == 0))
            {
                Findline.endline = i;
                break;
            }
            Findline.endline=max(Findline.endline,30);

        }
    }

    EmergencyStop();    //丢线停车

    widthlose=0;
    for(uint16 i=70;i>20;i--)
    {
        if(width[i]-(Findline.rightline[i]-Findline.leftline[i])>5)
        {
            widthlose++;
        }
    }
}

/*************************************计算偏差***************************/
void caculate_err(void)
{
    int16 i,sum;
    lastError = Findline.err[0];

    sum = 0;
    for(i = 59; i >= 1; i--)
        Findline.err[i] = Findline.err[i - 1]; //偏差滤波
    Findline.err[0] = 0.0;
    if(qianzhan > Findline.endline + 4)//<58
    {
        for(i = qianzhan; i > Findline.endline && i > qianzhan-4; i--) // 摄像头前瞻修改位置， 通过修改i的范围来控制前瞻 原来是8
        {
            sum++;
            Findline.err[0] += (float)( Findline.midline[i]-80);
        }
    Findline.err[0] = Findline.err[0] / sum;
    }
    else
        Findline.err[0] = (float)(Findline.midline[Findline.endline + 2] - 80);

    if (lastError * Findline.err[0] < 0 && fabs(lastError - Findline.err[0]) > 30)
    {
        Findline.err[0] = lastError;
    }

    if(start_flag==0)
    {
        Findline.err[0]=0;
    }

    if(right_lucounter==0&&youluzhang_flag)
    {
        Findline.err[0] = -chuerr;
    }
    if(left_lucounter==0&&zuoluzhang_flag)
    {
        Findline.err[0] = chuerr;
    }
    if(right_lucounter==1&&youluzhang_flag)
    {
        Findline.err[0] = huierr;
    }
    if(left_lucounter==1&&zuoluzhang_flag)
    {
        Findline.err[0] = -huierr;
    }

}

/***************************** 丢线停车**************************************/
void EmergencyStop(void)
{
    //思路：丢线时近处的几行大部分都是黑色的，只要这部分的黑色像素累计大于一个阈值就认为丢线
    int16 i = 0, j = 0, count_black = 0 ;
    for(i = 118; i > 115; i--)
    {
        for(j = 130; j >= 30; j--)
        {
            if(mt9v03x_image_new[i][j] == 0)
            {
                count_black = count_black + 1;
            }
        }
    }
    if(count_black > 280&&!stop_flag)    //像素点大于某一阈值并且满足停车标志位条件
    {
        Findline.loseflag = 1;
        stop();
    }
}

void dynamic_qianzhan(void)
{
    float err_temp;
//    if(fabs(Findline.err[0]) >= 40 ){err_temp = 40;}
//    else {
//        err_temp = fabs(Findline.err[0]);
//    }
//
//    qianzhan = qianzhan_init + 0.5*err_temp;

    err_temp = fabs(Findline.err[0]);

    if(err_temp <= 5 ){qianzhan = qianzhan_init;}
    else if (err_temp <= 12) {
        qianzhan = qianzhan_init + 5;
    }

    else if (err_temp <= 20) {
        qianzhan = qianzhan_init + 10;
    }

    else {
        qianzhan = qianzhan_init + 16;
    }

}



