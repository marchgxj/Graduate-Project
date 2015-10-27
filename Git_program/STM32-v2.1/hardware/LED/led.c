/* Includes ------------------------------------------------------------------*/  
#include "common.h"

/*
 * ��������LED_Init
 * ����  ����ʼ��PC3��PD6Ϊ�����.��ʹ���������ڵ�ʱ��,LED IO��ʼ��
 * ����  ����
 * ���  ����
 */	    
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD,ENABLE);//ʹ��PORTA,PORTC,PORTDʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;								//�˿�����, �������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 			//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 			//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOD, &GPIO_InitStructure);	  				     		//�����趨������ʼ��PD.12
	GPIO_SetBits(GPIOD,GPIO_Pin_0);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ;								//�˿�����, �������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 			//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 			//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOD, &GPIO_InitStructure);	  				     		//�����趨������ʼ��PC.3
	GPIO_SetBits(GPIOD,GPIO_Pin_1);														//PC.3����� 	

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;								//�˿�����, �������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 			//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 			//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOD, &GPIO_InitStructure);	  				     		//�����趨������ʼ��PD.12
	GPIO_SetBits(GPIOD,GPIO_Pin_2);													//PD.12����� 


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ;								//�˿�����, �������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 			//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 			//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOD, &GPIO_InitStructure);	  				     		//�����趨������ʼ��PD.12
	GPIO_SetBits(GPIOD,GPIO_Pin_3);													//PD.12����� 


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;								//�˿�����, �������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 			//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 			//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOD, &GPIO_InitStructure);	  				     		//�����趨������ʼ��PD.12
	GPIO_SetBits(GPIOD,GPIO_Pin_4);													//PD.12����� 	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 ;								//�˿�����, �������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 			//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 			//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOD, &GPIO_InitStructure);	  				     		//�����趨������ʼ��PD.12
	GPIO_SetBits(GPIOD,GPIO_Pin_5);													//PD.12�����

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;								//�˿�����, �������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 			//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 			//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOD, &GPIO_InitStructure);	  				     		//�����趨������ʼ��PD.12
	GPIO_SetBits(GPIOD,GPIO_Pin_6);													//PD.12����� 
	
	//GIO1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ;								//�˿�����, �������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 			//�������
	GPIO_Init(GPIOC, &GPIO_InitStructure);	  				     		//�����趨������ʼ��PD.12

	//TIME1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;								//�˿�����, �������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 			//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 			//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);	  				     		//�����趨������ʼ��PD.12
	TIME1_LOW;
	
	//TIM2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ;								//�˿�����, �������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 			//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 			//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);	  				     		//�����趨������ʼ��PD.12
	TIME2_LOW;
	
// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ;								//�˿�����, �������
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 			//�������
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 			//IO���ٶ�Ϊ50MHz
// 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  				     		//�����趨������ʼ��PD.12
// 	GPIO_SetBits(GPIOA,GPIO_Pin_9);													//PD.12����� 
// 	
// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 ;								//�˿�����, �������
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 			//�������
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 			//IO���ٶ�Ϊ50MHz
// 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  				     		//�����趨������ʼ��PD.12
// 	GPIO_SetBits(GPIOA,GPIO_Pin_10);													//PD.12����� 

}
