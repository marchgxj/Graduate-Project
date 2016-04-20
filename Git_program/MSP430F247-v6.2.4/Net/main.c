#include "common.h"

int main(void)
{	    
    __disable_interrupt();
    
    halBoardInit();
    A7139_Init(480.001f);
    EndPointDevice.pyh_address = GetPhyAddress();
    EndPointDevice.device_type = DEVICE;
    
   
    halLedSetAll();
#if NET_TEST == 1 || QOS_TEST==1
    ChannelSelection(CHANNEL_5,CHANNEL_6);
#else
    ChannelSelection(CHANNEL_13,CHANNEL_14);
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
