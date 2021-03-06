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
void AD_Init()
{
    P6DIR &= 0xAE;   // 1010 1111
    P6SEL |= 0x51;   //0101 0000
    ADC12CTL0 &= ~ENC;
    ADC12CTL0 = SHT0_2+ADC12ON; 
    ADC12CTL1 = CONSEQ_0+ADC12SSEL_0+SHP+CSTARTADD_0; 
    delay_1ms();
}
/*******************************************
函数名称：SampleChannel(Uint16 ChannelNox)
功    能：msp430 自带ADC 单通道多次采样 
参    数：ChannelNox 采样通道号   
说明：    当通道为1时，开启定时器6S延时唤醒，如果是通道0,关闭定时器
返回值  ：采样通道AD值
********************************************/
int16 SampleChannel(Uint16* SampleValueX,Uint16* SampleValueY)	//进行采样通道电源等的设置
{     
    uint16 buf = 0;
    HAL_PLU_SET;
    delay_1ms();
    //Uint16 *ram_ptr;
    *SampleValueX =0;
    *SampleValueY =0;
    ADC12CTL0 &= ~ (ENC+ADC12SC);
    ADC12MCTL0 = INCH_4;
    ADC12CTL0 |= ENC + ADC12SC;
    while (ADC12CTL1 & ADC12BUSY!=0);                         //Wait if ADC10 core is active
    *SampleValueX=ADC12MEM0;
    ADC12CTL0 &= ~ (ENC+ADC12SC);
    ADC12MCTL0 = INCH_6;
    ADC12CTL0 |= ENC + ADC12SC;
    while (ADC12CTL1 & ADC12BUSY!=0);                         //Wait if ADC10 core is active
    *SampleValueY=ADC12MEM0;
    ADC12CTL0 &= ~ (ENC+ADC12SC);
    HAL_PLU_CLR;
    
    TIME1_HIGH;
    delay_1ms();
    TIME1_LOW;
    
    return (*SampleValueX-*SampleValueY);
}
int16 SampleVoltage(Uint16* Value,Uint16* Temp)
{     
    ADC12CTL0 &= ~ (ENC+ADC12SC);
    ADC12MCTL0 = INCH_0;
    ADC12CTL0 |= ENC + ADC12SC;
    while (ADC12CTL1 & ADC12BUSY!=0);                         //Wait if ADC10 core is active   
    *Value=ADC12MEM0;
    ADC12CTL0 &= ~ (ENC+ADC12SC);
    ADC12MCTL0 = INCH_10;
    ADC12CTL0 |= ENC + ADC12SC;
    while (ADC12CTL1 & ADC12BUSY!=0);                         //Wait if ADC10 core is active   
    *Temp=ADC12MEM0;
    ADC12CTL0 &= ~ (ENC+ADC12SC);
    
    return *Value;
}
#pragma vector=ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
   __bic_SR_register_on_exit(CPUOFF);         // Clear CPUOFF bit from 0(SR)  
}

void AD_cal()
{
    int i=0;
    uint16 ADX,ADY,ADZ,GMIADX,GMIADY;
    uint16 ADvalueX=0,ADvalueY=0,ADvalueZ = 0;
    uint16 GMI_ADvalueX=0,GMI_ADvalueY=0;
    uint32 intensity = 0;
    
    halLedSetAll();
    delay_ms(2000);
    
    ADX = 0;
    ADY = 0;
    for(i=0;i<26;i++)
    {
        if(i>=10)
        {
            SampleChannel(&GMI_ADvalueX,&GMI_ADvalueY);
            Multi_Read_HMC(&ADvalueX,&ADvalueY,&ADvalueZ);
            ADX += ADvalueX;
            ADY += ADvalueY;
            ADZ += ADvalueZ;
            GMIADX += GMI_ADvalueX;
            GMIADY += GMI_ADvalueY;
            delay_ms(50);
        }
    }
    MagneticUnit.XMiddle = ADX>>4;
    MagneticUnit.YMiddle = ADY>>4;
    MagneticUnit.ZMiddle = ADZ>>4;
    MagneticUnit.GMI_XMiddle = GMIADX>>4;
    MagneticUnit.GMI_YMiddle = GMIADY>>4;
    MagneticUnit.XMiddleM = MagneticUnit.XMiddle;
    MagneticUnit.YMiddleM = MagneticUnit.YMiddle;
    MagneticUnit.ZMiddleM = MagneticUnit.ZMiddle;
    MagneticUnit.GMI_XMiddleM = MagneticUnit.GMI_XMiddle;
    MagneticUnit.GMI_YMiddleM = MagneticUnit.GMI_YMiddle;
    MagneticUnit.Ext_Middle = abs(MagneticUnit.XMiddle-MagneticUnit.YMiddle);
    
    intensity = sqrt_16((((uint32)MagneticUnit.XMiddle*(uint32)MagneticUnit.XMiddle)+((uint32)MagneticUnit.YMiddle*(uint32)MagneticUnit.YMiddle)+((uint32)MagneticUnit.ZMiddle*(uint32)MagneticUnit.ZMiddle)));
    MagneticUnit.Int_Middle = intensity;
    
    for(int i=0;i<FILTER_LENGTH;i++)
    {
        SampleChannel(&ADvalueX,&ADvalueY);
        FilterData[i].xvalue = ADvalueX;
        FilterData[i].yvalue = ADvalueY;
        delay_ms(50);
    }
    for(int i=0;i<SLOP_LENGTH;i++)
    {
        SampleChannel(&ADvalueX,&ADvalueY);
        SlopData[i].xvalue = ADvalueX;
        SlopData[i].yvalue = ADvalueY;
        delay_ms(50);
    }
  

    //NoCarCalibration();
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

