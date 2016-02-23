/***********************************************************************************
    Filename: board.c

    Copyright 2007 Texas Instruments, Inc.
***********************************************************************************/
#include "common.h"

//------------------------------------------------------------------------------
//  Global variables
//------------------------------------------------------------------------------

// The constants below define some of the I/O signals used by the board
// Port, pin number, pin bitmask, direction and initial value should be
// set in order to match the target hardware. Once defined, the pins are
// configured in halBoardInit() by calling halDigioConfig()


//------------------------------------------------------------------------------
//  void halBoardInit(void)
//
//  DESCRIPTION:
//    Set up board. Initialize MCU, configure I/O pins and user interfaces
//------------------------------------------------------------------------------
uint16 rebootfalg = 0;
void halBoardInit(void)
{
    halMcuInit();
    delay_s(1);
    GPIO_Init();
    //halUartInit(0, 0); 
   // sched_init();
    //÷–∂œ≥ı ºªØ
    time_init();
    SPI3_Init();
    FLASHInit();
    AD_Init();
    Interrupt_Init();
    Init_TQ();
    Init_5983();
    AD_cal(); 
}
