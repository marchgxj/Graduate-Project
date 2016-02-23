#include "common.h"

uint8 normal_test[MAX_PACK_LENGTH];
uint16 ADvalueX,ADvalueY,ADvalueZ;
uint16 test1,test2;
 int main(void)
{	    
    uint8 i=0;
    __disable_interrupt();
    
    //halBoardInit();
    halMcuInit();
    delay_s(1);
    GPIO_Init();
    //halUartInit(0, 0); 
   // sched_init();
    //÷–∂œ≥ı ºªØ
    time_init();
    SPI3_Init();
    FLASHInit();
    AD_Init();
    Interrupt_Init();
    Init_TQ();
    
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
    
    A7139_Init(470.001f);
    EndPointDevice.pyh_address = GetPhyAddress();
    EndPointDevice.device_type = DEVICE;
    
    halLedSetAll();
#if NET_TEST == 1 || QOS_TEST==1
    ChannelSelection(CHANNEL_1,CHANNEL_2);
#else
    ChannelSelection(CHANNEL_1,CHANNEL_2);
#endif
ReJoin:
    EndPointDevice.pyh_address = GetPhyAddress();
    EndPointDevice.device_type = DEVICE;
    ChannelSelectionOnce(RejoinChannel,RejoinChannel+1);
    TA0CCTL0 &= ~CCIFG;
    TBCCTL0 &= ~CCIFG;
    EN_INT;
    __enable_interrupt();

    for(;;)
    {
        Process_Event();
        if(ReJoinFlag == 1)
        {
            ReJoinHandler();
            ReJoinFlag = 0;
            __disable_interrupt();
            halLedFlow();
            goto ReJoin;
        }
    }
}
