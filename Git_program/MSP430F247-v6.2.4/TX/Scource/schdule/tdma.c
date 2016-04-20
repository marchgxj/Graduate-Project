//******************
// FileName: tdma.c
//
// Description:
//  TDMA schdule,send data,receive ack or something
//
// Author: 2016/1/19, by xiaoximi
//

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
        if(Frame_Time>send_time+DATAACK_TIMEOUT)             //60为DataACK接收超时，在收到ACK时不会进入if内
        {
            resend_count++;
            EndPointDevice.data_ack = 0;
            RXMode();
           //test by wyd PostTask(EVENT_CSMA_RESEND);
            return EndPointDevice.data_ack;
        }
        
            
    }
    
    A7139_ReadFIFO(DataRecvBuffer,MAX_PACK_LENGTH);
    RXMode();
    if(PackValid()&&(Unpack(DataRecvBuffer) == DATAACK_TYPE)) //如果收到正确的ACK
    {
        DataACKHandler();
    }
    else                                                        //否则进入CSMA阶段
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
    //不在低功耗模式时发送传感器数据，用于绘制变化曲线
    if(EndPointDevice.power == 0)
    {
        DataSendBuffer[8] = Draw_DataX>>8;
        DataSendBuffer[9] = Draw_DataX;
        DataSendBuffer[10] = Draw_DataY>>8;
        DataSendBuffer[11] = Draw_DataY;
        PostTask(EVENT_COLLECT_DATA_F);
        /*DataSendBuffer[8] = VarianceX>>8;
        DataSendBuffer[9] = VarianceX;
        DataSendBuffer[10] = VarianceY>>8;
        DataSendBuffer[11] = VarianceY;
        PostTask(EVENT_GET_VARIANCE);*/
    }
    //低功耗时只发送识别结果
    else
    {
        DataSendBuffer[8] = DataPacket.data;
        DataSendBuffer[9] = 0;
        DataSendBuffer[10] = 0;
        DataSendBuffer[11] = 0;
    }

}
 
    


void DataSend(void)
{
    uint8 ack_flag = 0;
    uint32 a,b,c;             //防止第一个节点为负
    uint32 before_slot_wake = WAKE_TIME;
    //before_slot_wake = (((EndPointDevice.cluster_innernum-1)*SLOT_LENGTH)-WAKE_TIME)+5000;
    //为什么写一起就不对！！！
    a = (EndPointDevice.cluster_innernum-1);
    b = a*SLOT_LENGTH;
    c = b + 5000;               //每个时隙向后移5ms，让中继切换状态
    before_slot_wake = (c-WAKE_TIME)/100;
    
    DIS_INT;
    
    while(Frame_Time<=before_slot_wake);
    TIME2_HIGH;
    EndPointDevice.data_ack = 0;
    

    A7139_Wake();
    
    CreatSendData();
    SendPack();
    RXMode();

    ack_flag = RecvDataACK();
    if(ack_flag == 1)
    {      
        if(EndPointDevice.power == 1)
        {
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
        PostTask(EVENT_CSMA_RESEND);
        EndPointDevice.data_ack = 0;
        EN_INT; 
    }

}
uint8 rejoin_flag = 0;
void DataACKHandler()
{

    EndPointDevice.time_stamp = DataRecvBuffer[6]<<8|DataRecvBuffer[7];
    EndPointDevice.data_ack = 1;
    DataPacket.ab_slot_num++;
    rejoin_flag = DataRecvBuffer[1]&0x01;
    if(rejoin_flag == 1)
    {
        PostTask(EVENT_REJOIN_HANDLER);
    }
    
}
void CSMADataResend()
{
    SendByCSMA(DataSendBuffer,MAX_PACK_LENGTH);
    A7139_Sleep();
    TIME2_HIGH;
}