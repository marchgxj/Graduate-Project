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

typedef struct
{
    uint16 value;
    uint16 num;
}DataStruct;

extern void CollectData();
extern void IdentifyCar();

extern uint8 Data_Change_Flag;
extern uint16 AD_middle_valueX;
extern uint16 AD_middle_valueY;
extern int16 AD_Value;
extern uint8 Car_Flag;
extern int16 Magnet_Value[COLLECT_WIDTH];
extern uint8  Start_Collect;
extern int Ave_Slop;
extern uint8 Car_Status;
#endif 
