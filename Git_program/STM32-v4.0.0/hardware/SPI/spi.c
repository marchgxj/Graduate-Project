/* Includes ------------------------------------------------------------------*/
#include "spi.h"
#include "common.h"

	
void MOSI_IN(void)
{
	/*GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//���ó���������
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//��ʼ��GPIO*/
	GPIOA->CRL&=0xFFFF0FFF;
	GPIOA->CRL|=0x00008000;
	//GPIOA->ODR|=1<<6;
	
}
void MOSI_OUT(void)
{
	/*GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//��ʼ��GPIO*/
	
	GPIOA->CRL&=0xFFFF0FFF;
	GPIOA->CRL|=0x00003000;
}
void SPI3_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
	//scs
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);				
	//sck
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);				
	//sdio
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//GIO2
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//GIO1
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  /*GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);*/
		
	
	SCS = 1;
	SCK = 0;
	
}



/*
 * ��������SPI1_Init
 * ����  ��SPI1ģ��ĳ�ʼ�����룬���ó�����ģʽ������SD Card/W25X16/24L01/JF24C	
 * ����  ����
 * ���  ����
 */						  
void SPI1_Init(void)
{	    
 	GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
	 
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA|RCC_APB2Periph_SPI1, ENABLE );//PORTB��SPI2ʱ��ʹ�� 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //PA5.6.7��������  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_4); //PA5.6.7����
			
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;		//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//ѡ���˴���ʱ�ӵ���̬:ʱ�����ո�
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//���ݲ����ڵڶ���ʱ����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ16
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI1, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
	SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����
	
	SPI2_ReadWriteByte(0xff);//��������	
}

/*
 * ��������SPI2_Init
 * ����  ��SPI2ģ��ĳ�ʼ�����룬���ó�����ģʽ������SD Card/W25X16/24L01/JF24C	
 * ����  ����
 * ���  ����
 */	
void SPI2_Init(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTBʱ��ʹ�� 
	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );//SPI2ʱ��ʹ�� 	
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //PB13/14/15����������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);  //PB13/14/15����

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//ѡ���˴���ʱ�ӵ���̬:ʱ�����ո�
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//���ݲ����ڵڶ���ʱ����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ16
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI2, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
	SPI_Cmd(SPI2, ENABLE); //ʹ��SPI����
	
	SPI2_ReadWriteByte(0xff);//��������	 

}

/*
 * ��������SPI2_SetSpeed
 * ����  ��SPI2�ٶ����ú���	
 * ����  ��SpeedSet:
					 SPI_BaudRatePrescaler_2   2��Ƶ   
					 SPI_BaudRatePrescaler_8   8��Ƶ   
					 SPI_BaudRatePrescaler_16  16��Ƶ  
					 SPI_BaudRatePrescaler_256 256��Ƶ 
 * ���  ����
 */	
void SPI2_SetSpeed(u8 SpeedSet)
{
	SPI2->CR1&=0XFFC7; 
	SPI2->CR1|=SpeedSet;
	SPI_Cmd(SPI2,ENABLE); 
} 

/*
 * ��������SPI1_SetSpeed
 * ����  ��SPI1�ٶ����ú���	
 * ����  ��SpeedSet:
					 SPI_BaudRatePrescaler_2   2��Ƶ   
					 SPI_BaudRatePrescaler_8   8��Ƶ   
					 SPI_BaudRatePrescaler_16  16��Ƶ  
					 SPI_BaudRatePrescaler_256 256��Ƶ 
 * ���  ����
 */	
void SPI1_SetSpeed(u8 SpeedSet)
{
	SPI1->CR1&=0XFFC7; 
	SPI1->CR1|=SpeedSet;
	SPI_Cmd(SPI1,ENABLE); 
} 

/*
 * ��������SPI2_ReadWriteByte
 * ����  ��SPIx ��дһ���ֽ�
 * ����  ��TxData:Ҫд����ֽ�
 * ���  ������ֵ:��ȡ�����ֽ�
 */	
u8 SPI2_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI_I2S_SendData(SPI2, TxData); //ͨ������SPIx����һ������
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET); //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI_I2S_ReceiveData(SPI2); //����ͨ��SPIx������յ�����					    
}

/*
 * ��������SPI1_ReadWriteByte
 * ����  ��SPIx ��дһ���ֽ�
 * ����  ��TxData:Ҫд����ֽ�
 * ���  ������ֵ:��ȡ�����ֽ�
 */
u8 SPI1_ReadWriteByte(u8 TxData)
{				   			 
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI_I2S_SendData(SPI1, TxData); //ͨ������SPIx����һ������
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����					    
}

void SPIx_WriteByte(u8 dat)
{
    u8 i;
    for(i=0; i<8; i++)
    {
        if(dat & 0x80)
            MOSI = 1;
        else
            MOSI = 0;
        delay_us(1);
        SCK = 1;
        delay_us(1);
        SCK  =0;
        dat=dat<<1;
				delay_us(1);
    }
}
void SPIx_WriteWord(u16 wrd)
{
    u8 i;
	  MOSI = 0;
    for(i=0; i<16; i++)
    {
        if(wrd & 0x8000)
            MOSI = 1;
        else
            MOSI = 0;
				delay_us(1);
        SCK = 1;
        delay_us(1);
        SCK = 0;
        wrd=wrd<<1;
				delay_us(1);
    }
}
u16 SPIx_ReadWord(void)
{
	u8  i;
	u16 tmp=0;
	MOSI = 0;
	MOSI_IN();	
	for(i=0; i<16; i++)
	{
        if(MOSI_DATA)
            tmp = 0x01|(tmp << 1);
        else
            tmp = tmp << 1;
				delay_us(1);
        SCK = 1;
        delay_us(1);
        SCK = 0;
				//tmp = tmp << 1;
				delay_us(1);
     }
	MOSI_OUT();
     return tmp;
}
u8 SPIx_ReadByte(void)
{
	u8 i,tmp=0;
	MOSI_IN();
	for(i=0; i<8; i++)
	{
        if(MOSI_DATA)
            tmp = 0x01|(tmp << 1);
        else
            tmp = tmp << 1;
				delay_us(1);
        SCK=1;
        delay_us(1);
        SCK = 0;
				//tmp = tmp << 1;
				delay_us(1);
     }
	MOSI_OUT();
	return tmp;
}

