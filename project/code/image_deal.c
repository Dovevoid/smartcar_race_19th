/*
 * image_deal.c
 *
 *  Created on: 2024年8月1日
 *      Author: Dove
 */

#include "zf_common_headfile.h"
#include "headfile.h"


uint8 mt9v03x_image_new[MT9V03X_H][MT9V03X_W];//总钻风储存图像数组
uint8  threshold=185;                         //二值化阈值，根据实际调整


/***灰度图二值化处理***/
void Binarazation(void)
{
    int i,j;

    for(i=0;i<MT9V03X_H;i++)
    {
        for(j=0;j<MT9V03X_W;j++)
        {
            if(mt9v03x_image[i][j]>threshold)
            {
                mt9v03x_image_new[i][j]=255;
            }
            else
            {
                mt9v03x_image_new[i][j]=0;
            }
        }
    }

}

/***************************************************************************
 ** 函数功能: 大津法二值化算法
 ** 参    数: 图像指针，行数，列数
 ** 返 回 值: 二值化阈值
 ** 作    者:
 ** 注    意：运用了摄像头采集的灰度图像的全局变量
 *************************************************************************/
uint8 otsuThreshold(uint8 *image, uint16 col, uint16 row)
{
    #define GrayScale 256
    uint16 width = col;
    uint16 height = row;
    int pixelCount[GrayScale];
    float pixelPro[GrayScale];
    int i, j, pixelSum = width * height/4;
    uint8 threshold = 0;
    uint8* data = image;  //指向像素数据的指针
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }

    uint32 gray_sum=0;
    //统计灰度级中每个像素在整幅图像中的个数
    for (i = 0; i < height; i+=2)
    {
        for (j = 0; j < width; j+=2)
        {
            pixelCount[(int)data[i * width + j]]++;  //将当前的点的像素值作为计数数组的下标
            gray_sum+=(int)data[i * width + j];       //灰度值总和
        }
    }

    //计算每个像素值的点在整幅图像中的比例

    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;

    }

    //遍历灰度级[0,255]
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;


    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
    for (j = 0; j < GrayScale; j++)
    {

        w0 += pixelPro[j];  //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
        u0tmp += j * pixelPro[j];  //背景部分 每个灰度值的点的比例 *灰度值

        w1=1-w0;
        u1tmp=gray_sum/pixelSum-u0tmp;

        u0 = u0tmp / w0;              //背景平均灰度
        u1 = u1tmp / w1;              //前景平均灰度
        u = u0tmp + u1tmp;            //全局平均灰度
        deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
        if (deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;
            threshold = j;
        }
        if (deltaTmp < deltaMax)
        {
            break;
        }

    }

    return threshold;

}

/***************************************************************************
 ** 函数功能: 谷底最小值二值化算法
 ** 参    数: uint16 width : 图像宽度
 **         uint16 height: 图像高度
 ** 返 回 值: 二值化阈值
 ** 作    者: 广东工业大学大佬
 ** 注    意：运用了摄像头采集的灰度图像的全局变量
 *************************************************************************/
uint8 GuDiThreshold(uint16 width, uint16 height)     //计算二值化阈值：谷底最小值
{
    uint16 graynum[256] = {0};
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            uint8 pix = mt9v03x_image[i][j];
            graynum[pix]++;
        }
    }
    uint8 Max[2] = {0};
    uint8 index[2] = {0};
    for (int i = 0; i < 2; ++i)
    {
        for (uint16 j = 0; j < 256; ++j)
        {
            if (i == 0)
            {
                if (graynum[j] > Max[i])
                {
                    Max[i] = graynum[j];
                    index[i] = j;
                }
            }
            else
            {
                if (graynum[j] > Max[i] && graynum[j] != Max[0])
                {
                    Max[i] = graynum[j];
                    index[i] = j;
                }
            }
        }
    }
    if (index[0] > index[1])
    {
        uint8 temp = 0;
        temp = index[0];
        index[0] = index[1];
        index[1] = temp;
    }

    uint8 Min = 255, index_Min = 0;
    for (uint8 i = index[0]; i < index[1]; ++i)
    {
        if (graynum[i] < Min)
        {
            Min = graynum[i];
            index_Min = i;
        }
    }

    return index_Min;
}


/*
 *******************************************************************************************
 ** 函数功能: 类似一维Means的二值化阈值计算
 ** 参    数: width：图像宽度
 **         height：图像高度
 ** 返 回 值: 二值化阈值
 ********************************************************************************************
 */
uint8 OneDimensionalThreshold(uint16 width, uint16 height)
{
    int row,cloum;
    int G1,G2;
    int g1,g2;
    uint8 threshold=160,threshold_last=0;   //阈值与上一次阈值，初始化为不同的值，第一个阈值是认为随机设定的

    while(abs(threshold-threshold_last)>10)   //只有当连续两次计算的阈值相等时才会跳出while
    {
        //初始化数据
        G1=0;G2=0;
        g1=0;g2=0;
        //进行G1和G2的分类
        for(row=0;row<height;row++)
        {
            for(cloum=0;cloum<width;cloum++)
            {
                if(mt9v03x_image[row][cloum]>threshold)
                {
                    G1+=mt9v03x_image[row][cloum];
                    g1++;
                }
                else
                {
                    G2+=mt9v03x_image[row][cloum];
                    g2++;
                }
            }
        }
        //进行新阈值的计算
        threshold_last=threshold;       //保存上一次的阈值
        threshold=((G1/g1)+(G2/g2))/2;  //阈值=（G1平均值+G2平均值）/ 2
    }
    return threshold;
}


/*---------------------------------------------------------------
 【函    数】Bin_Image_Filter
 【功    能】过滤噪点
 【参    数】无
 【返 回 值】无
 【注意事项】
 ----------------------------------------------------------------*/
void Bin_Image_Filter (void)
{
    uint8 nr; //行
    uint8 nc; //列

    for (nr = 1; nr < MT9V03X_H - 1; nr++)
    {
        for (nc = 1; nc < MT9V03X_W - 1; nc = nc + 1)
        {
            if ((mt9v03x_image_new[nr][nc] == 0)
                    && (mt9v03x_image_new[nr - 1][nc] + mt9v03x_image_new[nr + 1][nc] + mt9v03x_image_new[nr][nc + 1] + mt9v03x_image_new[nr][nc - 1] > 510))
            {
                mt9v03x_image_new[nr][nc] = 255;
            }
            else if ((mt9v03x_image_new[nr][nc] == 255)
                    && (mt9v03x_image_new[nr - 1][nc] + mt9v03x_image_new[nr + 1][nc] + mt9v03x_image_new[nr][nc + 1] + mt9v03x_image_new[nr][nc - 1] < 510))
            {
                mt9v03x_image_new[nr][nc] = 0;
            }
        }
    }
}


/********************************************************************************************
 ** 函数功能: MT9V03X图像处理程序
 ** 参    数: uint8 MODE ,切换模式，0不用方法，1大津法，2谷底法，3一维means方法
 *********************************************************************************************/
void Image_deal(uint8 MODE)
{
    //根据场地条件选择 图像处理算法
    //动态阈值
    switch (MODE)
    {
        case 0:
            threshold=185;
            break;
        case 1:
            threshold = otsuThreshold(&mt9v03x_image[0][0], MT9V03X_W, MT9V03X_H);
            break;
        case 2:
            threshold = GuDiThreshold(MT9V03X_W, MT9V03X_H);
            break;
        case 3:
            threshold = OneDimensionalThreshold(MT9V03X_W, MT9V03X_H);
            break;
    }

    Binarazation();//二值化
    //Bin_Image_Filter ();//降噪
}
