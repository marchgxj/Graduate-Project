#include "common.h"
u8 DataRecv[MAX_PACK_LENGTH];
u8 DataSend[MAX_PACK_LENGTH];
int time_out = 0;
void Interrupt_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);							//ʹ�ܸ��ù���ʱ��
    
    //GPIOA.0 �ж����Լ��жϳ�ʼ������ �����ش��� PA0  WK_UP
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource6); 
    EXTI_InitStructure.EXTI_Line=EXTI_Line6;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);																	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;						//ʹ�ܰ���KEY1,KEY0���ڵ��ⲿ�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;		//��ռ���ȼ�2 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;						//�����ȼ�1 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);  	  														//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
    
    A7139_StrobeCmd(CMD_PLL);
    delay_us(2);
    A7139_StrobeCmd(CMD_RX);
    delay_us(2);
    
}
void TIM3_IRQHandler(void)   //1s
{
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		time_out++;
		if(time_out>5)
		{
				A7139_StrobeCmd(CMD_STBY);
				delay_us(1);
				A7139_StrobeCmd(CMD_PLL);
				delay_us(1);
				A7139_StrobeCmd(CMD_RX);
				delay_us(1);
				LED2_REV();
			  time_out = 0;
				printf("restart \r\n");
		}
}
void TIM4_IRQHandler(void)   //100us
{
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
}
	  
void EXTI9_5_IRQHandler(void)
{
    int i=0;
		EXTI->PR |= EXTI_Line6;
    /************3.6ms*************/
    //	  LED2_OFF();
    A7139_ReadFIFO(DataRecv,MAX_PACK_LENGTH);
    delay_us(1);
    A7139_StrobeCmd(CMD_RX);
    delay_us(1);
		Usart1_PutChar(0x7D);
		Usart1_PutChar(0x7E);
		Usart1_PutData(DataRecv,MAX_PACK_LENGTH);
// 		Usart1_PutChar(0x0D);
// 		Usart1_PutChar(0x0A);
		//printf("\r\n");
		//printf("%d \r\n",DataRecv);
		//Usart1_PutData(DataRecv,PACK_LENGTH);
		LED1_REV();
		time_out = 0;
    /*****************************/
   
}
void DisableInterrupt()
{
		__disable_irq(); 
}
void EnableInterrupt()
{
		__enable_irq(); 
}
