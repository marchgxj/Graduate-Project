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
   
    A7139_SetPackLen(TEST_LENGTH);
    delay_us(1);
    A7139_SetFreq(472.501f);
    delay_us(1);
    A7139_Cal();                    //更改完频率后校准
    delay_us(1);

    for(i=0;i<MAX_PACK_LENGTH;i++)
    {
       normal_test[i]=i;
    }
    __enable_interrupt();
    /****************send test**********************/ 

    while(1) 
    {    
        if(ADCal_Flag == 1)
        {
            Calibration();
            ADCal_Flag = 0;
        }
//        //GetVariance();
//        SampleChannel(&ADvalueX,&ADvalueY);
//        VarianceX = abs(ADvalueX - AD_middle_valueX)*abs(ADvalueX - AD_middle_valueX);
//        VarianceY = abs(ADvalueY - AD_middle_valueY)*abs(ADvalueY - AD_middle_valueY);
//        VarianceM = abs(VarianceY-VarianceX/100);
//        //GetExtremum();
//        minus = abs(ADvalueX - ADvalueY);
//        ExtremumValue = abs(minus - ExtremumValueMiddle);
//        Var_state = MultiState(VarianceM,VAR_THRESHOLD);
//        Ext_state = MultiState(ExtremumValue,EXT_THRESHOLD);
//        normal_test[0] = ADvalueX>>8;
//        normal_test[1] = ADvalueX;
//        normal_test[2] = ADvalueY>>8;
//        normal_test[3] = ADvalueY;
//        normal_test[4] = VarianceM>>8;
//        normal_test[5] = VarianceM;
//        normal_test[6] = ExtremumValueMiddle>>8;
//        normal_test[7] = ExtremumValueMiddle;
//        normal_test[8] = ExtremumValue>>8;
//        normal_test[9] = ExtremumValue;
//        normal_test[10] = Var_state;
//        normal_test[11] = Ext_state;
//        //        normal_test[2] = minus>>8;
//        //        normal_test[3] = minus;
//        //        normal_test[2] = VarianceX>>8;
//        //        normal_test[3] = VarianceX;
//        
//        
////        while(GetExtremum()!=1)
////        {
////            delay_ms(100);
////        }
////        normal_test[0] = ExtremumValue>>8;
////        normal_test[1] = ExtremumValue;
////        normal_test[2] = 100>>8;
////        normal_test[3] = 100;
//        A7139_WriteFIFO(normal_test,6);
//        delay_us(1);
//        A7139_StrobeCmd(CMD_TX);
//        delay_us(1);
//        halLedToggle(1);
//        while(GIO1)
//        {}
//        delay_ms(100);
    }
    /*************************************************/  

}
