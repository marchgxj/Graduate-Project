#include "common.h"
uint16 Receive_Timeout = 0;
uint8 TIM3_Count = 0;
uint16 KeepAliveCheck_Count = 0;
uint32 KeepAliveCount = 0;
uint8 DataPool[MAX_PACK_LENGTH * 20];
uint16 Data_Current = 0;
uint16 A7139_Reset_Count = 0;
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

		//GPIOA.0 �ж����Լ��жϳ�ʼ������ �����ش��� PA0  WK_UP
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource6); 
		EXTI_InitStructure.EXTI_Line=EXTI_Line6;
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
		
		
		/*//GPIOA.0 �ж����Լ��жϳ�ʼ������ �����ش��� PA0  WK_UP
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource3); 
		EXTI_InitStructure.EXTI_Line=EXTI_Line3;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);																	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;						//ʹ�ܰ���KEY1,KEY0���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		//��ռ���ȼ�2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;						//�����ȼ�1 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); */
		

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
    TIM3_Count++;
		KeepAliveCount++;
		if(TIM3_Count<5)
		{	
#if (SEND_REJOIN_EN == 1)
					PostTask(EmptyBuffer,EVENT_REJOIN_SEND);
#endif
		}
		else
		{
				TIM3_Count = 10;
			if(PostBeacon()!=TQ_SUCCESS)
			{
					DebugMsg("Post Beacon Fail");
					LED0_ON();
					LED1_ON();
					LED2_ON();
					LED3_ON();
					LED4_ON();
					LED5_ON();
					LED6_ON();
			}
			
#if (UPLOAD_DATA_EN == 1)
				PostTask(EmptyBuffer,EVENT_UPLOAD_DATA);
#endif
		}
		
		KeepAliveCheck_Count++;
		if(KeepAliveCheck_Count == KEEPALIBEPERIOD)
		{
				KeepAliveCheck_Count = 0;
				PostTask(EmptyBuffer,EVENT_KEEPALIVE_CHECK);
		}
		
		Frame_Time = 0;
		//BeaconSendFlag = 1;
		Receive_Timeout++;
		if(Receive_Timeout > TIMEOUT)
		{
				Receive_Timeout = 0;
			  PostTask(EmptyBuffer,EVENT_A7139_RESET);
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
	  Frame_Time++;
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
    EXTI->PR |= EXTI_Line6;
		
    A7139_ReadFIFO(DataRecvBuffer,MAX_PACK_LENGTH);
		
    RXMode();
		
		if(PackValid())
		{
				LED2_REV();
			  Receive_Timeout = 0;
				switch (Unpack(DataRecvBuffer))
				{
						case JOINREQUEST_TYPE:
							PostTask(DataRecvBuffer,EVENT_JOINREQUEST_HANDLER);
							break;
						case JOINREQUESTACKOK_TYPE:
							PostTask(DataRecvBuffer,EVENT_JOINREQUESTACKOK_HANDLER);
							break;
						case DATA_TYPE:
							PostTask(DataRecvBuffer,EVENT_DATA_HANDLER);
							break;
				}
		}
		
   
}
uint8 mode = 0;
void EXTI3_IRQHandler(void)
{
		EXTI->PR |= EXTI_Line3;
		/*if(mode==254)
			mode = 0;
		mode++;
		if(mode%2==0)
		{
				Power_Mode = 1;
				LED5_ON();
		}
		else
		{
				Power_Mode = 0;
				LED5_OFF();
		}*/
			
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
		EXTI->PR |= EXTI_Line6;
		__enable_irq(); 
}