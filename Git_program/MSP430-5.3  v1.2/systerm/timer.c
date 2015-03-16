/***********************************************************************************
    Filename: hal_timer.h

    Copyright 2007 Texas Instruments, Inc.
***********************************************************************************/

#include "types.h"
#include "timer.h"
#include "board.h"
#include "led.h"
#include "mcu.h"

static uint16 mode;

//�жϳ�ʼ������������һ��
void time_init(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  
  halDigioConfig(&pinLed1);
    
  /*CCTL0 = CCIE;                         // CCR0 interrupt enabled
  CCR0 = 16386;//16386;                     // ������Ƕ��ٺ��룬32767Ϊ1s
  mode = TASSEL_1 + MC_1;
  TACTL = mode;              // ACLK, upmode
  CCTL0 &= ~CCIE;  */
  
  
  TA1CTL = TASSEL_2 + ID0 + TACLR ; //ACLK=32768Hz ,���tar ,2��Ƶ
  //TA1CCTL0 = CCIE; //
  TA1CCR0 = 795;//1ms   7956
  
  TA1CTL |= MC0; //������   

}
//----------------------------------------------------------------------------------
//  void halTimerRestart(void)
//
//  DESCRIPTION:
//    Restart timer. The timer is first stopped, then restarted, counting up from 0
//----------------------------------------------------------------------------------
void halTimerRestart(void)
{
     TACTL = MC_0;
     TACTL = mode;
     TAR = 0;
     CCTL0 = CCIE;
}
//----------------------------------------------------------------------------------
//  void halTimerIntEnable(void)
//----------------------------------------------------------------------------------
void halTimerIntEnable(void)
{
    CCTL0 = CCIE; 
}
//----------------------------------------------------------------------------------
//  void halTimerIntDisable(void)
//----------------------------------------------------------------------------------
void halTimerIntDisable(void)
{
    CCTL0 &= ~CCIE; 
}

int count = 0;
// Timer A0 interrupt service routine 0.5s



#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer (void)
{
  //halUartWrite("RRRRRRRRRR", 10);
}