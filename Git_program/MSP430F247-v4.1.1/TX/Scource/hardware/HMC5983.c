#include "common.h"
#define	SlaveAddress   0x3C

uint8 ack=0;
uint8 const HMC_Config[3] = {0xFC,0x00,0x01};

//void HMC_Start()
//{
//    SCL_H;   //SCL高
//    delay_300us();
//    SDA_H;		//SDA高
//    delay_300us();
//    SDA_L;    //SDA先拉低
//    delay_300us();
//    SCL_L;		//SCL再拉低
//    delay_300us();
//    
//}
void HMC_Start()
{
    SDA_H;                    //拉高数据线
    SCL_H;                    //拉高时钟线
    delay_5us();                 //延时
    SDA_L;                    //产生下降沿
    delay_5us();                 //延时
    SCL_L;                    //拉低时钟线
    
}
//void HMC_Stop()
//{
//    SDA_L;		//SDA低
//    delay_300us();
//    SCL_H;   //SCL先拉高
//    delay_300us();
//    SDA_H;		//SDA再拉高
//    delay_300us();
//    
//}

void HMC_Stop()
{
    SDA_L;                    //拉低数据线
    SCL_H;                    //拉高时钟线
    delay_5us();                 //延时
    SDA_H;                    //产生上升沿
    delay_5us();                 //延时
    
}

//void HMC_SendACK(unsigned char ack)
//{
//        if(ack)
//    {
//        SDA_H;
//    }
//    else
//    {
//        SDA_L;
//    }
//
//    delay_300us();
//    SCL_H; //SCL变为高
//    delay_300us();
//    SCL_L;//SCL变为低
//    delay_300us();
//    
//}


void HMC_SendACK(unsigned char ack)
{
    if (ack==0)
    {
        SDA_L;                  //写应答信号
        SCL_H;                    //拉高时钟线
        delay_5us();                 //延时
        SCL_L;                    //拉低时钟线
        delay_5us();                 //延时
    }
    else
    {
        SDA_H;                  //写应答信号
        SCL_H;                    //拉高时钟线
        delay_5us();                 //延时
        SCL_L;                    //拉低时钟线
        delay_5us();                 //延时
    }
    
}
//unsigned char HMC_ReceiveACK()
//{
//    
//    unsigned char ack;
//
//    SCL_L; //SCL为低,由低变高读应答??????????????
//    SDA_H; //接收模式先把SDA拉高
//    delay_300us();
//    SDA_in;//把SDA改变为输入
//    delay_300us();
//                                               
//    SCL_H;//SCL变为高
//    delay_300us();
//    ack = SDA_val;
//    delay_300us();
//
//    SDA_out;
//    return ack;
//    
//}

unsigned char HMC_ReceiveACK()
{
    uint8 a;   
    SDA_in;
    
    SCL_H;                    //拉高时钟线
    delay_5us();                 //延时
    
    a = SDA_val;                 //读应答信号
    SCL_L;                    //拉低时钟线
    delay_5us();                 //延时
    SDA_out;

    return a;
    
}
//void HMC_SendByte(unsigned char data)
//{
//    unsigned char i;
//
//    for(i = 0; i < 8; i++)
//    {
//        if((data << i) & 0x80)
//        {
//            SDA_H;
//            delay_300us();
//        }
//        else
//        {
//            SDA_L;
//            delay_300us();
//        }
//        delay_300us();
//        SCL_H;
//        delay_300us();
//        SCL_L;
//    }
//    delay_300us();
//    SDA_H;
//    delay_300us();
//    SCL_H;
//    delay_300us();
//    SDA_in;
//    delay_300us();
//    if(SDA_val)
//    {
//        ack = 0;
//    }
//    else
//    {
//        ack = 1;      /*判断是否接收到应答信号*/
//    }
//    delay_300us();
//    SDA_out;
//    delay_300us();
//    SCL_L;
//}

void HMC_SendByte(unsigned char dat)
{
    uint8 i;

    for(i = 0;i < 8;i++)//8位计数器
    {
      
       if(dat & 0x80)//1
            SDA_H;            
        else 			    
            SDA_L;
        SCL_H;
        delay_5us();
        SCL_L;
        delay_5us();
        dat <<= 1;
    }    
    HMC_ReceiveACK();  
}
//unsigned char HMC_ReceiveByte()
//{	
//    unsigned char i;
//    unsigned char rec_byte = 0;
//    delay_300us();
//    SDA_H; 
//    delay_300us();
//    SDA_in;
//    delay_300us();
//
//    for(i = 0; i < 8; i++)
//    {
//        delay_300us();
//        SCL_L;
//        delay_300us();
//        SCL_H;
//        rec_byte <<= 1;
//        if(SDA_val)
//        {
//            rec_byte += 0x01;
//        }
//        delay_300us();
//    }
//    SCL_L;
//    delay_300us();
//    SDA_out;
//    delay_300us();
//    return rec_byte;
//}

unsigned char HMC_ReceiveByte()
{	
    int i=8;
    unsigned char ReceiveByte=0;
    SDA_H;
    SDA_in;
    while(i--)
    {
        ReceiveByte<<=1;      
        SCL_L;
        
        delay_5us();
        SCL_H;
        delay_5us();
      	if(SDA_val)
       	{
       		ReceiveByte|=0x01;
       	}
        
        SCL_L;
     }
      SDA_out;
    return ReceiveByte;
    

}
uint8 addr = 0;
void Single_Write_HMC(unsigned char REG_Addr, unsigned char REG_data)
{							   
    HMC_Start();
    HMC_SendByte(0x3c);  //slave Address
    addr = REG_Addr;
    HMC_SendByte(REG_Addr);
    HMC_SendByte(REG_data);
    HMC_Stop();
}



void Single_Read_HMC(uint16* XValue,uint16* YValue,uint16* ZValue)
{
    int xvalue = 0;
    int yvalue = 0;
    int zvalue = 0;
    uint8 buffer[6];
    uint8 i = 0;
    Single_Write_HMC(0x02,0x01);
    delay_ms(15);
    HMC_Start();
    HMC_SendByte(0x3c);  //slave Address
    HMC_SendByte(0x03);	
    HMC_Start();
    HMC_SendByte(0x3c+1);  //slave Address
//    HMC_SendByte(0x06);
    for( i=0;i<6;i++)
    {
        buffer[i] = HMC_ReceiveByte();
        if(i==5) 
            HMC_SendACK(1);
        else
            HMC_SendACK(0);	
    }
    HMC_Stop();
//    HMC_SendByte(0x3c);  //slave Address
//    HMC_SendByte(0x03);
//    HMC_Start();
//    HMC_SendByte(0x3c+1);  //slave Address
////    HMC_SendByte(REG_Addr);
//    data = HMC_ReceiveByte();
//    HMC_SendACK(1);
//    HMC_Stop();
    xvalue = (buffer[0]<<8)|buffer[1];
    zvalue = (buffer[2]<<8)|buffer[3];
    yvalue = (buffer[4]<<8)|buffer[5];
    if(xvalue!=-4096)
    {
        *XValue = xvalue + 2048;
    }
    else
    {
        *XValue = 0;
    }
    if(yvalue!=-4096)
    {
        *YValue = yvalue + 2048;
    }
    else
    {
        *YValue = 0;
    }
    if(zvalue!=-4096)
    {
        *ZValue = zvalue + 2048;
    }
    else
    {
        *ZValue = 0;
    }
    
    
}

void ResetHMC(uint8 dir)
{
    
    Single_Write_HMC(0x00,0x71);
    Single_Write_HMC(0x01,0xA0);
    Single_Write_HMC(0x02,0x00);
    delay_ms(100);
    Single_Write_HMC(0x00,0x72);
    Single_Write_HMC(0x01,0xA0);
    Single_Write_HMC(0x02,0x00);
    delay_ms(100);
    Single_Write_HMC(0x00,HMC_Config[0]);
    Single_Write_HMC(0x01,HMC_Config[1]);
    Single_Write_HMC(0x02,HMC_Config[2]);
    
    
}
    
void Multi_Read_HMC(uint16* XValue,uint16* YValue,uint16* ZValue)
{
    
    int i;
    int xvalue = 0;
    int yvalue = 0;
    int zvalue = 0;
    uint8 buffer[6];
    Single_Write_HMC(0x02,0x01);
    delay_1ms();delay_1ms();delay_1ms();delay_1ms();delay_1ms();

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
    
    
    xvalue = (buffer[0]<<8)|buffer[1];
    zvalue = (buffer[2]<<8)|buffer[3];
    yvalue = (buffer[4]<<8)|buffer[5];
    if(xvalue!=-4096)
    {
        *XValue = xvalue + 2048;
    }
    else
    {
        *XValue = 0;
    }
    if(zvalue!=-4096)
    {
        *YValue = zvalue + 2048;
    }
    else
    {
        *YValue = 0;
    }
    if(yvalue!=-4096)
    {
        *ZValue = yvalue + 2048;
    }
    else
    {
        *ZValue = 0;
    }
}







void Init_HMC(uint8* buffer)
{
    int i;
    Single_Write_HMC(0x00,HMC_Config[0]);       //第一次发送没有ack
    Single_Write_HMC(0x00,HMC_Config[0]);
    Single_Write_HMC(0x01,HMC_Config[1]);   //  0x20  92nT   0x00  73nT   0x40 122nT
    Single_Write_HMC(0x02,HMC_Config[2]);
    
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
    delay_ms(1000);
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
void HMC5983_cal()
{
//    int i=0;
//    uint16 ADvalueX=0,ADvalueY=0;
//    uint16 ADX,ADY;
//    uint32 intensity = 0;
//    
//    halLedSetAll();
//    delay_ms(2000);
//    
//    ADX = 0;
//    ADY = 0;
//    for(i=0;i<26;i++)
//    {
//        if(i>=10)
//        {
//            Multi_Read_HMC(&ADvalueX,&ADvalueY);
//            ADX += ADvalueX;
//            ADY += ADvalueY;
//            delay_ms(50);
//        }
//    }
//    MagneticUnit.XMiddle = ADX>>4;
//    MagneticUnit.YMiddle = ADY>>4;
//    MagneticUnit.XMiddleM = MagneticUnit.XMiddle;
//    MagneticUnit.YMiddleM = MagneticUnit.YMiddle;
//    MagneticUnit.Ext_Middle = abs(MagneticUnit.XMiddle-MagneticUnit.YMiddle);
//    
//    intensity = sqrt_16((((uint32)MagneticUnit.XMiddle*(uint32)MagneticUnit.XMiddle)+((uint32)MagneticUnit.YMiddle*(uint32)MagneticUnit.YMiddle)));
//    MagneticUnit.Int_Middle = intensity;
//    
//    for(int i=0;i<FILTER_LENGTH;i++)
//    {
//        Multi_Read_HMC(&ADvalueX,&ADvalueY);
//        FilterData[i].xvalue = ADvalueX;
//        FilterData[i].yvalue = ADvalueY;
//        delay_ms(50);
//    }
//    for(int i=0;i<SLOP_LENGTH;i++)
//    {
//        Multi_Read_HMC(&ADvalueX,&ADvalueY);
//        SlopData[i].xvalue = ADvalueX;
//        SlopData[i].yvalue = ADvalueY;
//        delay_ms(50);
//    }
//  
//
//    //NoCarCalibration();
//    halLedClearAll();
//    delay_ms(50);
//    halLedSetAll();
//    delay_ms(50);
//    halLedClearAll();
//    delay_ms(50);
//    halLedSetAll();
//    delay_ms(50);
//    halLedClearAll();
//    delay_ms(50);
}
