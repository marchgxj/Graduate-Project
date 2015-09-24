/***********************************************************************************
    Filename: hal_digio.h

    Copyright 2007 Texas Instruments, Inc.
***********************************************************************************/

#ifndef DIGIO_H
#define DIGIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>


enum {
    HAL_DIGIO_INPUT,
    HAL_DIGIO_OUTPUT
};

enum {
    HAL_DIGIO_INT_FALLING_EDGE,
    HAL_DIGIO_INT_RISING_EDGE
};


typedef struct {
    uint8 port;     // port number
    uint8 pin;      // pin number
    uint8 pin_bm;   // pin bitmask
    uint8 dir;      // direction (input or output)
    uint8 initval;  // initial value
} digioConfig;


#define HAL_DIGIO_OK       0
#define HAL_DIGIO_ERROR  (~0)


//----------------------------------------------------------------------------------
// Function declarations
//----------------------------------------------------------------------------------

uint8 halDigioConfig(const digioConfig* p);
uint8 halDigioClear(const digioConfig* p);
uint8 halDigioToggle(const digioConfig* p);
uint8 halDigioSet(const digioConfig* p);
uint8 halDigioGet(const digioConfig* p);

uint8 halDigioIntConnect(const digioConfig *p, ISR_FUNC_PTR func);
uint8 halDigioIntEnable(const digioConfig *p);
uint8 halDigioIntDisable(const digioConfig *p);
uint8 halDigioIntClear(const digioConfig *p);
uint8 halDigioIntSetEdge(const digioConfig *p, uint8 edge);


#ifdef  __cplusplus
}
#endif

#define TIME1_HIGH    (P3OUT |= BIT6)
#define TIME1_LOW     (P3OUT &= ~ BIT6)
#define TIME2_HIGH    (P3OUT |= BIT7)
#define TIME2_LOW     (P3OUT &= ~ BIT7)
//#define SDA_L         (P6OUT &= ~ BIT6)
//#define SDA_H         (P6OUT |= BIT6)
//#define SCL_L         (P6OUT &= ~ BIT5)
//#define SCL_H         (P6OUT |= BIT5)
//#define SDA_IN        SDA_in();
//#define SDA_OUT       SDA_out();
//#define SDA_val       ((P6IN & BIT6)>>6)

#define SDA_L         (P3OUT &= ~ BIT3)
#define SDA_H         (P3OUT |= BIT3)
#define SCL_L         (P3OUT &= ~ BIT2)
#define SCL_H         (P3OUT |= BIT2)
#define SDA_in        P3DIR &= ~BIT3
#define SDA_out       P3DIR |= BIT3
#define SDA_val       P3IN&BIT3
#define IRD_LOW       (P1OUT &= ~ BIT6)
#define IRD_HIGH      (P1OUT |= BIT6)
extern void GPIO_Init();


/**********************************************************************************/
#endif
