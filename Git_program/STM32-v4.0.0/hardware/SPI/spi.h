/* Includes ------------------------------------------------------------------*/
#ifndef __SPI_H
#define __SPI_H
#include "sys.h"

extern void SPI1_Init(void);			 						//��ʼ��SPI1��
extern void SPI2_Init(void);			 						//��ʼ��SPI2��
extern void SPI1_SetSpeed(u8 SpeedSet); 			//����SPI1�ٶ�  		 
extern void SPI2_SetSpeed(u8 SpeedSet); 			//����SPI2�ٶ�   
u8 SPI1_ReadWriteByte(u8 TxData);			//SPI1���߶�дһ���ֽ�
u8 SPI2_ReadWriteByte(u8 TxData);			//SPI2���߶�дһ���ֽ�
extern void ByteSend(u8 src);
extern u8 ByteRead(void);
extern void MOSI_IN(void);
extern void MOSI_OUT(void);
extern void SPIx_WriteByte(u8 dat);
extern void SPI3_Init(void);
extern void SPIx_WriteWord(u16 wrd);
extern u16 SPIx_ReadWord(void);
extern u8 SPIx_ReadByte(void);
extern void GO_Init(void);
#define SCK PAout(5)
#define SCS PAout(4)
#define MOSI PAout(6)
#define MOSI_DATA GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)
#define GIO2		  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)
#define GIO1		  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)
//#define GIO2		  PBout(5)

		 
#endif
