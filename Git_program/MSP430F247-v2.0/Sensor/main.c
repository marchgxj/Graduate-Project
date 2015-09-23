#include "common.h"

uint8 normal_test[MAX_PACK_LENGTH];
uint8 test_rssi = 0;
uint16 address = 0;
uint16 minus = 0;
uint16 inten = 0;
uint16 XValue = 0;
uint16 YValue = 0;
int main(void)
{	    
    uint8 i=0;
    __disable_interrupt();
    halMcuInit();
    delay_s(1);
    GPIO_Init();
    //halUartInit(0, 0); 
   // sched_init();
    //中断初始化
    time_init();
    SPI3_Init();
    FLASHInit();

    AD_cal(); 

    while(1)
    {
        //SampleChannel(&XValue,&YValue);
        P6DIR &= 0xaf;   // 1010 1111
        P6SEL |= 0x50;   //0101 0000
        HAL_PLU_SET;
        
        //Uint16 *ram_ptr;
        XValue =0;
        YValue =0;
        //保存的采样值
        ADC12CTL0 &= ~ENC;                                 //在改变设置前停止A/D转换
        //while (ADC12CTL1 & ADC12BUSY);                         //Wait if ADC10 core is active
        
        ADC12CTL0 = SHT0_2+ADC12ON+MSC;  //t_sample=16 x ADC10CLKs,参考电压AVCC AVSS，开启ADC模块
        //ADC10CTL0 = ADC10SHT_2+MSC+ADC10ON+MSC ;
        ADC12CTL1 = CONSEQ_1+ADC12SSEL_0+SHP+CSTARTADD_0; 
       // ADC12CTL1=SHP;
        //ADC12AE0 |= 0x04;                            //使能相应的通道  
        ADC12MCTL0=INCH_4;
        //ADC12MCTL5=INCH_5;
        ADC12MCTL1=INCH_6+EOS;
        delay_1ms();
        delay_1ms();
        ADC12CTL0 |= ENC + ADC12SC;
        delay_1ms();
        delay_1ms();
        while (ADC12IFG & BIT0==0);                         //Wait if ADC10 core is active
        
        while (ADC12IFG & BIT1==0);                         //Wait if ADC10 core is active
        XValue=ADC12MEM0;
        YValue=ADC12MEM1;
        //HAL_PLU_CLR;

        delay_ms(50);
        halLedToggle(1);
    }
    /****************send test**********************/ 

    

}
