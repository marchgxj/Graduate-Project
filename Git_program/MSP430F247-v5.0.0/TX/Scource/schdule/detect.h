#ifndef _DETECT_h_
#define _DETECT_h_
#include "common.h"

#define COLLECT_EN   0          //�Ƿ������ݲɼ�
#define COLLECT_PERIOD  500   //�ɼ�����  ��λ��ms
#define COLLECT_PERIOD_L  1    //�͹���ģʽ���ɼ�����  ��λ��1s
#define COLLECT_WIDTH   5       //�ɼ�����
#define SLOP_THRESHLOD  10      //б����ֵ

#define OFFSET          3      //���������
#define THRESHOLD       100
#define IDENTIFY_WIDTH  10

//���������״̬
#define NOCAR      0            //û��ͣ��
#define CAR        1            //ͣ��
#define NOCAR2CAR  2            //�޳����г��м�״̬
#define CAR2NOCAR  3            //�г����޳��м�״̬
#define COLLECTING 4            //�ɼ�������
#define STABLE     5            //�������ȶ�
#define OFFLINE    253            //������
#define LOWPOWER   252            //������

#define CAL_PERIOD  6000          //10����У׼һ��

#define VAR_THRESHOLD  50             //�����ж���ֵ
#define EXT_THRESHOLD  40               //�����ֵ�ж���ֵ
#define INT_THRESHOLD  60


#define TEST_LENGTH   48
#define FILTER_LENGTH 20
#define SLOP_LENGTH   3
#define INFRA_LENGTH  5

#define LOWPOWER_THRESHOLD_HIGH 600
#define LOWPOWER_THRESHOLD_LOW  500

#define OPEN_GMI_COUNT   400  //�೤ʱ������Ƿ���GMI  ��λ��50ms
#define CLOSE_GMI_COUNT  1200  //  ��λ��50ms

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
    uint16 YValue;
    uint16 YMiddle;
    uint16 YMiddleM;
    uint16 YMiddleMF;
    uint16 ZValue;
    uint16 ZMiddle;
    uint16 ZMiddleM;
    uint16 ZMiddleMF;
    
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
extern void GetIntensity();
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
extern uint8 Quick_CollectM;
extern uint16 infraredData[INFRA_LENGTH];
#endif 