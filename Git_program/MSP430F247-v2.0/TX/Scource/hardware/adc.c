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
int16 SampleChannel(Uint16* SampleValueX,Uint16* SampleValueY)	//���в���ͨ����Դ�ȵ�����
{     
    P6DIR &= 0xaf;   // 1010 1111
    P6SEL |= 0x50;   //0101 0000
    HAL_PLU_SET;
    
    //Uint16 *ram_ptr;
    *SampleValueX =0;
    *SampleValueY =0;
    //����Ĳ���ֵ
    ADC12CTL0 &= ~ENC;                                 //�ڸı�����ǰֹͣA/Dת��
    //while (ADC12CTL1 & ADC12BUSY);                         //Wait if ADC10 core is active
    
    ADC12CTL0 = SHT0_2+ADC12ON+MSC;  //t_sample=16 x ADC10CLKs,�ο���ѹAVCC AVSS������ADCģ��
    //ADC10CTL0 = ADC10SHT_2+MSC+ADC10ON+MSC ;
    ADC12CTL1 = CONSEQ_1+ADC12SSEL_0+SHP+CSTARTADD_0; 
   // ADC12CTL1=SHP;
    //ADC12AE0 |= 0x04;                            //ʹ����Ӧ��ͨ��  
    ADC12MCTL0=INCH_4;
    //ADC12MCTL5=INCH_5;
    ADC12MCTL1=INCH_6+EOS;
    delay_1ms();
    ADC12CTL0 |= ENC + ADC12SC;
    delay_1ms();
    while (ADC12IFG & BIT0==0);                         //Wait if ADC10 core is active
    
    while (ADC12IFG & BIT1==0);                         //Wait if ADC10 core is active
    *SampleValueX=ADC12MEM0;
    *SampleValueY=ADC12MEM1;
    HAL_PLU_CLR;
    
    TIME1_HIGH;
    delay_1ms();
    TIME1_LOW;
    
    return (*SampleValueX-*SampleValueY);
}
#pragma vector=ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
   __bic_SR_register_on_exit(CPUOFF);         // Clear CPUOFF bit from 0(SR)  
}

void AD_cal()
{
    int i=0;
    uint16 ADvalueX=0,ADvalueY=0;
    uint32 intensity = 0;
    uint16 ADX,ADY;
    halLedSetAll();
    delay_ms(2000);
    
    ADX = 0;
    ADY = 0;
    for(i=0;i<16;i++)
    {
        SampleChannel(&ADvalueX,&ADvalueY);
        ADX += ADvalueX;
        ADY += ADvalueY;
        delay_ms(50);
    }
    MagneticUnit.XMiddle = ADX>>4;
    MagneticUnit.YMiddle = ADY>>4;
    MagneticUnit.XMiddleM = MagneticUnit.XMiddle;
    MagneticUnit.YMiddleM = MagneticUnit.YMiddle;
    MagneticUnit.Ext_Middle = abs(MagneticUnit.XMiddle-MagneticUnit.YMiddle);
    
    intensity = sqrt_16((((uint32)MagneticUnit.XMiddle*(uint32)MagneticUnit.XMiddle)+((uint32)MagneticUnit.YMiddle*(uint32)MagneticUnit.YMiddle)));
    MagneticUnit.Int_Middle = intensity;

    
    halLedClearAll();
    delay_ms(50);
    halLedSetAll();
    delay_ms(50);
    halLedClearAll();
    delay_ms(50);
    halLedSetAll();
    delay_ms(50);
    halLedClearAll();
    delay_ms(50);
}

