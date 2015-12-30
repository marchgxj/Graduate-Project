/* Includes ------------------------------------------------------------------*/
#ifndef __SPI_H
#define __SPI_H
#include "sys.h"

extern void SPI1_Init(void);			 						//初始化SPI1口
extern void SPI2_Init(void);			 						//初始化SPI2口
extern void SPI1_SetSpeed(u8 SpeedSet); 			//设置SPI1速度  		 
extern void SPI2_SetSpeed(u8 SpeedSet); 			//设置SPI2速度   
u8 SPI1_ReadWriteByte(u8 TxData);			//SPI1总线读写一个字节
u8 SPI2_ReadWriteByte(u8 TxData);			//SPI2总线读写一个字节
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
#define SCS PBout(5)
#define SCK PBout(6)
#define MOSI PBout(7)
#define MOSI_DATA GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)
////#define GIO2		  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)
#define GIO1		  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)
////#define GIO2		  PBout(5)

		 
#endif
