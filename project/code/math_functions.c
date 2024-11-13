/*
 * math_functions.c
 *
 *  Created on: 2024年2月27日
 *      Author: Dove
 */

#include "math_functions.h"


/***找最大值的函数***/
uint8 max(uint8 a,uint8 b)
{
    uint8 i;
    i= (a>b)?a:b;
    return i;
}
/***找最小值的函数***/
uint8 min(uint8 a,uint8 b)
{
    uint8 i;
    i= (a<b)?a:b;
    return i;
}

/***一组数据计算方差***/
float get_var(int16* line,int16 begin,int16 end)
{
    int16 i,sum = 0;
    float com = 0,ave = 0;

    //使begin小于end
    if(begin > end)
    {
        i = begin;
        begin = end;
        end = i;
    }

    for(i = begin;i < end ; i++)
    {
        sum += *(line+i);
    }

    ave = sum*1.0/(end-begin+1);

    for(i = begin;i < end ; i++)
    {
        com += (*(line+i)-ave)*(*(line+i)-ave);
    }

    return com/(end-begin+1);
}



