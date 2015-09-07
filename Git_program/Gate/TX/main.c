#include "common.h"

uint8 normal_test[MAX_PACK_LENGTH];
uint8 test_rssi = 0;
uint16 address = 0;
uint16 minus = 0;
uint16 XValue = 0;
uint16 YValue = 0;
int main(void)
{	    
    __disable_interrupt();
    halBoardInit();   
    A7139_SetPackLen(MAX_PACK_LENGTH);
    delay_us(1);
    A7139_SetFreq(CHANNEL);
    delay_us(1);
    A7139_Cal();                    //更改完频率后校准
    delay_us(1);
    EndPointDevice.pyh_address = GetPhyAddress();
    A7139_DeepSleep();
    EndPointDevice.power = 1;
    while(1)
    {
        SampleChannel(&MagneticUnit.XValue,&MagneticUnit.YValue);
        if((MagneticUnit.XValue>500)&&(MagneticUnit.XValue<2500)&&
           (MagneticUnit.YValue>500)&&(MagneticUnit.YValue<2500))
        {
            halLedSet(2);
            delay_ms(2000);
            AD_cal();
            break;
        }
        delay_ms(50);
    }
    __enable_interrupt();
    /****************send test**********************/  
    while(1) 
    {    
        Process_Event();
//        CreatCSMASendData();
//        SendPack();
//        delay_ms(500);
//        halLedToggle(2);
  
    }
    /*************************************************/  

}
