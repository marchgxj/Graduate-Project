//******************
// FileName: criteria.c
//
// Description:
//  get some parameters for judge
//
// Author: 2016/1/19, by xiaoximi
//

#include "common.h"
//z轴绝对值

uint32 getAbsoluteValue()
{
    return (uint32)abs(MagneticUnit.ZValue - MagneticUnit.ZMiddle)
          *(uint32)abs(MagneticUnit.ZValue - MagneticUnit.ZMiddle);
}

//y、z轴极值
uint16 getExtremum()
{
    uint16 minus = 0;
    minus = abs(MagneticUnit.ZValue - MagneticUnit.YValue);
    return abs(minus - MagneticUnit.Ext_Middle);
}

//绝对磁场强度
uint16 getIntensity()
{
    uint32 intensity = 0;
    intensity = ((uint32)(MagneticUnit.XValue)*(uint32)(MagneticUnit.XValue))
        +((uint32)(MagneticUnit.YValue)*(uint32)(MagneticUnit.YValue))
        +((uint32)(MagneticUnit.ZValue)*(uint32)(MagneticUnit.ZValue));
    return sqrt_16(intensity);
}

//欧氏距离
uint16 getEuclideanMetric(uint16 x1,uint16 y1,uint16 z1,uint16 x2,uint16 y2,uint16 z2)
{
    uint32 distance = 0;
    distance = pow(abs(x1 - x2),2)
             + pow(abs(y1 - y2),2)
             + pow(abs(z1 - z2),2);
    return sqrt_16(distance);
                 
}

//最大直径
uint16 getMaxDiameter(uint16 *x,uint16 *y,uint8 length)
{
    uint16 startx = 0;
    uint16 starty = 0;
    uint16 finaldiameter = 0;
    if(length<2)
        return 0;
    startx = x[0];
    starty = y[0];
    for(int i=0;i<length-1;i++)
    {
        finaldiameter = MAX(finaldiameter,
                            (sqrt_16(pow(startx - x[i],2) + pow(starty - y[i],2))));
    }
    
    return finaldiameter;
}

//周长
uint16 getPerimeter(uint16 *x,uint16 *y,uint8 length)
{
    uint16 finalperimeter = 0;
    uint16 perimeterbuf = 0;
    if(length<2)
        return 0;
    for(int i=0;i<length-1;i++)
    {
        perimeterbuf = sqrt_16(pow(x[i]-x[i+1],2)+pow(y[i]-y[i+1],2));
        if (perimeterbuf > 15)
        {
            finalperimeter += perimeterbuf;
        }
    }
    return finalperimeter;
}
    
uint32 getCompatness(uint16 *x,uint16 *y,uint8 length)
{
    uint16 perimeter = 0;
    uint16 diameter = 0;
    uint32 pow2 = 0;
    if(length<2)
        return 0;
    perimeter = getPerimeter(x,y,length);
    diameter = getMaxDiameter(x,y,length)+1;
    pow2 = pow(perimeter,2);
    
    return pow2/diameter;
    //return pow(getPerimeter(x,y,length),2) / getMaxDiameter(x,y,length);
}