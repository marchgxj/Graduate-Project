#ifndef _INTERRUPT_h_
#define _INTERRUPT_h_
#include "common.h"
#include "detect.h"

#define   EN_TIMER1    {TBCCTL0 = CCIE;}
#define   DIS_TIMER1   TBCCTL0 &= ~CCIE
#define   EN_TIMER0    {TA0CCTL0 = CCIE;}
#define   DIS_TIMER0   TA0CCTL0 &= ~CCIE
#define   EN_INT        P1IE |= pinGIO2.pin_bm;
#define   DIS_INT       P1IE &= ~pinGIO2.pin_bm;

extern void Interrupt_Init(void);
extern void TestSend();

extern uint32 Frame_Time;
extern uint16 Keep_Alive_Count;
extern uint16 Keep_Alive_Detect;
extern uint8  ADCal_Flag;
extern uint16 Direction_Count;
extern uint8 DataSendDraw[TEST_LENGTH];
extern uint8 force_quit_quick_collect;


#endif