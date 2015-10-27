/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "common.h"
/*
 * ��������NVIC_Configuration
 * ����  ������NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
 * ����  ����
 * ���  ����
 */
void NVIC_Configuration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
}
void sys_init(void)
{
		delay_init();	    	 			
		NVIC_Configuration(); 	 	
		uart_init();	 		
		LED_Init();			     			
		KEY_Init();	
		SPI3_Init();
		//A7139_Init(487.501f);
	  TIM3_Int_Init(BECAON_PERIOD*10-1,7200-1);	//10Khz�ļ���Ƶ�ʣ�������1000Ϊ100ms 
	  //TIM4_Int_Init(72,99);  //��Сʱ�䵥λ 1us
		Interrupt_Init();
		Init_TQ();
		Init_Upload_TQ();
		
}
