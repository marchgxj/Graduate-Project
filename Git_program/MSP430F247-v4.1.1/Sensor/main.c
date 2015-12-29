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
    halLedSetAll();
    for(i=0;i<5;i++)
    {
        Init_5983();
    }
    halLedClearAll();
    
    //halUartInit(0, 0); 
   // sched_init();
    //ÖÐ¶Ï³õÊ¼»¯
//    time_init();
//    SPI3_Init();
//    FLASHInit();
//
//    AD_cal(); 

    while(1)
    {
        
    }
    /****************send test**********************/ 

    

}
