#include "common.h"

uint8 normal_test[MAX_PACK_LENGTH];
uint8 test_rssi = 0;
uint16 address = 0;
uint16 minus = 0;

int main(void)
{	    
    uint8 i=0;
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
