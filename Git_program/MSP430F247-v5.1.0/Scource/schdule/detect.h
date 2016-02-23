#ifndef _DETECT_h_
#define _DETECT_h_
#include "common.h"

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

#define TEST_LENGTH             56
#define FILTER_LENGTH           20
#define SLOP_LENGTH             3
#define INFRA_LENGTH            5
#define MIDDLE_QUENE_LENGTH     5
#define MAX_QUICK_COLLECT_TIME  6000  //5mins 在快采模式的最长时间 单位：50ms
#define HMC5983_RESET_PERIOD    12000 //10mins 5983复位周期 单位：50ms



#define OPEN_GMI_COUNT   400  //多长时间后检测是否开启GMI  单位：50ms
#define CLOSE_GMI_COUNT  1200  //  单位：50ms

//控制命令
#define CMD_SEND_TEST        1
#define CMD_NOCAR            2
#define CMD_CAR              3


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
extern void CmdHandler();


extern FilterStruct FilterData[FILTER_LENGTH];
extern uint16 Collect_Period;
extern FilterStruct SlopData[SLOP_LENGTH];
extern uint8 Quick_Collect;
extern uint8 HMC_Changed;
extern uint16 OpenGMI_Count;
extern uint8 Exit_Sleep;
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
