//******************
// FileName: filter.c
//
// Description:
//  --
//
// Author: 2016/1/19, by xiaoximi
//

#include "common.h"

uint16 averageFilter(uint16 *buf,uint8 length)
{
    uint16 sum;                 //length<15 when data<4096
    for(int i=0;i<length;i++)
    {
        sum += *buf++;
    }
    return sum/length;
}

uint16 medianFilter(uint16 *data,uint8 length)
{
    uint16 buf[20];
    for(int i=0;i<length;i++)
    {
        buf[i]=*data++;
    }
    sorted(buf,length);
    return buf[length/2];
}