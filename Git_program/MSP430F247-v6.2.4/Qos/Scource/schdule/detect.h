#ifndef _DETECT_h_
#define _DETECT_h_
#include "common.h"
#define MCU_SLEEP_ENABLE 1

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
#define OFFLINE    253            //离线
#define LOWPOWER   252            //电量低
#define TEMPOVER   251            //温度异常

#define CAL_PERIOD  6000          //10分钟校准一次

#define VAR_THRESHOLD  50             //方差判断阈值
#define EXT_THRESHOLD  40               //两轴差值判断阈值
#define INT_THRESHOLD  60

#define TEST_LENGTH 46
#define FILTER_LENGTH 20
#define SLOP_LENGTH 3


#define LOWPOWER_THRESHOLD 600

#define TEMP_THRESHOLD_HIGH     3500
#define TEMP_THRESHOLD_LOW      1000

#define OPEN_GMI_COUNT   400  //多长时间后检测是否开启GMI  单位：50ms
#define CLOSE_GMI_COUNT  1200  //  单位：50ms

//控制命令
#define SEND_TEST        1
#define RECAL            2


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
    uint16 YValue;
    uint16 YMiddle;
    uint16 YMiddleM;
    uint16 ZValue;
    uint16 ZMiddle;
    uint16 ZMiddleM;
    
    
    uint16 GMI_XValue;
    uint16 GMI_XMiddle;
    uint16 GMI_XMiddleM;
    uint16 GMI_YValue;
    uint16 GMI_YMiddle;
    uint16 GMI_YMiddleM;
    
    uint16 Intensity;
    uint16 CarIntensity;
    uint16 Int_Middle;
    uint8  IntState;
    uint8  IntStateM;
    uint32 Variance;
    uint32 CarVariance;
    uint8  VarState;
    uint8  VarStateM;
    uint16 Extremum;
    uint16 CarExtremum;
    uint16 Ext_Middle;
    uint8  ExtState;
    uint8  ExtStateM;
    int XAve_Slop;
    int YAve_Slop;
    int ZAve_Slop;
    
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
extern void GetVariance();
extern void GetExtremum();
extern uint8 MultiState(uint16 value,uint16 threshold);
extern unsigned int sqrt_16(unsigned long M);
extern void GetIntensity();
extern void CarCalibration();
extern void TotalJudge();
extern void CmdHandler();

extern FilterStruct FilterData[FILTER_LENGTH];

extern uint16 Collect_Period;
extern FilterStruct SlopData[SLOP_LENGTH];
extern uint8 Quick_Collect;
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
extern uint8 HMC_Changed;
extern uint16 OpenGMI_Count;
extern uint8 Exit_Sleep;
extern uint8 XValue_Parking;
extern uint8 YValue_Parking;
#endif 
