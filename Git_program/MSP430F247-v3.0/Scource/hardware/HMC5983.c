#include "common.h"
#define	SlaveAddress   0x3C
uint8 buffer[6] ;
uint8 ack=0;
uint8 const HMC_Config[3] = {0xFC,0x00,0x00};

void HMC_Start()
{
    SCL_H;   //SCL高
    delay_us(300);
    SDA_H;		//SDA高
    delay_us(300);
    SDA_L;    //SDA先拉低
    delay_us(300);
    SCL_L;		//SCL再拉低
    delay_us(300);
    
}

void HMC_Stop()
{
    SDA_L;		//SDA低
    delay_us(300);
    SCL_H;   //SCL先拉高
    delay_us(300);
    SDA_H;		//SDA再拉高
    delay_us(300);
    
}
void HMC_SendACK(unsigned char ack)
{
        if(ack)
    {
        SDA_H;
    }
    else
    {
        SDA_L;
    }

    delay_us(300);
    SCL_H; //SCL变为高
    delay_us(300);
    SCL_L;//SCL变为低
    delay_us(300);
    
}

unsigned char HMC_ReceiveACK()
{
    
    unsigned char ack;

    SCL_L; //SCL为低,由低变高读应答??????????????
    SDA_H; //接收模式先把SDA拉高
    delay_us(300);
    SDA_IN;//把SDA改变为输入
    delay_us(300);
                                               
    SCL_H;//SCL变为高
    delay_us(300);
    ack = SDA_val;
    delay_us(300);

    SDA_OUT;
    return ack;
    
}
void HMC_SendByte(unsigned char data)
{
    unsigned char i;

    for(i = 0; i < 8; i++)
    {
        if((data << i) & 0x80)
        {
            SDA_H;
            delay_us(300);
        }
        else
        {
            SDA_L;
            delay_us(300);
        }
        delay_us(300);
        SCL_H;
        delay_us(300);
        SCL_L;
    }
    delay_us(300);
    SDA_H;
    delay_us(300);
    SCL_H;
    delay_us(300);
    SDA_IN;
    delay_us(300);
    if(SDA_val)
    {
        ack = 0;
    }
    else
    {
        ack = 1;      /*判断是否接收到应答信号*/
    }
    delay_us(300);
    SDA_OUT;
    delay_us(300);
    SCL_L;
}
unsigned char HMC_ReceiveByte()
{	
    unsigned char i;
    unsigned char rec_byte = 0;
    delay_us(300);
    SDA_H; 
    delay_us(300);
    SDA_IN;
    delay_us(300);

    for(i = 0; i < 8; i++)
    {
        delay_us(300);
        SCL_L;
        delay_us(300);
        SCL_H;
        rec_byte <<= 1;
        if(SDA_val)
        {
            rec_byte += 0x01;
        }
        delay_us(300);
    }
    SCL_L;
    delay_us(300);
    SDA_OUT;
    delay_us(300);
    return rec_byte;
}

void Single_Write_HMC(unsigned char REG_Addr, unsigned char REG_data)
{							   
    HMC_Start();
    HMC_SendByte(0x3c);  //slave Address
    HMC_SendByte(REG_Addr);
    HMC_SendByte(REG_data);
    HMC_Stop();
}



unsigned char Single_Read_HMC(unsigned char REG_Addr)
{
    unsigned char data=0;
    HMC_Start();
    HMC_SendByte(0x3c);  //slave Address
    HMC_SendByte(REG_Addr);
    
    HMC_Start();
    HMC_SendByte(0x3c+1);  //slave Address
    HMC_SendByte(REG_Addr);
    data = HMC_ReceiveByte();
    HMC_SendACK(1);
    HMC_Stop();
    return data;
    
    
}

void single_read_data()
{
    int i;
    Single_Write_HMC(0x02,0x01);
    HMC_Start();
    HMC_SendByte(0x3c+1);  //slave Address
    //	HMC_SendByte(0x06);
    for( i=0;i<6;i++)
    {
        buffer[i] = HMC_ReceiveByte();
        if(i==5) 
            HMC_SendACK(1);
        else
            HMC_SendACK(0);	
    }
    
}

void Multi_Read_HMC()
{
    int i;
    HMC_Start();
    HMC_SendByte(0x3c);  //slave Address
    HMC_SendByte(0x03);	
    HMC_Start();
    HMC_SendByte(0x3c+1);  //slave Address
    //	HMC_SendByte(0x06);
    for( i=0;i<6;i++)
    {
        buffer[i] = HMC_ReceiveByte();
        if(i==5) 
            HMC_SendACK(1);
        else
            HMC_SendACK(0);	
    }
    
    
    
    
    HMC_Stop();
    delay_ms(5);
    
}







void Init_HMC(uint8* buffer)
{
    int i;
    
    Single_Write_HMC(0x00,HMC_Config[0]);
    Single_Write_HMC(0x01,HMC_Config[1]);   //  0x20  92nT   0x00  73nT   0x40 122nT
    Single_Write_HMC(0x02,HMC_Config[2]);
    delay_ms(6);
    
    HMC_Start();
    HMC_SendByte(0x3c);  //slave Address
    HMC_SendByte(0x00);
    
    HMC_Start();
    HMC_SendByte(0x3d);  //slave Address
    //HMC_SendByte(0x03);
    for( i=0;i<6;i++)
    {
        *buffer = HMC_ReceiveByte();
        buffer++;
        if(i==5) 
            HMC_SendACK(1);
        else
            HMC_SendACK(0);	
    }
    HMC_Stop();
    delay_ms(70);
	
    
}

void Init_5983()
{
    uint8 buffer[6];
    IRD_HIGH;
    Init_HMC(buffer);
    Init_HMC(buffer);
    if((buffer[0]!=HMC_Config[0])||(buffer[1]!=HMC_Config[1])||buffer[2]!=HMC_Config[2])
    {
        while(1)
        {
            halLedClear(3);
            halLedClear(4);
            halLedSet(1);
            halLedSet(2);
            delay_ms(500);
            halLedSet(3);
            halLedSet(4);
            halLedClear(1);
            halLedClear(2);
            delay_ms(500);
        }
    }
    
}
