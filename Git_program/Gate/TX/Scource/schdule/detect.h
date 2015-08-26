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

#define VAR_THRESHOLD  5000             //方差判断阈值
#define EXT_THRESHOLD  200

typedef struct
{
    uint16 value;
    uint16 num;
}DataStruct;

extern void CollectData();
extern void IdentifyCar();
extern void Calibration();
extern void GetVariance();
extern void GetExtremum();

extern uint8 Data_Change_Flag;
extern uint16 AD_middle_valueX;
extern uint16 AD_middle_valueY;
extern int16 AD_Value;
extern uint8 Car_Flag;
extern int16 Magnet_Value[COLLECT_WIDTH];
extern uint8  Start_Collect;
extern int Ave_Slop;
extern uint8 Car_Status;
extern uint16 Cal_Time;          //校准周期
extern uint16 VarianceX;           //X轴方差
extern uint16 VarianceY;           //Y轴方差
extern uint16 VarianceAve;         //两轴平均方差
extern uint16 VarianceM;           //两轴方差差值
extern uint8  State1_Count;        
extern uint8  State2_Count;
extern uint8  State3_Count;
extern uint8 Parking_State;
extern uint16 ExtremumValueMiddle;
#endif 
