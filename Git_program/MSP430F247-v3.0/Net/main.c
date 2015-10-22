#include "common.h"

uint8 normal_test[MAX_PACK_LENGTH];
uint16 ADvalueX,ADvalueY,ADvalueZ;

int main(void)
{	    
    uint8 i=0;
    __disable_interrupt();
    
    halBoardInit();
    
ReJoin:
    A7139_Init(470.001f);
    EndPointDevice.pyh_address = GetPhyAddress();
    EndPointDevice.device_type = DEVICE;
    for(i=0;i<MAX_PACK_LENGTH;i++)
    {
       normal_test[i]=i;
    }

    //    传感器测试
//    A7139_SetPackLen(TEST_LENGTH);
//    delay_us(1);
//    A7139_SetFreq(475.001f);
//    delay_us(1);
//    A7139_Cal();                    //更改完频率后校准
//    delay_us(1);
//    while(1)
//    {
//        Quick_Collect =  1;
//        IdentifyCar();
//        TestSend();
//        delay_ms(50);
//    }
    /*    功耗测试*/
    /*IRD_HIGH;
    Multi_Read_HMC(&ADvalueX,&ADvalueY,&ADvalueZ);
    delay_ms(1000);
    halLedSet(1);
    halLedClearAll();
    A7139_DeepSleep();
    delay_ms(2000);
    halLedClear(1);
    LPM3;*/

    halLedSetAll();
    ChannelSelection(CHANNEL_4,CHANNEL_6);

    __enable_interrupt();

    for(;;)
    {
        Process_Event();
        if(ReJoinFlag == 1)
        {
            ReJoinFlag = 0;
            __disable_interrupt();
            goto ReJoin;
        }
    }
}
