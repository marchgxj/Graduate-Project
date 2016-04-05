//******************
// FileName: math.c
//
// Description:
//  --
//
// Author: 2016/1/19, by xiaoximi
//
#include "common.h"



unsigned int sqrt_16(unsigned long M)
{
// FunctionName: sqrt_16
//{
// Parameter:uint32 M:待开方数
//
// Return:uint16：开方后结果
//
// Description:
//  --
//
// Created: 2016/1/19
//
// Author:xiaoximi
//}
    
    uint16 result;
    uint16 i;
    unsigned long tmp, ttp;   
    if (M == 0)               
        return 0;
    
    result = 0;
    
    tmp = (M >> 30);          
    M <<= 2;
    if (tmp > 1)             
    {
        result ++;                 
        tmp -= result;
    }
    
    for (i=15; i>0; i--)     
    {
        result <<= 1;              
        
        tmp <<= 2;
        tmp += (M >> 30);     
        
        ttp = result;
        ttp = (ttp<<1)+1;
        
        M <<= 2;
        if (tmp >= ttp)       
        {
            tmp -= ttp;
            result ++;
        }
    }
    return result;
}


uint16 getAverage(uint16 *data,uint8 length)
{
// FunctionName: getAverage
//{
// Parameter:data:数据缓存 length:数据长度
//
// Return:数据缓存内的平均数
//
// Description:
//  --
//
// Created: 2016/1/19
//
// Author:xiaoximi
//}
    uint8 i = 0;
    uint32 count = 0;
    for(i=0;i<length;i++)
    {
        count += *data++;
    }
    return (uint16)(count / length);
}


uint16 getVariance(uint16 *data,uint8 length)
{
 // FunctionName: getVariance
//{
// Parameter:data:数据缓存 length:数据长度
//
// Return:数据缓存内的方差
//
// Description:
//  --
//
// Created: 2016/1/19
//
// Author:xiaoximi
//}
    
    uint16 average = 0;
    uint32 sum = 0;
    uint8 i = 0;
    if(length == 1)
    {
        return 0;
    }
    average = getAverage(data,length);
    for(i=0;i<length;i++)
    {
        sum += (*data-average)*(*data-average);
        data++;
    }
    return (uint16)(sum / length);
}

void bubbledata(DataStruct *a,uint16 length) 
{ 
    uint16 i,j;
    DataStruct temp;
    for(i=0;i<length-1;i++) 
    {
        for(j=i+1;j<length;j++) 
            if(a[i].value<a[j].value) 
            { 
                temp=a[i];
                a[i]=a[j]; 
                a[j]=temp; 
            }
        
    }
    
}


void sorted(uint16 *a,uint16 length)
{
// FunctionName: soted
//{
// Parameter:a:待排序的数据缓存 length:数据长度
//
// Return:
//
// Description:
//  --
//
// Created: 2016/1/19
//
// Author:xiaoximi
//}
    
    uint16 i,j;
    uint16 temp;
    for(i=0;i<length-1;i++) 
    {
        for(j=i+1;j<length;j++) 
            if(a[i]<a[j]) 
            { 
                temp=a[i];
                a[i]=a[j]; 
                a[j]=temp; 
            }
    }
}


uint32 pow(int16 buf,uint8 type)
{
// FunctionName: pow
//{
// Parameter:data：待处理数据 type：处理类型
//
// Return:处理结果
//
// Description:
//  type=0.5 开根号，type=2 平方
//
// Created: 2016/1/19
//
// Author:xiaoximi
//}
    
    uint32 result = 1;
    uint16 data = 0;
    data = abs(buf);

    if(type>3)
        return 0;
    else
    {
        for(int i=0;i<type;i++)
        {
            result *= data;
        }
    }
    return result;
}
