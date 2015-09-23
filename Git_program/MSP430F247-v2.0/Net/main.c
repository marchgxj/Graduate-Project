#include "common.h"

uint8 normal_test[MAX_PACK_LENGTH];

int main(void)
{	    
    uint8 i=0;
    __disable_interrupt();
    
    halBoardInit(); 
ReJoin:
    EndPointDevice.pyh_address = GetPhyAddress();
    EndPointDevice.device_type = DEVICE;
    for(i=0;i<MAX_PACK_LENGTH;i++)
    {
       normal_test[i]=i;
    }
    ChannelSelection(CHANNEL_4,CHANNEL_6);
    halLedClearAll();
    
    __enable_interrupt();

    for(;;)
    {
        Process_Event();
        if(ReJoinFlag == 1)
        {
            ReJoinFlag = 0;
            goto ReJoin;
        }
    }
}
