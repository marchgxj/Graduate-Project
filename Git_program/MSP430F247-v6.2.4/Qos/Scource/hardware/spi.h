/* Includes ------------------------------------------------------------------*/
#ifndef __SPI_H
#define __SPI_H
#include <msp430.h>
#include "sys.h"
#include "types.h"
#include "sys.h"

extern void SPI1_Init(void);			 						//��ʼ��SPI1��
extern void SPI2_Init(void);			 						//��ʼ��SPI2��
extern void SPI1_SetSpeed(uint8 SpeedSet); 			//����SPI1�ٶ�  		 
extern void SPI2_SetSpeed(uint8 SpeedSet); 			//����SPI2�ٶ�   
uint8 SPI1_ReadWriteByte(uint8 TxData);			//SPI1���߶�дһ���ֽ�
uint8 SPI2_ReadWriteByte(uint8 TxData);			//SPI2���߶�дһ���ֽ�
extern void ByteSend(uint8 src);
extern uint8 ByteRead(void);
extern void MOSI_IN(void);
extern void MOSI_OUT(void);
extern void SPIx_WriteByte(uint8 dat);
extern void SPI3_Init(void);
extern void SPIx_WriteWord(uint16 wrd);
extern uint16 SPIx_ReadWord(void);
extern uint8 SPIx_ReadByte(void);
extern void GO_Init(void);


#define MOSI_DATA  ((P1IN & BIT2)>>2)
#define GIO2       ((P1IN & BIT4)>>4)
#define GIO1       ((P1IN & BIT3)>>3)

#define SCK_CLEAR  (P1OUT &= ~BIT1)
#define SCK_SET    (P1OUT |= BIT1)
#define MOSI_CLEAR (P1OUT &= ~BIT2)
#define MOSI_SET   (P1OUT |= BIT2)
#define SCS_CLEAR  (P1OUT &= ~BIT0)
#define SCS_SET    (P1OUT |= BIT0)

	 
#endif
