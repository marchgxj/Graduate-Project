/**
 * File      : adc.h
 * This file is part of attic fan control master
 * COPYRIGHT (C) 2013, Grand YQ
 *
 * brief: form systerm
 *
 * Change Logs:
 * Date           Author       Notes
 * 2013-04-27     minzhao       the first version
 */

#ifndef __ADC_H__
#define __ADC_H__
#include <types.h>
/********************��������**************************/
int16 SampleChannel(Uint16 ChannelNox);	//���в���ͨ����Դ�ȵ�����
static void   BubbbleData(Uint16 *pData,uint8 Count);
extern void   Delayus(Uint16 n);
extern void   Delayms(Uint16 n);
extern int ad_cal_value;
extern void AD_cal();
/********************��������**************************/

#endif