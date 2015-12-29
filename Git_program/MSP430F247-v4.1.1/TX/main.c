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
    halBoardInit();
    
    A7139_SetPackLen(TEST_LENGTH);
    delay_us(1);
    A7139_SetFreq(475.001f);
    delay_us(1);
    A7139_Cal();                    //更改完频率后校准
    delay_us(1);

    for(i=0;i<46;i++)
    {
       DataSendDraw[i]=i;
    }
    halLedClearAll();
    

    __enable_interrupt();
//    while(1)
//    {
//        SampleChannel(&XValue,&YValue);
//        delay_ms(20);
//        Direction_Count=700;
//        if((XValue>900)&&(XValue<1300)&&(YValue>900)&&(YValue)<1300&&(abs(XValue-YValue)<350)||(Direction_Count==700))
//        {
//            halLedSet(3);
//            delay_ms(2000);
//            Direction_Count=700;
//            NoCarCalibration();
//            halLedClear(3);
//            break;
//        }
//    }
    /****************send test**********************/ 

    while(1) 
    {    
    
//        SampleChannel(&MagneticUnit.XValue,&MagneticUnit.YValue); 
//        HAL_PLU_CLR;
//        delay_ms(50);
//        DataSendDraw[0] = MagneticUnit.XValue>>8;
//        DataSendDraw[1] = MagneticUnit.XValue;
//        DataSendDraw[2] = MagneticUnit.YValue>>8;
//        DataSendDraw[3] = MagneticUnit.YValue;
//        DataSendDraw[4] = MagneticUnit.Variance>>8;
//        DataSendDraw[5] = MagneticUnit.Variance;
//        DataSendDraw[6] = MagneticUnit.Ext_Middle>>8;
//        DataSendDraw[7] = MagneticUnit.Ext_Middle;
//        DataSendDraw[8] = MagneticUnit.Extremum>>8;
//        DataSendDraw[9] = MagneticUnit.Extremum;
//        DataSendDraw[10] = MagneticUnit.VarState;
//        DataSendDraw[11] = MagneticUnit.ExtState;
//        DataSendDraw[12] = MagneticUnit.XMiddle>>8;
//        DataSendDraw[13] = MagneticUnit.XMiddle;
//        DataSendDraw[14] = MagneticUnit.YMiddle>>8;
//        DataSendDraw[15] = MagneticUnit.YMiddle;
//        DataSendDraw[16] = MagneticUnit.Intensity>>8;
//        DataSendDraw[17] = MagneticUnit.Intensity;
//        DataSendDraw[18] = MagneticUnit.Int_Middle>>8;
//        DataSendDraw[19] = MagneticUnit.Int_Middle;
//        DataSendDraw[20] = MagneticUnit.IntState;
//        DataSendDraw[21] = EndPointDevice.parking_state;
//
//        A7139_WriteFIFO(DataSendDraw,TEST_LENGTH);
//        delay_us(1);
//        A7139_StrobeCmd(CMD_TX);
//        delay_us(1);
//        halLedToggle(1);
//        while(GIO1)
//        {}
    }
    /*************************************************/  

}
