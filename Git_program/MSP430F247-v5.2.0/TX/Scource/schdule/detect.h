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
#define MIDDLE_QUENE_LENGTH     5
#define MAX_QUICK_COLLECT_TIME  2000  //5mins 在快采模式的最长时间 单位：50ms
#define HMC5983_RESET_PERIOD    12000 //10mins 5983复位周期 单位：50ms

#define    VAR_THRESHOLD       50             //方差判断阈值
#define    EXT_THRESHOLD       40               //两轴差值判断阈值
#define    INT_THRESHOLD       60
#define    DIS_THRESHOLD       30
#define    COM_THRESHOLD       1500

#define    COM_STABLE_COUNT    150
//控制命令
#define CMD_SEND_TEST        1
#define CMD_NOCAR            2
#define CMD_CAR              3
#define CMD_REBOOT           4

#define NORMAL_ENV_THRESHOLD    20
#define REVERSE_ENV_THRESHOLD   40

typedef struct
{
    uint16 value;
    uint16 num;
}DataStruct;

typedef struct
{
    uint16 XValue;
    uint16 XMiddle;
    uint16 XValue_Stable;
    uint16 XValue_parked_stable;
    uint16 YValue;
    uint16 YMiddle;
    uint16 YValue_Stable;
    uint16 YValue_parked_stable;
    uint16 ZValue;
    uint16 ZMiddle;
    uint16 ZValue_Stable;
    uint16 ZValue_parked_stable;
    uint16 Z_parked_distance;
    
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
    uint16 parked_distance;
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

extern uint8 vsEnvironment(uint8 threshold);
extern void TotalJudge();
extern void CmdCalibration();
extern void parkingStableSet();
extern void CmdHandler();


extern FilterStruct FilterData[FILTER_LENGTH];
extern uint16 Collect_Period;
extern FilterStruct SlopData[SLOP_LENGTH];
extern uint8 Quick_Collect;
extern uint8 Exit_Sleep;
extern uint16 Ext_Threshold;
extern uint16 Int_Threshold;
extern uint16 Var_Threshold;
extern uint16 Dis_Threshold;
extern uint8 Quick_CollectM;
extern uint16 storage_count_send;
extern uint16 x_middle_quene[MIDDLE_QUENE_LENGTH];
extern uint16 y_middle_quene[MIDDLE_QUENE_LENGTH];
extern uint16 z_middle_quene[MIDDLE_QUENE_LENGTH];
extern uint8 middle_quene_count;
extern uint16 parking_time;
extern uint8 parking_stable_flag; 
extern uint16 update_middle_times;
extern uint8 toggle_reason; 
extern uint16 toggle_distance_test;
extern uint32 compactness_latest;
extern uint16 xcheck;
extern uint16 ycheck;
extern uint16 zcheck;
#endif 
