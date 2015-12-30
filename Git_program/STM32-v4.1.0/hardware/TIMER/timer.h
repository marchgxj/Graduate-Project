/* Includes ------------------------------------------------------------------*/
#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"


extern void TIM3_Int_Init(u16 arr,u16 psc);
extern void TIM4_Int_Init(u16 arr,u16 psc);
#define TIMEOUT  180    //1MIn
#define GET_TIME() TIM_GetCounter(TIM4)
#define RST_TIME() TIM_SetCounter(TIM4,0)
#endif
