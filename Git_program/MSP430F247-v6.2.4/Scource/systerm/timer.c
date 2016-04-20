/***********************************************************************************
    Filename: hal_timer.h

    Copyright 2007 Texas Instruments, Inc.
***********************************************************************************/

#include "types.h"
#include "timer.h"
#include "board.h"
#include "led.h"
#include "mcu.h"



//中断初始化，现在用了一个
void time_init(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  
  //use 16M as MCLK
  TBCTL = TBSSEL_2 + MC_1 +ID0;
 // TA1CTL = TASSEL_2 + ID0 + TACLR + MC0; //ACLK=32768Hz ,清除tar ,2分频

  TBCCR0 = 795;//100us
  //TBCCTL0 = CCIE;

  //TimerA used for 1s, use 
  TA0CTL = TASSEL_1 + MC_1; 
  TA0CCR0 = 1650;
  TA0CCTL0 = CCIE; 
  
  
  
  

}
//----------------------------------------------------------------------------------
//  void halTimerRestart(void)
//
//  DESCRIPTION:
//    Restart timer. The timer is first stopped, then restarted, counting up from 0
//----------------------------------------------------------------------------------
void halTimerRestart(void)
{
    TBCTL = TBSSEL_2 + MC_1 +ID0;
    TAR = 0;
    TBCCTL0 = CCIE;
}
//----------------------------------------------------------------------------------
//  void halTimerIntEnable(void)
//----------------------------------------------------------------------------------
void halTimerIntEnable(void)
{
   TBCCTL0 = CCIE;
}
//----------------------------------------------------------------------------------
//  void halTimerIntDisable(void)
//----------------------------------------------------------------------------------
void halTimerIntDisable(void)
{
   TBCCTL0 &= ~CCIE; 
}

int count = 0;
// Timer A0 interrupt service routine 0.5s



#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer (void)
{
  //halUartWrite("RRRRRRRRRR", 10);
}