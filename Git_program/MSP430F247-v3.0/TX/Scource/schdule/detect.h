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

#define CAL_PERIOD  6000          //10����У׼һ��

#define VAR_THRESHOLD  200             //�����ж���ֵ
#define EXT_THRESHOLD  80               //�����ֵ�ж���ֵ
#define INT_THRESHOLD  150

#define TEST_LENGTH 38
#define FILTER_LENGTH 20
#define SLOP_LENGTH 3

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
    uint32 Variance;
    uint32 CarVariance;
    uint8  VarState;
    uint16 Extremum;
    uint16 CarExtremum;
    uint16 Ext_Middle;
    uint8  ExtState;
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

extern FilterStruct FilterData[FILTER_LENGTH];
extern uint16 Collect_Period;
extern FilterStruct SlopData[SLOP_LENGTH];
extern uint8 Quick_Collect;
#endif 
