#include "common.h"

uint8 normal_test[MAX_PACK_LENGTH];
uint8 test_rssi = 0;
uint16 address = 0;
uint16 minus = 0;
uint16 XValue = 0;
uint16 YValue = 0;
uint16 Tim_Count = 0;
int main(void)
{	    
    __disable_interrupt();
    halBoardInit();   
    A7139_SetPackLen(TEST_LENGTH);
    delay_us(1);
    A7139_SetFreq(CHANNEL_SEE);
    delay_us(1);
    A7139_Cal();                    //更改完频率后校准
    delay_us(1);
    EndPointDevice.pyh_address = GetPhyAddress();
    EndPointDevice.power = 1;
//    while(1)
//    {
////        Tim_Count++;
//        SampleChannel(&MagneticUnit.XValue,&MagneticUnit.YValue); 
//        GetSlop(MagneticUnit.XValue,MagneticUnit.YValue);
//        GetVariance();
//        GetExtremum();
//        GetIntensity();
//        VarianceMultiState();
//        ExtremumMultiState();
//        IntensityMultiState();
//        TotalJudge();
//        TestSend();
//        delay_ms(50);
//        halLedToggle(2);
//        if(Tim_Count>200)
//        {
//            halLedSet(2);
//            delay_ms(2000);
//            AD_cal();
//            break;
//        }
//    }
//    A7139_SetFreq(CHANNEL);
//    delay_us(1);
//    A7139_SetPackLen(MAX_PACK_LENGTH);
//    delay_us(1);
//    A7139_DeepSleep();
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
