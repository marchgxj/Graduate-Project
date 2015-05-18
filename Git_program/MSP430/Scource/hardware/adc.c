/**
 * File      : adc.c
 * This file is part of attic fan control master
 * COPYRIGHT (C) 2013, Grand YQ
 *
 * brief: form systerm
 *
 * Change Logs:
 * Date           Author       Notes
 * 2013-04-27     minzhao       the first version
 */
#include "adc.h"
#include "common.h"

/*******************************************
�������ƣ�SampleChannel(Uint16 ChannelNox)
��    �ܣ�msp430 �Դ�ADC ��ͨ����β��� 
��    ����ChannelNox ����ͨ����   
˵����    ��ͨ��Ϊ1ʱ��������ʱ��6S��ʱ���ѣ������ͨ��0,�رն�ʱ��
����ֵ  ������ͨ��ADֵ
********************************************/
int16 SampleChannel(Uint16 ChannelNox)	//���в���ͨ����Դ�ȵ�����
{     
    P2DIR |= 0xfb;   

    HAL_PLU_SET;
    //Uint16 *ram_ptr;
    int16 SampleValue =0;                             //����Ĳ���ֵ
    ADC10CTL0 &= ~ENC;                                 //�ڸı�����ǰֹͣA/Dת��
    while (ADC10CTL1 & ADC10BUSY);                         //Wait if ADC10 core is active
    ADC10CTL0 = ADC10SHT_2+ADC10ON+ADC10IE;  //t_sample=16 x ADC10CLKs,�ο���ѹAVCC AVSS������ADCģ��
    //ADC10CTL0 = ADC10SHT_2+MSC+ADC10ON+MSC ;
    ADC10CTL1 = CONSEQ_0+ADC10SSEL_0+INCH_2; 
    ADC10AE0 |= 0x04;                            //ʹ����Ӧ��ͨ��  
    
    delay_1ms();
    ADC10CTL0 |= ENC + ADC10SC;
    delay_1ms();
    SampleValue=ADC10MEM; 
    delay_1ms();
    HAL_PLU_CLR;
    
    return (SampleValue);
}
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
   __bic_SR_register_on_exit(CPUOFF);         // Clear CPUOFF bit from 0(SR)  
}

void AD_cal()
{
    int i=0;
    for(i=0;i<16;i++)
    {
          AD_middle_value += SampleChannel(0x02);
    
    }
    AD_middle_value = AD_middle_value>>4;
    halLedSet(1);
    delay_ms(1000);
    halLedClear(1);
    delay_ms(50);
    halLedSet(1);
    delay_ms(50);
    halLedClear(1);
    delay_ms(50);
    halLedSet(1);
    delay_ms(50);
    halLedClear(1);
    delay_ms(50);
}

