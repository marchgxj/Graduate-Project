#include "common.h"

uint8 normal_test[MAX_PACK_LENGTH];
uint8 test_rssi = 0;
uint16 address = 0;
#define    PHY_ADDRESS_WRITE          0x000E
int main(void)
{	    
    __disable_interrupt(); 
    halMcuInit();
    delay_s(1);
    GPIO_Init();
    FLASHInit();
    FLASH_Erase((uint16 *)PHY_ADDRESS_SEGMENT);
    FLASH_Writew((uint16 *)PHY_ADDRESS_SEGMENT,PHY_ADDRESS_WRITE);
//    FLASH_Erase((uint16 *)XMIDDLE_SEGMENT);
//    FLASH_Writew((uint16 *)XMIDDLE_SEGMENT,0x0000);
//    FLASH_Erase((uint16 *)YMIDDLE_SEGMENT);
//    FLASH_Writew((uint16 *)YMIDDLE_SEGMENT,0x0000);
//    FLASH_Erase((uint16 *)ZMIDDLE_SEGMENT);
//    FLASH_Writew((uint16 *)ZMIDDLE_SEGMENT,0x0000);
    address = GetPhyAddress();

    
    if(address == PHY_ADDRESS_WRITE)
    {
        halLedSetAll();
    }
    else
    {
        halLedClearAll();
    }
    for(;;)
    {
        
    }
    
}
