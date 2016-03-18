/***********************************************************************************
    Filename: digio.c

    Copyright 2007 Texas Instruments, Inc.
***********************************************************************************/

#include "digio.h"
#include "common.h"

// The number of ports and pins supported by a microcontroller will
// vary (e.g. different families of MSP430 have different number of
// ports, and not all will support interrupts on port 2). The TEMPLATE
// HAL implements support for interrupts on port 1 and up to two ports.


//static ISR_FUNC_PTR port1_isr_tbl[8] = {0};
//static ISR_FUNC_PTR port2_isr_tbl[8] = {0};



//----------------------------------------------------------------------------------
//  uint8 halDigioConfig(const digioConfig* p)
//
//  DESCRIPTION:
//    Configure the pin as specified by p.
//----------------------------------------------------------------------------------
uint8 halDigioConfig(const digioConfig* p)
{
    register volatile uint8* dir;
    register volatile uint8* out;
    register const uint8 bitmask = p->pin_bm;

    // Sanity check
    if ((bitmask == 0) || (bitmask != (uint8)BV(p->pin)))
    {
        return(HAL_DIGIO_ERROR);
    }

    switch(p->port)
    {
        case 1: P1SEL &= ~bitmask; out = &P1OUT; dir = &P1DIR; break;
        case 2: P2SEL &= ~bitmask; out = &P2OUT; dir = &P2DIR; break;
        case 3: P3SEL &= ~bitmask; out = &P3OUT; dir = &P3DIR; break;
        case 4: P4SEL &= ~bitmask; out = &P4OUT; dir = &P4DIR; break;
        case 5: P5SEL &= ~bitmask; out = &P5OUT; dir = &P5DIR; break;
        case 6: P6SEL &= ~bitmask; out = &P6OUT; dir = &P6DIR; break;
        default: return(HAL_DIGIO_ERROR);
    }

    if (p->dir == HAL_DIGIO_OUTPUT)
    {
        if (p->initval == 1)
        {
            *out |= bitmask;
        }
        else
        {
            *out &= ~bitmask;
        }
        *dir |= bitmask;
    }
    else // input
    {
        *out &= ~bitmask;
        *dir &= ~bitmask;
    }
    return(HAL_DIGIO_OK);
}

//----------------------------------------------------------------------------------
//  uint8 halDigioSet(digioConfig* p)
//----------------------------------------------------------------------------------
uint8 halDigioSet(const digioConfig* p)
{
    if (p->dir == HAL_DIGIO_OUTPUT)
    {
        switch (p->port)
        {
            case 1: P1OUT |= p->pin_bm; break;
            case 2: P2OUT |= p->pin_bm; break;
            case 3: P3OUT |= p->pin_bm; break;
            case 4: P4OUT |= p->pin_bm; break;
            case 5: P5OUT |= p->pin_bm; break;
            case 6: P6OUT |= p->pin_bm; break;
            default: return(HAL_DIGIO_ERROR);
        }
        return(HAL_DIGIO_OK);
    }
    return(HAL_DIGIO_ERROR);
}

//----------------------------------------------------------------------------------
//  uint8 halDigioClear(const digioConfig* p)
//----------------------------------------------------------------------------------
uint8 halDigioClear(const digioConfig* p)
{
    if (p->dir == HAL_DIGIO_OUTPUT)
    {
        switch (p->port)
        {
            case 1: P1OUT &= ~p->pin_bm; break;
            case 2: P2OUT &= ~p->pin_bm; break;
            case 3: P3OUT &= ~p->pin_bm; break;
            case 4: P4OUT &= ~p->pin_bm; break;
            case 5: P5OUT &= ~p->pin_bm; break;
            case 6: P6OUT &= ~p->pin_bm; break;
            
            default: return(HAL_DIGIO_ERROR);
        }
        return(HAL_DIGIO_OK);
    }
    return(HAL_DIGIO_ERROR);
}

//----------------------------------------------------------------------------------
//  uint8 halDigioToggle(const digioConfig* p)
//----------------------------------------------------------------------------------
uint8 halDigioToggle(const digioConfig* p)
{
    if (p->dir == HAL_DIGIO_OUTPUT)
    {
        switch (p->port)
        {
            case 1: P1OUT ^= p->pin_bm; break;
            case 2: P2OUT ^= p->pin_bm; break;
            case 3: P3OUT ^= p->pin_bm; break;
            case 4: P4OUT ^= p->pin_bm; break;
            case 5: P5OUT ^= p->pin_bm; break;
            case 6: P6OUT ^= p->pin_bm; break;
            
            default: return(HAL_DIGIO_ERROR);
        }
        return(HAL_DIGIO_OK);
    }
    return(HAL_DIGIO_ERROR);
}

//----------------------------------------------------------------------------------
//  uint8 halDigioGet(const digioConfig* p)
//----------------------------------------------------------------------------------
uint8 halDigioGet(const digioConfig* p)
{
    if (p->dir == HAL_DIGIO_INPUT)
    {
        switch (p->port)
        {
            case 1: return (P1IN & p->pin_bm ? 1 : 0);
            case 2: return (P2IN & p->pin_bm ? 1 : 0);
            case 3: return (P3IN & p->pin_bm ? 1 : 0);
            case 4: return (P4IN & p->pin_bm ? 1 : 0);
            case 5: return (P5IN & p->pin_bm ? 1 : 0);
            case 6: return (P6IN & p->pin_bm ? 1 : 0);
            
            default: break;
        }
    }
    return(HAL_DIGIO_ERROR);
}

//----------------------------------------------------------------------------------
//  uint8 halDigioIntConnect(const digioConfig *p, ISR_FUNC_PTR func)
//----------------------------------------------------------------------------------
//uint8 halDigioIntConnect(const digioConfig *p, ISR_FUNC_PTR func)
//{
//    istate_t key;
//    HAL_INT_LOCK(key);
//    switch (p->port)
//    {
//        case 1: port1_isr_tbl[p->pin] = func; break;
//        case 2: port2_isr_tbl[p->pin] = func; break;
//        default: HAL_INT_UNLOCK(key); return(HAL_DIGIO_ERROR);
//    }
//    halDigioIntClear(p);
//    HAL_INT_UNLOCK(key);
//    return(HAL_DIGIO_OK);
//}


//----------------------------------------------------------------------------------
//  uint8 halDigioIntEnable(const digioConfig *p)
//----------------------------------------------------------------------------------
uint8 halDigioIntEnable(const digioConfig *p)
{
    switch (p->port)
    {
        case 1: P1IE |= p->pin_bm; break;
        case 2: P2IE |= p->pin_bm; break;
        default: return(HAL_DIGIO_ERROR);
    }
    return(HAL_DIGIO_OK);
}

//----------------------------------------------------------------------------------
//  uint8 halDigioIntDisable(const digioConfig *p)
//----------------------------------------------------------------------------------
uint8 haldigioIntDisable(const digioConfig *p)
{
    switch (p->port)
    {
        case 1: P1IE &= ~p->pin_bm; break;
        case 2: P2IE &= ~p->pin_bm; break;
        default: return(HAL_DIGIO_ERROR);
    }
    return(HAL_DIGIO_OK);
}

//----------------------------------------------------------------------------------
//  uint8 halDigioIntClear(const digioConfig *p)
//----------------------------------------------------------------------------------
uint8 halDigioIntClear(const digioConfig *p)
{
    switch (p->port)
    {
        case 1: P1IFG &= ~p->pin_bm; break;
        case 2: P2IFG &= ~p->pin_bm; break;
        default: return(HAL_DIGIO_ERROR);
    }
    return(HAL_DIGIO_OK);
}

//----------------------------------------------------------------------------------
//  uint8 halDigioIntSetEdge(const digioConfig *p, uint8 edge)
//----------------------------------------------------------------------------------
uint8 halDigioIntSetEdge(const digioConfig *p, uint8 edge)
{
    switch(edge)
    {
        case HAL_DIGIO_INT_FALLING_EDGE:
            switch(p->port)
            {
                case 1: P1IES |= p->pin_bm; break;
                case 2: P2IES |= p->pin_bm; break;
                default: return(HAL_DIGIO_ERROR);
            }
            break;

         case HAL_DIGIO_INT_RISING_EDGE:
            switch(p->port)
            {
                case 1: P1IES &= ~p->pin_bm; break;
                case 2: P2IES &= ~p->pin_bm; break;
                default: return(HAL_DIGIO_ERROR);
            }
            break;

         default:
            return(HAL_DIGIO_ERROR);
    }
    return(HAL_DIGIO_OK);
}
const digioConfig pinLed1   = {5, 1, BIT1, HAL_DIGIO_OUTPUT, 0};
const digioConfig pinLed2   = {5, 2, BIT2, HAL_DIGIO_OUTPUT, 0};
const digioConfig pinLed3   = {5, 3, BIT3, HAL_DIGIO_OUTPUT, 0};
const digioConfig pinLed4   = {5, 4, BIT4, HAL_DIGIO_OUTPUT, 0};
const digioConfig pinGIO2   = {1, 4, BIT4, HAL_DIGIO_INPUT,  0};
const digioConfig pinGIO1   = {1, 3, BIT3, HAL_DIGIO_INPUT,  0};
const digioConfig pinKZ   = {6, 5, BIT5, HAL_DIGIO_OUTPUT, 0};
const digioConfig TIME1   = {3, 6, BIT6, HAL_DIGIO_OUTPUT, 0};
const digioConfig TIME2   = {3, 7, BIT7, HAL_DIGIO_OUTPUT, 0};
//const digioConfig SCL      = {6, 5, BIT5, HAL_DIGIO_OUTPUT, 0};
//const digioConfig SDA      = {6, 6, BIT6, HAL_DIGIO_OUTPUT, 0};
//const digioConfig SDAIN   = {6, 6, BIT6, HAL_DIGIO_INPUT, 0};
const digioConfig SDAIN   = {3, 1, BIT1, HAL_DIGIO_INPUT, 0};
const digioConfig SCL      = {3, 2, BIT2, HAL_DIGIO_OUTPUT, 1};
const digioConfig SDA     = {3, 1, BIT1, HAL_DIGIO_OUTPUT, 1};
const digioConfig IRD     = {1, 6, BIT6, HAL_DIGIO_OUTPUT, 0};

const digioConfig A7139_EN     = {2, 0, BIT0, HAL_DIGIO_OUTPUT, 0};

void GPIO_Init()
{
    halDigioConfig(&pinLed1);
    halDigioConfig(&pinLed2);
    halDigioConfig(&pinLed3);
    halDigioConfig(&pinLed4);
    halDigioConfig(&pinGIO1);
    halDigioConfig(&pinGIO2);//�ж�
    halDigioConfig(&pinKZ);
    halDigioConfig(&TIME1);
    halDigioConfig(&TIME2);
    halDigioConfig(&SDA);
    halDigioConfig(&SCL);
    halDigioConfig(&IRD);
    halDigioConfig(&A7139_EN);
    P2OUT |= BIT0;
    SDA_L;
    SCL_L;
    
}
//void SDA_in()
//{
//    halDigioConfig(&SDAIN);
//}
//void SDA_out()
//{
//    halDigioConfig(&SDA);
//}
