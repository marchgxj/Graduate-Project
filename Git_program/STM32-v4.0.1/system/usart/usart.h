/******************** (C) COPYRIGHT 2013 VeryAI Team **************************
 * 作者    ：VeryAI Team
 * 库版本  ：ST3.5.0
 * 实验平台：VeryAI STM32开发板
 * 描述    : 串口驱动代码  
 *
 * 论坛    ：http://www.veryai.com
 * 淘宝    ：http://veryaishop.taobao.com
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
	
#define USART_REC_LEN	200  	//定义最大接收字节数 200
#define EN_USART1_RX 	1			//使能（1）/禁止（0）串口1接收



#define DIS_USARTINT USART_ITConfig(USART1 , USART_IT_RXNE, DISABLE);
#define EN_USARTINT  USART_ITConfig(USART1 , USART_IT_RXNE, ENABLE);
	  	
extern u8  USART_RX_BUF[USART_REC_LEN];//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;    //接收状态标记	

//如果想串口中断接收，请不要注释以下宏定义
void uart_init(void);
extern void Usart1_PutChar(uint8_t ch);
extern void Usart1_PutData(uint8_t *buffer,uint8_t count);
extern uint8 USART1_Getchar(void);
extern void DebugMsg(char* msg);
extern void LinkSend(void);
#endif


