/* Includes ------------------------------------------------------------------*/
#include "key.h"
#include "common.h"

/*
 * ��������KEY_Init
 * ����  ��������ʼ������
 * ����  ����
 * ���  ����
 */	 
void KEY_Init(void) 														//IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD,ENABLE);//ʹ��PORTA,PORTC,PORTDʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//���ó���������
 	GPIO_Init(GPIOC, &GPIO_InitStructure);				//��ʼ��GPIO

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//���ó���������
 	GPIO_Init(GPIOD, &GPIO_InitStructure);				//��ʼ��GPIO
	
	//��ʼ�� WK_UP-->GPIOA.0	��������
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	//PA0���ó����룬Ĭ������	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//��ʼ��GPIOA.0
}

/*
 * ��������KEY_Init
 * ����  ������������
					 0��û���κΰ�������
					 1��KEY0����
					 2��KEY1����
					 3��KEY2���� WK_UP
					 ע��˺�������Ӧ���ȼ�,KEY2>KEY0>KEY1
 * ����  ��mode:0,��֧��������;1,֧��������;
 * ���  �����ذ���ֵ
 */	 
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;														//�������ɿ���־
	if(mode)key_up=1;  														//֧������		  
	if(key_up&&(KEY0==0||KEY1==0||KEY2==1))
	{
		delay_ms(10);																//ȥ���� 
		key_up=0;
		if(KEY2==1)return KEY_UP;
		else if(KEY0==0)return KEY_DOWN;
		else if(KEY1==0)return KEY_RIGHT;
	}else if(KEY2==0&&KEY0==1&&KEY1==1)key_up=1; 	// �ް�������
 	return 0;																			
}
