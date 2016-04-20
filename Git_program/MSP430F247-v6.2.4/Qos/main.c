#include "common.h"

uint8 normal_test[MAX_PACK_LENGTH];

int main(void)
{	    
    __disable_interrupt();
    
    halBoardInit();
    halLedClearAll();

    __enable_interrupt();
    for(;;)
    {
        Process_Event();
    }
}
