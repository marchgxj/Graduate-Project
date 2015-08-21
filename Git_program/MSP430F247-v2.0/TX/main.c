#include "common.h"

uint8 normal_test[MAX_PACK_LENGTH];
uint8 test_rssi = 0;
uint16 address = 0;
uint16 minus = 0;

int main(void)
{	    
    uint8 i=0;
    uint16 ADvalueX = 0;
    uint16 ADvalueY = 0;
    __disable_interrupt();
    halBoardInit();    
    A7139_SetPackLen(4);
    delay_us(1);
    A7139_SetFreq(470.001f);
    delay_us(1);
    A7139_Cal();                    //更改完频率后校准
    delay_us(1);

    for(i=0;i<MAX_PACK_LENGTH;i++)
    {
       normal_test[i]=i;
    }
    /****************send test**********************/  
    while(1) 
    {    
        
        //GetVariance();
        SampleChannel(&ADvalueX,&ADvalueY);
        VarianceX = abs(ADvalueX - AD_middle_valueX)*abs(ADvalueX - AD_middle_valueX);
        VarianceY = abs(ADvalueY - AD_middle_valueY)*abs(ADvalueY - AD_middle_valueY);
        VarianceM = abs(VarianceY-VarianceX/100);
        //SampleChannel(&ADvalueX,&ADvalueY);
        minus =  abs(VarianceY-VarianceX/100);
        normal_test[0] = minus>>8;
        normal_test[1] = minus;
        normal_test[2] = 30000>>8;
        normal_test[3] = 30000;
//        normal_test[2] = VarianceX>>8;
//        normal_test[3] = VarianceX;
        A7139_WriteFIFO(normal_test,4);
        delay_us(1);
        A7139_StrobeCmd(CMD_TX);
        delay_us(1);
        halLedToggle(1);
        while(GIO1)
        {}

        delay_ms(100);
    }
    /*************************************************/  

}
