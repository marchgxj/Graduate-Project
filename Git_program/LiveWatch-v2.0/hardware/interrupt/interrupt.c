#include "common.h"
int time_out = 0;
u8 DataRecv[MAX_PACK_LENGTH];
u8 DataSend[MAX_PACK_LENGTH];
/*******************************************************************************
* Function Name  : Interrupt_Init
* Description    : This function init external interrupt for radio receive.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Interrupt_Init(void)
{
		EXTI_InitTypeDef EXTI_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);							//ʹ�ܸ��ù���ʱ��
    EXTI_ClearITPendingBit(EXTI_Line9);

		//GPIOA.0 �ж����Լ��жϳ�ʼ������ �����ش��� PA0  WK_UP
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource9); 
		EXTI_InitStructure.EXTI_Line=EXTI_Line9;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);																	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;						//ʹ�ܰ���KEY1,KEY0���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		//��ռ���ȼ�2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;						//�����ȼ�1 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);  	  														//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	  A7139_StrobeCmd(CMD_PLL);
		delay_us(2);
		A7139_StrobeCmd(CMD_RX);
		delay_us(2);

}
/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles period interrupt every 1S.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_IRQHandler(void)   //500ms
{
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
    //PostBeacon();
		//BeaconSendFlag = 1;
		time_out++;
		if(time_out > TIMEOUT)
		{
				A7139_StrobeCmd(CMD_STBY);
				delay_us(1);
				A7139_StrobeCmd(CMD_PLL);
				delay_us(1);
				A7139_StrobeCmd(CMD_RX);
				delay_us(1);
			  time_out = 0;
				printf("restart \r\n");
				LED2_REV();
		}	

}
/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : This function handles period interrupt every 100us.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM4_IRQHandler(void)   //100us
{
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
}
/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : This function handles radio receive.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint8 rssisee = 0;
void EXTI9_5_IRQHandler(void)
{
		uint8 rssi = 0;  
		uint8 i = 0;
		EXTI->PR |= EXTI_Line9;
	
    //	  LED2_OFF();
		//rssi = A7139_GetRSSI();
    A7139_ReadFIFO(DataRecv,MAX_PACK_LENGTH);
    delay_us(1);
    A7139_StrobeCmd(CMD_RX);
    delay_us(1);
		time_out = 0;
		Usart1_PutChar(0x7D);
	  for(i=0;i<MAX_PACK_LENGTH;i++)
		{
				Usart1_PutChar(DataRecv[i]);
		}
		LED1_REV();
   
}
void DisableInterrupt()
{
		__disable_irq(); 
}
void DisableInterrupt_EUSART()
{
		TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE ); 
}

void EnableInterrupt()
{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		USART_ClearFlag(USART1 , USART_FLAG_RXNE);
		EXTI->PR |= EXTI_Line9;
		__enable_irq(); 
}
