/***********************************************************************************
    Filename: led.c

    Copyright 2007 Texas Instruments, Inc.
***********************************************************************************/
#include "led.h"
#include "common.h"



//----------------------------------------------------------------------------------
//  void halLedSet(uint8 id)
//
//  DESCRIPTION:
//    Turn LED on.
//----------------------------------------------------------------------------------
void halLedSet(uint8 id)
{
  P5DIR|=BIT1|BIT2|BIT3;  
#if LED_EN
  switch (id)
    {
        case 1: HAL_LED_SET_1; break;
        case 2: HAL_LED_SET_2; break;
        case 3: HAL_LED_SET_3; break;
        case 4: HAL_LED_SET_4; break;
        default: break;
    }
#endif
}
void halLedSetAll()
{
 #if LED_EN
    HAL_LED_SET_1; 
    HAL_LED_SET_2; 
    HAL_LED_SET_3; 
    HAL_LED_SET_4;
 #endif
}

//----------------------------------------------------------------------------------
//  void halLedClear(uint8 id)
//
//  DESCRIPTION:
//    Turn LED off.
//----------------------------------------------------------------------------------
void halLedClear(uint8 id)
{
#if LED_EN
    switch (id)
    {
        case 1: HAL_LED_CLR_1; break;
        case 2: HAL_LED_CLR_2; break;
        case 3: HAL_LED_CLR_3; break;
        case 4: HAL_LED_CLR_4; break;
        default: break;
    }
#endif
}

void halLedClearAll()
{
#if LED_EN
    HAL_LED_CLR_1;
    HAL_LED_CLR_2;
    HAL_LED_CLR_3;
    HAL_LED_CLR_4;
#endif
    
}
//----------------------------------------------------------------------------------
//  void halLedToggle(uint8 id)
//
//  DESCRIPTION:
//    Change state of LED. If on, turn it off. Else turn on.
//----------------------------------------------------------------------------------
void halLedToggle(uint8 id)
{
#if LED_EN
    switch (id)
    {
        case 1: HAL_LED_TGL_1; break;
        case 2: HAL_LED_TGL_2; break;
        case 3: HAL_LED_TGL_3; break;
        case 4: HAL_LED_TGL_4; break;
        default: break;
    }
#endif
}
void halLedToggleAll()
{
#if LED_EN
    HAL_LED_TGL_1; 
    HAL_LED_TGL_2;
    HAL_LED_TGL_3;
    HAL_LED_TGL_4;
#endif
    
}
void halLedFlow()
{
#if LED_EN
    halLedClearAll();
    HAL_LED_SET_1;
    delay_ms(100);
    HAL_LED_CLR_1;
    HAL_LED_SET_2;
    delay_ms(100);
    HAL_LED_CLR_2;
    HAL_LED_SET_3;
    delay_ms(100);
    HAL_LED_CLR_3;
    HAL_LED_SET_4;
    delay_ms(100);
    HAL_LED_CLR_4;
    HAL_LED_SET_1;
    delay_ms(100);
    HAL_LED_CLR_1;
    HAL_LED_SET_2;
    delay_ms(100);
    HAL_LED_CLR_2;
    HAL_LED_SET_3;
    delay_ms(100);
    HAL_LED_CLR_3;
    HAL_LED_SET_4;
    delay_ms(100);
    HAL_LED_CLR_4;
#endif
    
}



