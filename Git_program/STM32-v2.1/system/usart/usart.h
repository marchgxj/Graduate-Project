/******************** (C) COPYRIGHT 2013 VeryAI Team **************************
 * ����    ��VeryAI Team
 * ��汾  ��ST3.5.0
 * ʵ��ƽ̨��VeryAI STM32������
 * ����    : ������������  
 *
 * ��̳    ��http://www.veryai.com
 * �Ա�    ��http://veryaishop.taobao.com
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
	
#define USART_REC_LEN	200  	//�����������ֽ��� 200
#define EN_USART1_RX 	1			//ʹ�ܣ�1��/��ֹ��0������1����

#define DIS_USARTINT USART_ITConfig(USART1 , USART_IT_RXNE, DISABLE);
#define EN_USARTINT  USART_ITConfig(USART1 , USART_IT_RXNE, ENABLE);
	  	
extern u8  USART_RX_BUF[USART_REC_LEN];//���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;    //����״̬���	

//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(void);
extern void Usart1_PutChar(uint8_t ch);
extern void Usart1_PutData(uint8_t *buffer,uint8_t count);
extern uint8 USART1_Getchar(void);
#endif


