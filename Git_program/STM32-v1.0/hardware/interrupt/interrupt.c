#include "common.h"
u16 BeaconTimeCount = 0;
u8 BeaconSendFlag = 0;
u16 node2_count = 0; 
u16 node3_count = 0; 
u16 node4_count = 0; 
u16 node2_error = 0; 
u16 node3_error = 0; 
u16 node4_error = 0; 
u16 node2_reset = 0; 
u16 node3_reset = 0; 
u16 node4_reset = 0;
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
    
    //A7139_StrobeCmd(CMD_PLL);
    //delay_us(2);
    //A7139_StrobeCmd(CMD_RX);
    //delay_us(2);
    
}
void TIM3_IRQHandler(void)   //1s
{
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
    LED3_REV();
    
    BeaconTimeCount++;
    BeaconSendFlag = 1;
    
}
void TIM4_IRQHandler(void)   //100us
{
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
}
void EXTI9_5_IRQHandler(void)
{
    EXTI->PR |= EXTI_Line6;
    /************3.6ms*************/
    //	  LED2_OFF();
    A7139_ReadFIFO(DataRecv,32);
    delay_us(2);
    A7139_StrobeCmd(CMD_RX);
    delay_us(2);
    /*****************************/
    LED4_REV();
    
    if(DataRecv[1] == 0x02)
    {
        node2_count=DataRecv[7]<<8;
        node2_count|=DataRecv[8];
        node2_reset = DataRecv[3];
        if(DataRecv[6] != 0xAA)
        {
            node2_error++;
        }
    }
    if(DataRecv[1] == 0x03)
    {
        node3_count=DataRecv[9]<<8;
        node3_count|=DataRecv[10];
        node3_reset = DataRecv[3];
        if(DataRecv[6] != 0xAA)
        {
            node3_error++;
        }
    }
    if(DataRecv[1] == 0x04)
    {
        node4_count=DataRecv[11]<<8;
        node4_count|=DataRecv[12];
        node4_reset = DataRecv[3];
        if(DataRecv[6] != 0xAA)
        {
            node4_error++;
        }
    }
    
    //LED2_ON();
    
}
__asm void DisableInterrupt()
{
		cpsie I;
}
__asm void EnableInterrupt()
{
		cpsid I;
}
