//******************
// FileName: math.c
//
// Description:
//  --
//
// Author: 2016/1/19, by xiaoximi
//
#include "common.h"


// FunctionName: sqrt_16
//{
// Parameter:uint32 M:��������
//
// Return:uint16����������
//
// Description:
//  --
//
// Created: 2016/1/19
//
// Author:xiaoximi
//}
unsigned int sqrt_16(unsigned long M)
{
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

// FunctionName: getAverage
//{
// Parameter:data:���ݻ��� length:���ݳ���
//
// Return:���ݻ����ڵ�ƽ����
//
// Description:
//  --
//
// Created: 2016/1/19
//
// Author:xiaoximi
//}
uint16 getAverage(uint16 *data,uint8 length)
{
    uint8 i = 0;
    uint32 count = 0;
    for(i=0;i<length;i++)
    {
        count += *data++;
    }
    return (uint16)(count / length);
}

// FunctionName: getVariance
//{
// Parameter:data:���ݻ��� length:���ݳ���
//
// Return:���ݻ����ڵķ���
//
// Description:
//  --
//
// Created: 2016/1/19
//
// Author:xiaoximi
//}
uint16 getVariance(uint16 *data,uint8 length)
{
    uint16 average = 0;
    uint32 sum = 0;
    uint8 i = 0;
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

// FunctionName: soted
//{
// Parameter:a:����������ݻ��� length:���ݳ���
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
void sorted(uint16 *a,uint16 length)
{
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

// FunctionName: pow
//{
// Parameter:data������������ type����������
//
// Return:������
//
// Description:
//  type=0.5 �����ţ�type=2 ƽ��
//
// Created: 2016/1/19
//
// Author:xiaoximi
//}
uint32 pow(int16 buf,uint8 type)
{
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
