/*
 * image_deal.c
 *
 *  Created on: 2024��8��1��
 *      Author: Dove
 */

#include "zf_common_headfile.h"
#include "headfile.h"


uint8 mt9v03x_image_new[MT9V03X_H][MT9V03X_W];//����索��ͼ������
uint8  threshold=185;                         //��ֵ����ֵ������ʵ�ʵ���


/***�Ҷ�ͼ��ֵ������***/
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
 ** ��������: ��򷨶�ֵ���㷨
 ** ��    ��: ͼ��ָ�룬����������
 ** �� �� ֵ: ��ֵ����ֵ
 ** ��    ��:
 ** ע    �⣺����������ͷ�ɼ��ĻҶ�ͼ���ȫ�ֱ���
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
    uint8* data = image;  //ָ���������ݵ�ָ��
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }

    uint32 gray_sum=0;
    //ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���
    for (i = 0; i < height; i+=2)
    {
        for (j = 0; j < width; j+=2)
        {
            pixelCount[(int)data[i * width + j]]++;  //����ǰ�ĵ������ֵ��Ϊ����������±�
            gray_sum+=(int)data[i * width + j];       //�Ҷ�ֵ�ܺ�
        }
    }

    //����ÿ������ֵ�ĵ�������ͼ���еı���

    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;

    }

    //�����Ҷȼ�[0,255]
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;


    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
    for (j = 0; j < GrayScale; j++)
    {

        w0 += pixelPro[j];  //��������ÿ���Ҷ�ֵ�����ص���ռ����֮��   ���������ֵı���
        u0tmp += j * pixelPro[j];  //�������� ÿ���Ҷ�ֵ�ĵ�ı��� *�Ҷ�ֵ

        w1=1-w0;
        u1tmp=gray_sum/pixelSum-u0tmp;

        u0 = u0tmp / w0;              //����ƽ���Ҷ�
        u1 = u1tmp / w1;              //ǰ��ƽ���Ҷ�
        u = u0tmp + u1tmp;            //ȫ��ƽ���Ҷ�
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
 ** ��������: �ȵ���Сֵ��ֵ���㷨
 ** ��    ��: uint16 width : ͼ����
 **         uint16 height: ͼ��߶�
 ** �� �� ֵ: ��ֵ����ֵ
 ** ��    ��: �㶫��ҵ��ѧ����
 ** ע    �⣺����������ͷ�ɼ��ĻҶ�ͼ���ȫ�ֱ���
 *************************************************************************/
uint8 GuDiThreshold(uint16 width, uint16 height)     //�����ֵ����ֵ���ȵ���Сֵ
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
 ** ��������: ����һάMeans�Ķ�ֵ����ֵ����
 ** ��    ��: width��ͼ����
 **         height��ͼ��߶�
 ** �� �� ֵ: ��ֵ����ֵ
 ********************************************************************************************
 */
uint8 OneDimensionalThreshold(uint16 width, uint16 height)
{
    int row,cloum;
    int G1,G2;
    int g1,g2;
    uint8 threshold=160,threshold_last=0;   //��ֵ����һ����ֵ����ʼ��Ϊ��ͬ��ֵ����һ����ֵ����Ϊ����趨��

    while(abs(threshold-threshold_last)>10)   //ֻ�е��������μ������ֵ���ʱ�Ż�����while
    {
        //��ʼ������
        G1=0;G2=0;
        g1=0;g2=0;
        //����G1��G2�ķ���
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
        //��������ֵ�ļ���
        threshold_last=threshold;       //������һ�ε���ֵ
        threshold=((G1/g1)+(G2/g2))/2;  //��ֵ=��G1ƽ��ֵ+G2ƽ��ֵ��/ 2
    }
    return threshold;
}


/*---------------------------------------------------------------
 ����    ����Bin_Image_Filter
 ����    �ܡ��������
 ����    ������
 ���� �� ֵ����
 ��ע�����
 ----------------------------------------------------------------*/
void Bin_Image_Filter (void)
{
    uint8 nr; //��
    uint8 nc; //��

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
 ** ��������: MT9V03Xͼ�������
 ** ��    ��: uint8 MODE ,�л�ģʽ��0���÷�����1��򷨣�2�ȵ׷���3һάmeans����
 *********************************************************************************************/
void Image_deal(uint8 MODE)
{
    //���ݳ�������ѡ�� ͼ�����㷨
    //��̬��ֵ
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

    Binarazation();//��ֵ��
    //Bin_Image_Filter ();//����
}
