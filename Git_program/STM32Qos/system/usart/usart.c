/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "usart.h"	  
#include "common.h"
 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos ʹ��	  
#endif  
 
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); 
    USART_SendData(USART1,(uint8_t)ch);   
	return ch;
}
#endif 
 

/*
 * ��������uart_init
 * ����  ����ʼ��IO ����1 
 * ����  ��bound:������
 * ���  ����
 */	
void uart_init(){
	USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef  GPIO_InitStructure;             
  NVIC_InitTypeDef 	NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	USART_InitStructure.USART_BaudRate = 115200; 
  USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
  USART_InitStructure.USART_StopBits = USART_StopBits_1;    
  USART_InitStructure.USART_Parity = USART_Parity_No ;      
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;   
  
  USART_Init(USART1, &USART_InitStructure);  
  USART_Cmd(USART1, ENABLE);  

	//USART_ITConfig(USART1 , USART_IT_RXNE, ENABLE);
//******************************************************************************
//  ����1��ʹ�ùܽ�������붨��
//******************************************************************************
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;         //IO�ڵĵ�12��
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  //IO���ٶ�
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //IO�ڸ����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);            //��ʼ������2���IO��


  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;            //IO�ڵĵ�2��
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //IO���ٶ�
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//IO����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);               //��ʼ������2����IO
	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//  NVIC_InitStructure.NVIC_IRQChannel	=	USART1_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	=	3;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority	=	2;
//  NVIC_InitStructure.NVIC_IRQChannelCmd	=	ENABLE;
//  NVIC_Init(&NVIC_InitStructure);

}
void Usart1_PutChar(uint8_t ch)
{
    USART_SendData(USART1,  ch);

    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {} 
}
void Usart1_PutData(uint8_t *buffer,uint8_t count)
{
		int i;
		for (i = 0; i < count; i++)
    {
				Usart1_PutChar(buffer[i]);
    }
		//printf("n");
}
/*
 * ��������USART1_IRQHandler
 * ����  ������1�жϷ������
 * ����  ����
 * ���  ����
 */	
uint8 buda = 0;
uint8 Usart_Count = 0;
uint8 Usart_Data[4] = 0;
void USART1_IRQHandler(void)
{
	uint8 buf;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//�����ж�
	{
			USART_ClearFlag(USART1 , USART_FLAG_RXNE);
		  buf = USART_ReceiveData(USART1);
			if(buf == 'o')
			{
					Upload_Ack = 1;
			}
			else
			{
					Upload_Ack = 0;
				  if(buf == 0xAA)
					{
							Usart_Count = 0;
					}
					Usart_Data[Usart_Count++] = buf;
					if(buf==0xBB)
					{
							Usart_Count = 0;
							Cmd_Address = Usart_Data[1];
							Cmd_Command = Usart_Data[2];
							Usart1_PutChar(0xAA);
							LED5_REV();
					}
			}
			
	}
} 
uint8 USART1_Getchar(void)
{
		return USART_ReceiveData(USART1); 
}

#define DEBUGMSG_EN  1
void DebugMsg(char* msg)
{
#if DEBUGMSG_EN == 1
		uint16 i=0;
		Usart1_PutChar(0x7D);
		Usart1_PutChar(0x7F);
		
		printf("%s\n",msg);
#endif
}


