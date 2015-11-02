#include "common.h"
DataPacketStruct DataPacket;
uint16 ab_slot_num = 0;
uint32 send_time = 0;
uint32 resend_count = 0;
uint16 DataAck_Lost = 0;
uint8 Int_Enable_Flag = 0;
uint8 RecvDataACK()
{
    send_time = 0;
    while(GIO2==0)
    {
        delay_100us();
        send_time++;
        if(send_time>DATAACK_TIMEOUT)             //60ΪDataACK���ճ�ʱ�����յ�ACKʱ�������if��
        {
            resend_count++;
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
void CreatSendData()
{
        
    DataPacket.pack_length = DATA_PACK_LENGTH;
    DataPacket.pack_type = DATA_TYPE;
    DataPacket.ack_en = ACK_EN;
    DataPacket.des_cluster_id = EndPointDevice.des_cluster_id;
    DataPacket.des_cluster_innernum = EndPointDevice.des_cluster_innernum;
    DataPacket.src_cluster_id = EndPointDevice.cluster_id;
    DataPacket.src_cluster_innernum = EndPointDevice.cluster_innernum;
    
    DataPacket.data = EndPointDevice.parking_state;
    //DataPacket.data = (uint8)Ave_Slop;
    
    DataSendBuffer[0] = DataPacket.pack_length;
    DataSendBuffer[1] = DataPacket.pack_type<<2|DataPacket.ack_en<<1;
    DataSendBuffer[2] = DataPacket.des_cluster_id;
    DataSendBuffer[3] = DataPacket.des_cluster_innernum;
    DataSendBuffer[4] = DataPacket.src_cluster_id;
    DataSendBuffer[5] = DataPacket.src_cluster_innernum;
    DataSendBuffer[6] = DataPacket.ab_slot_num>>8;
    DataSendBuffer[7] = DataPacket.ab_slot_num;
    //���ڵ͹���ģʽʱ���ʹ��������ݣ����ڻ��Ʊ仯����
    if(EndPointDevice.power == 0)
    {
//        DataSendBuffer[8] = Draw_DataX>>8;
//        DataSendBuffer[9] = Draw_DataX;
//        DataSendBuffer[10] = Draw_DataY>>8;
//        DataSendBuffer[11] = Draw_DataY;
//        PostTask(EVENT_COLLECT_DATA_F);
        /*DataSendBuffer[8] = VarianceX>>8;
        DataSendBuffer[9] = VarianceX;
        DataSendBuffer[10] = VarianceY>>8;
        DataSendBuffer[11] = VarianceY;
        PostTask(EVENT_GET_VARIANCE);*/
    }
    //�͹���ʱֻ����ʶ����
    else
    {
        DataSendBuffer[8] = DataPacket.data;
        DataSendBuffer[9] = 0;
        DataSendBuffer[10] = 0;
        DataSendBuffer[11] = GetCheck(DataSendBuffer);
    }

}

void DataSend(void)
{
    uint8 ack_flag = 0;
    uint32 a,b,c;             //��ֹ��һ���ڵ�Ϊ��
    uint32 before_slot_wake = WAKE_TIME;
    uint16 timeout = 0;
    //before_slot_wake = (((EndPointDevice.cluster_innernum-1)*SLOT_LENGTH)-WAKE_TIME)+5000;
    //Ϊʲôдһ��Ͳ��ԣ�����
    a = (EndPointDevice.cluster_innernum-1);
    b = a*SLOT_LENGTH;
    c = b + 5000;               //ÿ��ʱ϶�����5ms�����м��л�״̬
    before_slot_wake = (c-WAKE_TIME)/100;
    
    halLedClear(3);
    Int_Enable_Flag = 0;
    DIS_INT;
    
    while(Frame_Time<=before_slot_wake)
    {
        if(timeout>6000)
        {
            ReJoinFlag = 1;
            Exit_Sleep = 1;
            Init_TQ();
            return;
        }
        timeout++;
        delay_100us();
    }
    halLedClear(3);
    TIME2_HIGH;
    EndPointDevice.data_ack = 0;
    

    A7139_Wake();
    
    CreatSendData();
    SendPack();
    RXMode();

    ack_flag = RecvDataACK();
    if(ack_flag == 1)
    {      
        DataAck_Lost = 0;
        if(EndPointDevice.power == 1)
        {
            DIS_TIMER1;
            DIS_INT;
            A7139_DeepSleep();
        }
        else
        {
            A7139_Sleep();
        }       
        TIME2_LOW;      
    }
    else
    {
        TIME2_LOW;
        DataAck_Lost++;
        if(DataAck_Lost>5)
        {
            PostTask(EVENT_A7139_RESET);
            DataAck_Lost = 0;
        }
        PostTask(EVENT_CSMA_RESEND);
        EndPointDevice.data_ack = 0;
        halLedSet(3);
        Int_Enable_Flag = 1;
        EN_INT; 
    }

}

uint8 rejoin_flag = 0;
void DataACKHandler()
{
    if(Unpack(DataRecvBuffer)!=DATAACK_TYPE)
    {
        return;
    }
    EndPointDevice.time_stamp = DataRecvBuffer[6]<<8|DataRecvBuffer[7];
    EndPointDevice.data_ack = 1;
    EndPointDevice.cmd = DataRecvBuffer[8];
    DataPacket.ab_slot_num++;
    rejoin_flag = DataRecvBuffer[1]&0x01;
    if(rejoin_flag == 1)
    {
        PostTask(EVENT_REJOIN_HANDLER);
    }
    if(EndPointDevice.cmd!=0)
    {
        PostTask(EVENT_CMD_HANDLER);
        Start_Sleep_Flag = 0;
        return;
    }
#if (MCU_SLEEP_ENABLE == 1)
    else
    {
        Start_Sleep_Flag = 1;
        //PostTask(EVENT_MCUSLEEP_ENABLE);
    }
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
    Int_Enable_Flag = 1;
    EN_INT;
}