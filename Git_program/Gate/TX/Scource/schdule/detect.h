#ifndef _DETECT_h_
#define _DETECT_h_
#include "common.h"

#define COLLECT_EN   0          //是否开启数据采集
#define COLLECT_PERIOD  1000   //采集周期  单位：100us
#define COLLECT_PERIOD_L  1    //低功耗模式，采集周期  单位：1s
#define COLLECT_WIDTH   5       //采集个数
#define SLOP_THRESHLOD  10      //斜率阈值

#define OFFSET          3      //跳变检测距离
#define THRESHOLD       100
#define IDENTIFY_WIDTH  10

//传感器监测状态
#define NOCAR      0            //没有停车
#define CAR        1            //停车
#define NOCAR2CAR  2            //无车到有车中间状态
#define CAR2NOCAR  3            //有车到无车中间状态
#define COLLECTING 4            //采集数据中
#define STABLE     5            //传感器稳定

#define CAL_PERIOD  6000          //10分钟校准一次

#define VAR_THRESHOLD  200             //方差判断阈值
#define EXT_THRESHOLD  80               //两轴差值判断阈值
#define INT_THRESHOLD  150


#define TEST_LENGTH 46
#define FILTER_LENGTH 20
#define SLOP_LENGTH  5
typedef struct
{
    uint16 value;
    uint16 num;
}DataStruct;
typedef struct
{
    uint16 XValue;
    uint16 YValue;
    uint16 XMiddle;
    uint16 XMiddleM;
    uint16 YMiddle;
    uint16 YMiddleM;
    uint16 Intensity;
    uint16 CarIntensity;
    uint16 Int_Middle;
    uint8  IntState;
    uint32 Variance;
    uint32 CarVariance;
    uint8  VarState;
    uint16 Extremum;
    uint16 CarExtremum;
    uint16 Ext_Middle;
    uint8  ExtState;
    int XAve_Slop;
    int YAve_Slop;
    
}MagneticStruct;
extern MagneticStruct MagneticUnit;
typedef struct
{
    uint16 xvalue;
    uint16 yvalue;
}FilterStruct;

extern void CollectData();
extern void IdentifyCar();
extern void NoCarCalibration();
extern void GetVariance();
extern void GetExtremum();
extern uint8 MultiState(uint16 value,uint16 threshold);
extern unsigned int sqrt_16(unsigned long M);
extern void GetIntensity();
extern void CarCalibration();
extern void TotalJudge();
extern void VarianceMultiState();
extern void IntensityMultiState();
extern void ExtremumMultiState();
extern void GetSlop(uint16 xvalue,uint16 yvalue);

extern FilterStruct FilterData[FILTER_LENGTH];
extern uint8 VState1_Count;
extern uint8 VState2_Count;
extern uint8 VState3_Count;
extern uint8 EState1_Count;
extern uint8 EState2_Count;
extern uint8 EState3_Count;
extern uint8 IState1_Count;
extern uint8 IState2_Count;
extern uint8 IState3_Count;
extern uint8 CarCaliFlag;
extern uint8 Parking_State;
#endif 
