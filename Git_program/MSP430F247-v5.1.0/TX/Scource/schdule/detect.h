#ifndef _DETECT_h_
#define _DETECT_h_
#include "common.h"

#define COLLECT_EN   0          //是否开启数据采集
#define COLLECT_PERIOD  500   //采集周期  单位：ms
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
#define OFFLINE    253            //电量低
#define LOWPOWER   252            //电量低

#define CAL_PERIOD  6000          //10分钟校准一次

#define VAR_THRESHOLD  150             //方差判断阈值
#define EXT_THRESHOLD  40               //两轴差值判断阈值
#define INT_THRESHOLD  60
#define COM_THRESHOLD  1500
#define DIS_THRESHOLD  30

#define TEST_LENGTH             56
#define FILTER_LENGTH           20
#define SLOP_LENGTH             3
#define INFRA_LENGTH            5
#define MIDDLE_QUENE_LENGTH     5
#define MAX_QUICK_COLLECT_TIME  6000  //5mins 在快采模式的最长时间 单位：50ms
#define HMC5983_RESET_PERIOD    12000 //10mins 5983复位周期 单位：50ms


#define LOWPOWER_THRESHOLD_HIGH 600
#define LOWPOWER_THRESHOLD_LOW  500

#define OPEN_GMI_COUNT   400  //多长时间后检测是否开启GMI  单位：50ms
#define CLOSE_GMI_COUNT  1200  //  单位：50ms

typedef struct
{
    uint16 value;
    uint16 num;
}DataStruct;

typedef struct
{
    uint16 XValue;
    uint16 XMiddle;
    uint16 XMiddleM;
    uint16 XMiddleMF;
    uint16 XValue_Stable;
    uint16 YValue;
    uint16 YMiddle;
    uint16 YMiddleM;
    uint16 YMiddleMF;
    uint16 YValue_Stable;
    uint16 ZValue;
    uint16 ZMiddle;
    uint16 ZMiddleM;
    uint16 ZMiddleMF;
    uint16 ZValue_Stable;
    
    uint16 GMI_XValue;
    uint16 GMI_XMiddle;
    uint16 GMI_XMiddleM;
    uint16 GMI_YValue;
    uint16 GMI_YMiddle;
    uint16 GMI_YMiddleM;
    
    uint16 Intensity;
    uint16 IntensityF;
    uint16 CarIntensity;
    uint16 Int_Middle;
    uint8  IntState;
    uint8  IntStateM;
    uint32 Variance;
    uint32 CarVariance;
    uint8  VarState;
    uint8  VarStateM;
    uint16 Extremum;
    uint16 ExtremumF;
    uint16 CarExtremum;
    uint16 Ext_Middle;
    uint8  ExtState;
    uint8  ExtStateM;
    int XAve_Slop;
    int YAve_Slop;
    int ZAve_Slop;
    uint16 infrared;
    uint16 distance;
    uint32 compatness;
    
}MagneticStruct;
extern MagneticStruct MagneticUnit;
typedef struct
{
    uint16 xvalue;
    uint16 yvalue;
    uint16 zvalue;
}FilterStruct;




extern void CollectData();
extern void IdentifyCar();
extern void NoCarCalibration();
extern uint8 MultiState(uint16 value,uint16 threshold);

extern uint8 CarCalibration();
extern void TotalJudge();
extern void CmdCalibration();


extern FilterStruct FilterData[FILTER_LENGTH];
extern uint16 Collect_Period;
extern FilterStruct SlopData[SLOP_LENGTH];
extern uint8 Quick_Collect;
extern uint8 HMC_Changed;
extern uint16 OpenGMI_Count;
extern uint8 XValue_Parking;
extern uint8 YValue_Parking;
extern uint16 Ext_Threshold;
extern uint16 Int_Threshold;
extern uint16 Var_Threshold;
extern uint16 Dis_Threshold;
extern uint8 Quick_CollectM;
extern uint16 infraredData[INFRA_LENGTH];
extern uint16 storage_count_send;
extern uint16 x_middle_quene[MIDDLE_QUENE_LENGTH];
extern uint16 y_middle_quene[MIDDLE_QUENE_LENGTH];
extern uint16 z_middle_quene[MIDDLE_QUENE_LENGTH];
extern uint8 middle_quene_count;
#endif 
