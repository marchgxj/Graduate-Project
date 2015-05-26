#ifndef _DETECT_h_
#define _DETECT_h_
#include "common.h"

#define COLLECT_EN  1           //�Ƿ������ݲɼ�
#define COLLECT_PERIOD  1000   //�ɼ�����  ��λ��100us
#define COLLECT_WIDTH   20    //��30S���㣬ÿ��ɼ�10��
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
extern int AD_middle_value;
extern int16 AD_Value;
extern uint8 Car_Flag;
extern uint16 Magnet_Value[COLLECT_WIDTH];
extern uint8  Start_Collect;
#endif 
