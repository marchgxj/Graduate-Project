#ifndef __ADC_H__
#define __ADC_H__
#include "common.h"

extern int16 SampleChannel(Uint16* SampleValueX,Uint16* SampleValueY);	//���в���ͨ����Դ�ȵ�����
extern void AD_cal();
extern int16 SampleVoltage(Uint16* Value,Uint16* Temp);
extern void AD_Init();






#endif