#ifndef _DETECT_h_
#define _DETECT_h_
#include "common.h"

#define COLLECT_EN   0          //�Ƿ������ݲɼ�
#define COLLECT_PERIOD  1000   //�ɼ�����  ��λ��100us
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

#define VAR_THRESHOLD  5000             //�����ж���ֵ
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
extern uint16 Cal_Time;          //У׼����
extern uint16 VarianceX;           //X�᷽��
extern uint16 VarianceY;           //Y�᷽��
extern uint16 VarianceAve;         //����ƽ������
extern uint16 VarianceM;           //���᷽���ֵ
extern uint8  State1_Count;        
extern uint8  State2_Count;
extern uint8  State3_Count;
extern uint8 Parking_State;
extern uint16 ExtremumValueMiddle;
#endif 
