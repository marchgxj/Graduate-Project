//******************
// FileName: criteria.c
//
// Description:
//  get some parameters for judge
//
// Author: 2016/1/19, by xiaoximi
//

#include "common.h"

uint16 diameterbuf = 0;
uint16 max_x = 0;
uint16 max_y = 0;
uint16 perimeterbuf = 0;

uint32 getAbsoluteValue()
{
    return (uint32)abs(MagneticUnit.ZValue_Stable - MagneticUnit.ZMiddle)
          *(uint32)abs(MagneticUnit.ZValue_Stable - MagneticUnit.ZMiddle);
}

//x、z轴极值
uint16 getExtremum()
{
    uint16 minus = 0;
    minus = abs(MagneticUnit.ZValue_Stable - MagneticUnit.XValue_Stable);
    return abs(minus - MagneticUnit.Ext_Middle);
}

//绝对磁场强度
uint16 getIntensity()
{
    uint32 intensity = 0;
    intensity = ((uint32)(MagneticUnit.XValue_Stable)*(uint32)(MagneticUnit.XValue_Stable))
        +((uint32)(MagneticUnit.YValue_Stable)*(uint32)(MagneticUnit.YValue_Stable))
        +((uint32)(MagneticUnit.ZValue_Stable)*(uint32)(MagneticUnit.ZValue_Stable));
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

uint16 getMaxDiameter(uint16 x1,uint16 y1,uint16 x2,uint16 y2)
{
    uint16 mx = 0;
    uint16 my = 0;
    mx = MAX(max_x,abs(x1 - x2));
    my = MAX(max_y,abs(y1 - y2));
    if((abs(mx-max_x)>700)||(abs(my-max_y)>500))
    {
        max_x = mx;
        max_y = my;
        diameterbuf = max_x + max_y;
    }
    return diameterbuf;
}

uint16 getPerimeter(uint16 x1,uint16 y1,uint16 x2,uint16 y2)
{
    uint16 perimeter = 0;
    
    perimeter = sqrt_16(pow(x1-x2,2)+pow(y1-y2,2));
    if (perimeter > 90)
    {
        perimeterbuf += perimeter;
    }
    return perimeterbuf;

}

uint32 getCompatness(uint16 x1,uint16 y1,uint16 x2,uint16 y2)
{
    
    uint16 perimeter = 0;
    uint16 diameter = 0;
    uint32 pow2 = 0; 
    perimeter = getPerimeter(x1,y1,x1,y2);
    diameter = getMaxDiameter(MagneticUnit.YMiddle,MagneticUnit.ZMiddle,x2,y2)+1;
    pow2 = pow(perimeter<<1,2);
    
    return pow2/diameter;
    //return pow(getPerimeter(x,y,length),2) / getMaxDiameter(x,y,length);
}