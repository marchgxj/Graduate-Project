#include "common.h"

uint16 x,y,z;
int main(void)
{	    
    __disable_interrupt();
    halMcuInit();
    delay_s(1);
    GPIO_Init();
    //halUartInit(0, 0); 
   // sched_init();
    //÷–∂œ≥ı ºªØ
    SPI3_Init();
    Init_PNI();
    //AD_cal(); 
    getOffset();
    halLedClearAll();
    while(1)
    {
        PNI_read_data(&x,&y,&z);
        if(x==10000||y==10000||z==10000)
        {
            halLedClear(1);
        }
        else
        {
            halLedSet(1);
        }
    }
    /****************send test**********************/ 

    

}
