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

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);							//使能复用功能时钟
    EXTI_ClearITPendingBit(EXTI_Line9);

		//GPIOA.0 中断线以及中断初始化配置 上升沿触发 PA0  WK_UP
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource9); 
		EXTI_InitStructure.EXTI_Line=EXTI_Line9;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);																	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;						//使能按键KEY1,KEY0所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		//抢占优先级2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;						//子优先级1 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);  	  														//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

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
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx更新中断标志 
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
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //清除TIMx更新中断标志 
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
		EXTI->PR |= EXTI_Line9;
	
    //	  LED2_OFF();
		//rssi = A7139_GetRSSI();
    A7139_ReadFIFO(DataRecv,MAX_PACK_LENGTH);
    delay_us(1);
    A7139_StrobeCmd(CMD_RX);
    delay_us(1);
		time_out = 0;
		Usart1_PutChar(0x7D);
		Usart1_PutChar(DataRecv[0]);
		Usart1_PutChar(DataRecv[1]);
		Usart1_PutChar(DataRecv[2]);
		Usart1_PutChar(DataRecv[3]);
		Usart1_PutChar(DataRecv[4]);
		Usart1_PutChar(DataRecv[5]);
		Usart1_PutChar(DataRecv[6]);
		Usart1_PutChar(DataRecv[7]);
		Usart1_PutChar(DataRecv[8]);
		Usart1_PutChar(DataRecv[9]);
		Usart1_PutChar(DataRecv[10]);
		Usart1_PutChar(DataRecv[11]);
		Usart1_PutChar(DataRecv[12]);
		Usart1_PutChar(DataRecv[13]);
		Usart1_PutChar(DataRecv[14]);
		Usart1_PutChar(DataRecv[15]);
		Usart1_PutChar(DataRecv[16]);
		Usart1_PutChar(DataRecv[17]);
		Usart1_PutChar(DataRecv[18]);
		Usart1_PutChar(DataRecv[19]);
		Usart1_PutChar(DataRecv[20]);
		Usart1_PutChar(DataRecv[21]);
		Usart1_PutChar(DataRecv[22]);
		Usart1_PutChar(DataRecv[23]);
		Usart1_PutChar(DataRecv[24]);
		Usart1_PutChar(DataRecv[25]);
		Usart1_PutChar(DataRecv[26]);
		Usart1_PutChar(DataRecv[27]);
		Usart1_PutChar(DataRecv[28]);
		Usart1_PutChar(DataRecv[29]);
		Usart1_PutChar(DataRecv[30]);
		Usart1_PutChar(DataRecv[31]);
		Usart1_PutChar(DataRecv[32]);
		Usart1_PutChar(DataRecv[33]);
		Usart1_PutChar(DataRecv[34]);
		Usart1_PutChar(DataRecv[35]);
		Usart1_PutChar(DataRecv[36]);
		Usart1_PutChar(DataRecv[37]);
		Usart1_PutChar(DataRecv[38]);
		Usart1_PutChar(DataRecv[39]);
		Usart1_PutChar(DataRecv[40]);
		Usart1_PutChar(DataRecv[41]);
		Usart1_PutChar(DataRecv[42]);
		Usart1_PutChar(DataRecv[43]);
		Usart1_PutChar(DataRecv[44]);
		Usart1_PutChar(DataRecv[45]);
		Usart1_PutChar(DataRecv[46]);
		Usart1_PutChar(DataRecv[47]);
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
