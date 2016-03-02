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

#define VAR_THRESHOLD  150             //�����ж���ֵ
#define EXT_THRESHOLD  40               //�����ֵ�ж���ֵ
#define INT_THRESHOLD  60
#define COM_THRESHOLD  1000
#define DIS_THRESHOLD  30

#define TEST_LENGTH             56
#define FILTER_LENGTH           20
#define SLOP_LENGTH             3
#define INFRA_LENGTH            5
#define MIDDLE_QUENE_LENGTH     3
#define MAX_QUICK_COLLECT_TIME  2000  //5mins �ڿ��ģʽ���ʱ�� ��λ��50ms
#define HMC5983_RESET_PERIOD    12000 //10mins 5983��λ���� ��λ��50ms


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
    uint16 XMiddleMF;
    uint16 XValue_Stable;
    uint16 YValue;
    uint16 YMiddle;
    uint16 YMiddleMF;
    uint16 YValue_Stable;
    uint16 ZValue;
    uint16 ZMiddle;
    uint16 ZMiddleMF;
    uint16 ZValue_Stable;
    
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

extern uint8 vsEnvironment();
extern void TotalJudge();
extern void CmdCalibration();
extern void leaveIdentify();


extern FilterStruct FilterData[FILTER_LENGTH];
extern uint16 Collect_Period;
extern FilterStruct SlopData[SLOP_LENGTH];
extern uint8 Quick_Collect;
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
extern uint16 parking_time;
extern uint8 reverse_flag; 
#endif 