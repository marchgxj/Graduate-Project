#include "common.h"


int main(void)
{	    
    __disable_interrupt();
    //halBoardInit();    
    halMcuInit();
    delay_s(1);
    GPIO_Init();
    halLedClear(1);
    halLedClear(2);
    halLedClear(3);
    halLedClear(4);

    for(;;)
    {
        halLedToggle(1);
        delay_ms(100);
        halLedToggle(2);
        delay_ms(100);
        halLedToggle(3);
        delay_ms(100);
        halLedToggle(4);
        delay_ms(100);
        
    }
    
}
