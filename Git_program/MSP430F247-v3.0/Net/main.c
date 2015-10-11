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
    
    /*    ����������
    A7139_SetPackLen(TEST_LENGTH);
    delay_us(1);
    A7139_SetFreq(475.001f);
    delay_us(1);
    A7139_Cal();                    //������Ƶ�ʺ�У׼
    delay_us(1);
    while(1)
    {
        Quick_Collect =  1;
        IdentifyCar();
        TestSend();
        delay_ms(50);
    }*/
    /*    ���Ĳ���*/
    /*IRD_LOW;
    delay_ms(1000);
    halLedSet(1);
    halLedClearAll();
    A7139_DeepSleep();
    delay_ms(2000);
    halLedClear(1);
    LPM3;*/
    
    
    
    ChannelSelection(CHANNEL_4,CHANNEL_6);

    
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
