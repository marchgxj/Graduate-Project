#include "common.h"
DataPacketStruct DataPacket;
uint16 ab_slot_num = 0;
uint32 send_time = 0;
uint32 resend_count = 0;
uint16 DataAck_Lost = 0;
uint8 RecvDataACK()
{
    send_time = 0;
    while(GIO2==0)
    {
        if(send_time<DATAACK_TIMEOUT)             //60ΪDataACK���ճ�ʱ�����յ�ACKʱ�������if��
        {
            send_time++;
            delay_100us();
            

            
        }
        else
        {
            EndPointDevice.data_ack = 0;
            RXMode();
            return EndPointDevice.data_ack;
        }
        
            
    }
    
    A7139_ReadFIFO(DataRecvBuffer,MAX_PACK_LENGTH);
    RXMode();
    if(PackValid()&&(Unpack(DataRecvBuffer) == DATAACK_TYPE)) //����յ���ȷ��ACK
    {
        DataACKHandler();
    }
    else                                                        //�������CSMA�׶�
    {
        EndPointDevice.data_ack = 0;
    }
    return EndPointDevice.data_ack;
        
       
}
uint16 PacketSend_Count = 0;
void CreatSendData()
{

    PacketSend_Count++;
    DataSendBuffer[0] = 12;
    DataSendBuffer[1] = 5<<2;
    DataSendBuffer[2] = 0xFF;
    DataSendBuffer[3] = 0XFF;
    DataSendBuffer[4] = 1;   //QOS ����
    DataSendBuffer[5] = PacketSend_Count>>8;
    DataSendBuffer[6] = PacketSend_Count;
    DataSendBuffer[7] = AckLost_Count>>8;
    DataSendBuffer[8] = AckLost_Count;
    DataSendBuffer[9] = 0;
    DataSendBuffer[10] = 0;
    DataSendBuffer[11] = 0;

}
uint16 AckLost_Count = 0;
void DataSend(void)
{
    uint8 ack_flag = 0;
    uint32 a,b,c;             //��ֹ��һ���ڵ�Ϊ��
    uint32 before_slot_wake = WAKE_TIME;
    //before_slot_wake = (((EndPointDevice.cluster_innernum-1)*SLOT_LENGTH)-WAKE_TIME)+5000;
    //Ϊʲôдһ��Ͳ��ԣ�����
    a = (EndPointDevice.cluster_innernum-1);
    b = a*SLOT_LENGTH;
    //c = b + 5000;               //ÿ��ʱ϶�����5ms�����м��л�״̬
    c = 5000;
    before_slot_wake = (c-WAKE_TIME)/100;
    
    halLedClear(3);
    DIS_INT;
    
    TIME2_HIGH;
    EndPointDevice.data_ack = 0;
    

    A7139_StrobeCmd(CMD_STBY);
    delay_ms(2);
    A7139_Init(482.001f);
    delay_ms(2);
    RXMode();
    delay_ms(2);
    
    CreatSendData();
    SendPack();
    RXMode();

    ack_flag = RecvDataACK();
    if(ack_flag == 1)
    {      
        DataAck_Lost = 0;
        
        A7139_DeepSleep();
        TIME2_LOW;
        halLedToggle(2);
        
    }
    else
    {
        AckLost_Count++;
        //EN_INT; 
    }
    halLedToggle(1);

}

uint8 rejoin_flag = 0;
void DataACKHandler()
{
    EndPointDevice.time_stamp = DataRecvBuffer[6]<<8|DataRecvBuffer[7];
    EndPointDevice.data_ack = 1;
    EndPointDevice.cmd = DataRecvBuffer[8];
    DataPacket.ab_slot_num++;
    rejoin_flag = DataRecvBuffer[1]&0x01;
    
    
#if (MCU_SLEEP_ENABLE == 1)
    
    PostTask(EVENT_MCUSLEEP_ENABLE);
    
#endif
    
    
}

void CSMADataResend()
{
    SendByCSMA(DataSendBuffer,MAX_PACK_LENGTH);
    A7139_Sleep();
    TIME2_HIGH;
}

void KeepAliveSend()
{
    A7139_Deep_Wake();
    halLedSet(3);
    EN_INT;
    EN_TIMER1;
}
