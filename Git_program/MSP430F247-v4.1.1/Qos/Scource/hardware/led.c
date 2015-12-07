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
  switch (id)
    {
        case 1: HAL_LED_SET_1; break;
        case 2: HAL_LED_SET_2; break;
        case 3: HAL_LED_SET_3; break;
        case 4: HAL_LED_SET_4; break;
        default: break;
    }
}
void halLedSetAll()
{
    HAL_LED_SET_1; 
    HAL_LED_SET_2; 
    HAL_LED_SET_3; 
    HAL_LED_SET_4; 
}

//----------------------------------------------------------------------------------
//  void halLedClear(uint8 id)
//
//  DESCRIPTION:
//    Turn LED off.
//----------------------------------------------------------------------------------
void halLedClear(uint8 id)
{
    switch (id)
    {
        case 1: HAL_LED_CLR_1; break;
        case 2: HAL_LED_CLR_2; break;
        case 3: HAL_LED_CLR_3; break;
        case 4: HAL_LED_CLR_4; break;
        default: break;
    }
}

void halLedClearAll()
{
    HAL_LED_CLR_1;
    HAL_LED_CLR_2;
    HAL_LED_CLR_3;
    HAL_LED_CLR_4;
    
}
//----------------------------------------------------------------------------------
//  void halLedToggle(uint8 id)
//
//  DESCRIPTION:
//    Change state of LED. If on, turn it off. Else turn on.
//----------------------------------------------------------------------------------
void halLedToggle(uint8 id)
{
    switch (id)
    {
        case 1: HAL_LED_TGL_1; break;
        case 2: HAL_LED_TGL_2; break;
        case 3: HAL_LED_TGL_3; break;
        case 4: HAL_LED_TGL_4; break;
        default: break;
    }
}
void halLedToggleAll()
{
    HAL_LED_TGL_1; 
    HAL_LED_TGL_2;
    HAL_LED_TGL_3;
    HAL_LED_TGL_4;
    
}



