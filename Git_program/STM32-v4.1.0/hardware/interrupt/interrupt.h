#ifndef _INTERRUPT_h_
#define _INTERRUPT_h_
#include "common.h"

extern void Interrupt_Init(void);
extern void DisableInterrupt(void);
extern void EnableInterrupt(void);



extern u8 BeaconSendFlag;
extern uint32 KeepAliveCount;
extern uint16 A7139_Reset_Count ;
#endif
