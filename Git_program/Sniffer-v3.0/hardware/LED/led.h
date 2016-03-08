/* Includes ------------------------------------------------------------------*/
#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define LED0_ON() PAout(0)=0
#define LED1_ON() PAout(1)=0
#define LED2_ON() PAout(2)=0
#define LED3_ON() PAout(3)=0
#define LED4_ON() PAout(4)=0
#define LED5_ON() PAout(5)=0
#define LED6_ON() PAout(6)=0
#define LED7_ON() PAout(7)=0

#define LED0_OFF() PAout(0)=1
#define LED1_OFF() PAout(1)=1
#define LED2_OFF() PAout(2)=1
#define LED3_OFF() PAout(3)=1
#define LED4_OFF() PAout(4)=1
#define LED5_OFF() PAout(5)=1
#define LED6_OFF() PAout(6)=1
#define LED7_OFF() PAout(7)=1

#define LED0_REV() PAout(0)^=1
#define LED1_REV() PAout(1)^=1
#define LED2_REV() PAout(2)^=1
#define LED3_REV() PAout(3)^=1
#define LED4_REV() PAout(4)^=1
#define LED5_REV() PAout(5)^=1
#define LED6_REV() PAout(6)^=1
#define LED7_REV() PAout(7)^=1

void LED_Init(void);//≥ı ºªØ
	 				    
#endif
