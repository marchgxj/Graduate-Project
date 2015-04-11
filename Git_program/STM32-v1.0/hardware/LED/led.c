/* Includes ------------------------------------------------------------------*/
#include "led.h"   
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
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);//ʹ��PC,PD�˿�ʱ��
    
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
    
}
