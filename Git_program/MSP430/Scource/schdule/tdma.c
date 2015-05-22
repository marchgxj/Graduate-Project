#include "common.h"
DataPacketStruct DataPacket;
uint16 ab_slot_num = 0;
uint32 send_time = 0;
uint32 resend_count = 0;
uint8 RecvDataACK()
{
    send_time = Frame_Time;
    while(GIO2==0)
    {
        if(Frame_Time>send_time+DATAACK_TIMEOUT)             //60ΪDataACK���ճ�ʱ�����յ�ACKʱ�������if��
        {
            //TIME1_LOW;
            resend_count++;
            EndPointDevice.data_ack = 0;
            RXMode();
            //TIME1_HIGH;
            PostTask(EVENT_CSMA_RESEND);
            return 0;
        }
        
            
    }
    
    A7139_ReadFIFO(DataRecvBuffer,MAX_PACK_LENGTH);
    RXMode();
    if(PackValid()&&(Unpack(DataRecvBuffer) == DATAACK_TYPE)) //����յ���ȷ��ACK
    {
        DataACKHandler();
        if(EndPointDevice.power == 1)
        {
            A7139_DeepSleep();
            DIS_INT;
        }
        return 1;
    }
    else                                                        //�������CSMA�׶�
    {
        PostTask(EVENT_CSMA_RESEND);
        EndPointDevice.data_ack = 0;
        return 0;
    }
        
       
}
void CreatSendData()
{
        
    DataPacket.pack_length = DATA_PACK_LENGTH;
    DataPacket.pack_type = DATA_TYPE;
    DataPacket.ack_en = ACK_EN;
    DataPacket.des_cluster_id = EndPointDevice.des_cluster_id;
    DataPacket.des_cluster_innernum = EndPointDevice.des_cluster_innernum;
    DataPacket.src_cluster_id = EndPointDevice.cluster_id;
    DataPacket.src_cluster_innernum = EndPointDevice.cluster_innernum;
    DataPacket.ab_slot_num = (uint16)resend_count>>8;
    //DataPacket.data = Car_Flag;
    DataPacket.data = (uint8)resend_count;
    
    DataSendBuffer[0] = DataPacket.pack_length;
    DataSendBuffer[1] = DataPacket.pack_type<<2|DataPacket.ack_en<<1;
    DataSendBuffer[2] = DataPacket.des_cluster_id;
    DataSendBuffer[3] = DataPacket.des_cluster_innernum;
    DataSendBuffer[4] = DataPacket.src_cluster_id;
    DataSendBuffer[5] = DataPacket.src_cluster_innernum;
    DataSendBuffer[6] = DataPacket.ab_slot_num>>8;
    DataSendBuffer[7] = DataPacket.ab_slot_num;
    DataSendBuffer[8] = DataPacket.data;
    DataSendBuffer[9] = 0;
    DataSendBuffer[10] = 0;
    DataSendBuffer[11] = 0;
}
 
    uint32 a,b,c;             //��ֹ��һ���ڵ�Ϊ��
    uint32 before_slot_wake = WAKE_TIME;
    uint8 ack_flag = 0;

void DataSend(void)
{

    //before_slot_wake = (((EndPointDevice.cluster_innernum-1)*SLOT_LENGTH)-WAKE_TIME)+5000;
    //Ϊʲôдһ��Ͳ��ԣ�����
    a = (EndPointDevice.cluster_innernum-1);
    b = a*SLOT_LENGTH;
    c = b + 5000;               //ÿ��ʱ϶�����5ms�����м��л�״̬
    before_slot_wake = (c-WAKE_TIME)/100;
    
    DIS_INT;
    while(Frame_Time<=before_slot_wake);
    EndPointDevice.data_ack = 0;
    //TIME1_LOW;

    A7139_Wake();

    CreatSendData();
    //TIME1_HIGH;
    SendPack();
    RXMode();
    ack_flag = RecvDataACK();
    if(ack_flag == 1)
    {
        A7139_Sleep();
    }
    else
    {
        EN_INT;
        
        //TIME1_HIGH;
        //delay_ms(5);
        //TIME1_LOW;
        //delay_ms(5);
        //TIME1_HIGH;
        
    }
    if((EndPointDevice.power == 0))
        EN_INT;
}
void DataACKHandler()
{
    EndPointDevice.time_stamp = DataSendBuffer[6]<<8|DataSendBuffer[7];
    EndPointDevice.data_ack = 1;
    //TIME1_LOW;
}
void CSMADataResend()
{
    SendByCSMA(DataSendBuffer,MAX_PACK_LENGTH);
    A7139_Sleep();
}