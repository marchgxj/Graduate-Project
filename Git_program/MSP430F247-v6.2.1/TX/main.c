//******************
// FileName: main.c
//
// Description:
//  --
//
// Author: 2016/1/19, by xiaoximi
//

#include "common.h"

int main(void)
{	    
    __disable_interrupt();
    halBoardInit();
    A7139_SetPackLen(TEST_LENGTH);
    delay_us(1);
    A7139_SetFreq(492.001f);
    delay_us(1);
    A7139_Cal();                    //更改完频率后校准
    delay_us(1);
//    halLedClearAll();
//    delay_ms(1000);
//    halLedSet(1);
//    halLedClearAll();
//    A7139_DeepSleep();
//    delay_ms(2000);
//    halLedClear(1);
//    LPM3;
    __enable_interrupt();

    while(1) 
    {    

    }
    

}
