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
#if (GMI_EN == 0 && TEMP_TEST != 2)
    *SampleValueX =0;
    *SampleValueY =0;
    return 0;
#endif
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
    
//    TIME1_HIGH;
//    delay_1ms();
//    TIME1_LOW;
    
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
    uint32 ADX,ADY,ADZ;
    uint16 ADvalueX=0,ADvalueY=0,ADvalueZ = 0;
    uint32 intensity = 0;
#if (NET_TEST==1 || QOS_TEST==1)
    return;
#endif
    
    halLedSetAll();
    delay_ms(2000);
    getOffset();
   
    ADX = 0;
    ADY = 0;
    ADZ = 0;
    for(i=0;i<16;i++)
    {
        PNI_read_data(&ADvalueX,&ADvalueY,&ADvalueZ);
        ADX += ADvalueX;
        ADY += ADvalueY;
        ADZ += ADvalueZ;
        delay_ms(50);
    }
    MagneticUnit.XMiddle = ADX>>4;
    MagneticUnit.YMiddle = ADY>>4;
    MagneticUnit.ZMiddle = ADZ>>4;
    MagneticUnit.XValue_Stable = MagneticUnit.XMiddle;
    MagneticUnit.YValue_Stable = MagneticUnit.YMiddle;
    MagneticUnit.ZValue_Stable = MagneticUnit.ZMiddle;
    xcheck = MagneticUnit.XMiddle;
    ycheck = MagneticUnit.YMiddle;
    zcheck = MagneticUnit.ZMiddle;
    for(i=0;i<MIDDLE_QUENE_LENGTH;i++)
    {
        x_middle_quene[i] = MagneticUnit.XMiddle;
        y_middle_quene[i] = MagneticUnit.YMiddle;
        z_middle_quene[i] = MagneticUnit.ZMiddle;
    }
    
    
    MagneticUnit.Ext_Middle = abs(MagneticUnit.ZMiddle-MagneticUnit.XMiddle);
    
    intensity = sqrt_16((((uint32)MagneticUnit.XMiddle*(uint32)MagneticUnit.XMiddle)+((uint32)MagneticUnit.YMiddle*(uint32)MagneticUnit.YMiddle)+((uint32)MagneticUnit.ZMiddle*(uint32)MagneticUnit.ZMiddle)));
    MagneticUnit.Int_Middle = intensity;
    MagneticUnit.IntensityF = MagneticUnit.Int_Middle;
    MagneticUnit.ExtremumF = MagneticUnit.Ext_Middle;
    
    Ext_Threshold = EXT_THRESHOLD;
    Int_Threshold = INT_THRESHOLD;
    Var_Threshold = VAR_THRESHOLD;
    Dis_Threshold = DIS_THRESHOLD;
    
    
    for(int i=0;i<FILTER_LENGTH;i++)
    {
        SampleChannel(&ADvalueX,&ADvalueY);
        FilterData[i].xvalue = ADvalueX;
        FilterData[i].yvalue = ADvalueY;
        delay_ms(50);
    }
    for(int i=0;i<SLOP_LENGTH;i++)
    {
        PNI_read_data(&ADvalueX,&ADvalueY,&ADvalueZ);
        SlopData[i].xvalue = ADvalueX;
        SlopData[i].yvalue = ADvalueY;
        SlopData[i].zvalue = ADvalueZ;
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

uint16 sampleInfrared()
{
    uint16 value = 0;
    ADC12CTL0 &= ~ (ENC+ADC12SC);
    ADC12MCTL0 = INCH_1;
    ADC12CTL0 |= ENC + ADC12SC;
    while (ADC12CTL1 & ADC12BUSY!=0);                         //Wait if ADC10 core is active   
    value=ADC12MEM0;
    ADC12CTL0 &= ~ (ENC+ADC12SC);
    
    return value;
}
