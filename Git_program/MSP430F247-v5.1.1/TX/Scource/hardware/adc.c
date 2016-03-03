#include "adc.h"
#include "common.h"

void AD_Init()
{
    P6DIR &= 0xAE;   // 1010 1111
    P6SEL |= 0x51;   //0101 0000
//    
    //保存的采样值
//    ADC12CTL0 &= ~ENC;                                 //在改变设置前停止A/D转换
//    ADC12CTL0 = SHT0_2+ADC12ON+MSC; 
//  
//    ADC12CTL1 = CONSEQ_1+ADC12SSEL_0+SHP+CSTARTADD_0; 
//    ADC12MCTL0=INCH_0 + INCH_4;
//    ADC12MCTL1=INCH_6+EOS;
    //保存的采样值
    ADC12CTL0 &= ~ENC;
    ADC12CTL0 = SHT0_2+ADC12ON; 

    ADC12CTL1 = CONSEQ_0+ADC12SSEL_0+SHP+CSTARTADD_0; 
//    ADC12MCTL0 = INCH_0;
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
//    P6DIR &= 0xaf;   // 1010 1111
//    P6SEL |= 0x50;   //0101 0000
    uint16 buf = 0;
    HAL_PLU_SET;
    delay_1ms();
    //Uint16 *ram_ptr;
    *SampleValueX =0;
    *SampleValueY =0;
//    //保存的采样值
//    ADC12CTL0 &= ~ENC;                                 //在改变设置前停止A/D转换
//    //while (ADC12CTL1 & ADC12BUSY);                         //Wait if ADC10 core is active
//    
//    ADC12CTL0 = SHT0_2+ADC12ON+MSC;  //t_sample=16 x ADC10CLKs,参考电压AVCC AVSS，开启ADC模块
//    //ADC10CTL0 = ADC10SHT_2+MSC+ADC10ON+MSC ;
//    ADC12CTL1 = CONSEQ_1+ADC12SSEL_0+SHP+CSTARTADD_0; 
//   // ADC12CTL1=SHP;
//    //ADC12AE0 |= 0x04;                            //使能相应的通道  
//    ADC12MCTL0=INCH_4;
//    //ADC12MCTL5=INCH_5;
//    ADC12MCTL1=INCH_6+EOS;
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
#pragma vector=ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
   __bic_SR_register_on_exit(CPUOFF);         // Clear CPUOFF bit from 0(SR)  
}
int16 SampleVoltage(Uint16* Value,Uint16* Temp)
{     
//    P6DIR &= 0xFE;   // 1010 1111
//    P6SEL |= 0x01;   //0101 0000
//    ADC12CTL0 &= ~ENC;
//    ADC12CTL0 = SHT0_2+ADC12ON; 
//
//    ADC12CTL1 = CONSEQ_0+ADC12SSEL_0+SHP+CSTARTADD_0; 
//    ADC12MCTL0 = INCH_0+EOS;
//    delay_1ms();
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
void AD_cal()
{
    int i=0;
    uint16 ADX,ADY,ADZ;
    uint16 ADvalueX=0,ADvalueY=0,ADvalueZ = 0;
    
    uint32 intensity = 0;
    
    halLedSetAll();
    delay_ms(2000);
    
    for(i=0;i<INFRA_LENGTH;i++)
    {
        infraredData[i] = sampleInfrared();
    }
    
    ADX = 0;
    ADY = 0;
    ADZ = 0;
    for(i=0;i<26;i++)
    {
        if(i>=10)
        {
            Multi_Read_HMC(&ADvalueX,&ADvalueY,&ADvalueZ);
            //Single_Read_HMC(&ADvalueX,&ADvalueY,&ADvalueZ);
            ADX += ADvalueX;
            ADY += ADvalueY;
            ADZ += ADvalueZ;
            delay_ms(50);
        }
    }
    MagneticUnit.XMiddle = ADX>>4;
    MagneticUnit.YMiddle = ADY>>4;
    MagneticUnit.ZMiddle = ADZ>>4;
    MagneticUnit.XMiddleMF = MagneticUnit.XMiddle;
    MagneticUnit.YMiddleMF = MagneticUnit.YMiddle;
    MagneticUnit.ZMiddleMF = MagneticUnit.ZMiddle;
    MagneticUnit.XValue_Stable = MagneticUnit.XMiddle;
    MagneticUnit.YValue_Stable = MagneticUnit.YMiddle;
    MagneticUnit.ZValue_Stable = MagneticUnit.ZMiddle;
    for(i=0;i<MIDDLE_QUENE_LENGTH;i++)
    {
        x_middle_quene[i] = MagneticUnit.XMiddle;
        y_middle_quene[i] = MagneticUnit.YMiddle;
        z_middle_quene[i] = MagneticUnit.ZMiddle;
    }
    
    
    MagneticUnit.Ext_Middle = abs(MagneticUnit.ZMiddle-MagneticUnit.YMiddle);
    
    intensity = sqrt_16((((uint32)MagneticUnit.XMiddle*(uint32)MagneticUnit.XMiddle)+((uint32)MagneticUnit.YMiddle*(uint32)MagneticUnit.YMiddle)+((uint32)MagneticUnit.ZMiddle*(uint32)MagneticUnit.ZMiddle)));
    MagneticUnit.Int_Middle = intensity;
    MagneticUnit.IntensityF = MagneticUnit.Int_Middle;
    MagneticUnit.ExtremumF = MagneticUnit.Ext_Middle;
    
    Ext_Threshold = EXT_THRESHOLD;
    Int_Threshold = INT_THRESHOLD;
    Var_Threshold = VAR_THRESHOLD;
    Dis_Threshold = DIS_THRESHOLD;
    
    //storage_x[0] = MagneticUnit.XMiddle;
    //storage_y[0] = MagneticUnit.YMiddle;
    //storage_z[0] = MagneticUnit.ZMiddle;
    
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
