#include "common.h"
#include "join.h"
const float CODE ChannelList[]={0,470.001f,472.501f,475.001f,477.501f,480.001f,482.501f,485.001f,487.501f,
                           490.001f,492.501f,495.001f,497.501f,500.001f,502.501f,505.001f,507.501f};
uint8 StartChannel = CHANNEL_1;
uint8 EndChannel = CHANNEL_2;
ChannelTable ScanChannel[CHANNEL_NUM];
JoinRequestPacketStruct JoinRequestPacket;
JoinRequestACKOKPacketStruct JoinRequestACKOKPacket;
uint8 ReJoinFlag = 0;

uint8 Scan_Channel(uint8 startch,uint8 endch)
{
    uint8 i;
    uint8 datalength = 0;  
    int16 timeout = SCAN_TIME_OUT;
    uint8 data[MAX_PACK_LENGTH];
       
    if(endch > CHANNEL_NUM)
    {
        return 0;
    }
    for(i=startch;i<endch+1;i++)   //ѭ��16��
    {
        A7139_SetFreq(ChannelList[i]);
        //A7139_SetFreq(470.001f);
        delay_us(1);
        A7139_Cal();                    //������Ƶ�ʺ�У׼
        delay_us(1);
        RXMode();
        while(GIO1)
        {
            delay_1ms();
            if(timeout--==0)
                break;
        }
        if(timeout>0)
        {
            A7139_ReadFIFO(&datalength,1);
            A7139_ReadFIFO(data,MAX_PACK_LENGTH);
            ScanChannel[i-1].channel_num = i;
            ScanChannel[i-1].channel_rssi = A7139_GetRSSI();
            ScanChannel[i-1].channel_free = data[6];
            ScanChannel[i-1].cluster_id = data[4];
        }
        else
        {
            ScanChannel[i-1].channel_num = 0;
            ScanChannel[i-1].channel_rssi = 0;
            ScanChannel[i-1].channel_free = 0;
            ScanChannel[i-1].cluster_id = 0;
        }
        timeout = SCAN_TIME_OUT; 
    }
        
    return i;
}

void bubble(ChannelTable *a,uint8 n) 
{ 
    uint8 i,j;
    ChannelTable temp; 
    for(i=0;i<n-1;i++) 
    {
        if(a[i].channel_free>MAX_DEVICE_NUM)
            continue;
        else
        {
            for(j=i+1;j<n;j++) 
                if(a[i].channel_rssi<a[j].channel_rssi) 
                { 
                    temp=a[i];
                    a[i]=a[j]; 
                    a[j]=temp; 
                }
        }
    }
    
} 

void SortChannel()
{
    bubble(ScanChannel,CHANNEL_NUM);
    EndPointDevice.channel = ScanChannel[0].channel_num;
    EndPointDevice.backup_channel = ScanChannel[1].channel_num;
    EndPointDevice.cluster_id = ScanChannel[0].cluster_id;
    EndPointDevice.des_cluster_id = ScanChannel[0].cluster_id;
    EndPointDevice.des_cluster_innernum = 0;
    A7139_SetFreq(ChannelList[EndPointDevice.channel]);
    delay_us(1);
    A7139_Cal();                    
    delay_us(1);
}

void CreatJoinRequest()
{
    JoinRequestPacket.length = JOINREQUEST_PACK_LENGTH;
    JoinRequestPacket.des_cluster_id = EndPointDevice.des_cluster_id;
    JoinRequestPacket.des_cluster_innernum = 0;
    JoinRequestPacket.phy_address = EndPointDevice.pyh_address;
    JoinRequestPacket.pack_type = JOINREQUEST_TYPE;
    JoinRequestPacket.ack_en = ACK_EN;
    
    DataSendBuffer[0] = JoinRequestPacket.length;
    DataSendBuffer[1] = JoinRequestPacket.pack_type<<2 | JoinRequestPacket.ack_en<<1;
    DataSendBuffer[2] = JoinRequestPacket.des_cluster_id;
    DataSendBuffer[3] = JoinRequestPacket.des_cluster_innernum;
    DataSendBuffer[4] = JoinRequestPacket.phy_address>>8;
    DataSendBuffer[5] = JoinRequestPacket.phy_address;
    DataSendBuffer[6] = 0;
    DataSendBuffer[7] = 0;
    DataSendBuffer[8] = 0;
    DataSendBuffer[9] = 0;
    DataSendBuffer[10] = 0;
    DataSendBuffer[11] = 0;
    
}

void SendJoinRequest()
{
    CreatJoinRequest();
    SendByCSMA(DataSendBuffer,MAX_PACK_LENGTH);
}

void CreatJoinRequestACKOK()
{
    JoinRequestACKOKPacket.length = JOINREQUESTACKOK_PACK_LENGTH;
    JoinRequestACKOKPacket.pack_type = JOINREQUESTACKOK_TYPE;
    JoinRequestACKOKPacket.ack_en = ACK_DIS;
    JoinRequestACKOKPacket.src_cluster_id = EndPointDevice.cluster_id;
    JoinRequestACKOKPacket.src_cluster_innernum = EndPointDevice.cluster_innernum;
    JoinRequestACKOKPacket.des_cluster_id = EndPointDevice.des_cluster_id;
    JoinRequestACKOKPacket.des_cluster_innernum = 0;
    
    DataSendBuffer[0] = JoinRequestACKOKPacket.length;
    DataSendBuffer[1] = JoinRequestACKOKPacket.pack_type<<2 | JoinRequestACKOKPacket.ack_en<<1;
    DataSendBuffer[2] = JoinRequestACKOKPacket.des_cluster_id;
    DataSendBuffer[3] = JoinRequestACKOKPacket.des_cluster_innernum;
    DataSendBuffer[4] = JoinRequestACKOKPacket.src_cluster_id;
    DataSendBuffer[5] = JoinRequestACKOKPacket.src_cluster_innernum;
    DataSendBuffer[6] = 0;
    DataSendBuffer[7] = 0;
    DataSendBuffer[8] = 0;
    DataSendBuffer[9] = 0;
    DataSendBuffer[10] = 0;
    DataSendBuffer[11] = 0;           
}

void JoinRequestACKHandler()
{
    uint8 accept = 0;
    uint8 ackok = 0;
    accept = DataRecvBuffer[1]&0x01;
    ackok = (DataRecvBuffer[1]&0x02)>>1;
    if((accept == 1)&&(ackok==1))                               //������������Ҫ�ظ�ACK-OK
    {
        EndPointDevice.cluster_id = DataRecvBuffer[6];
        EndPointDevice.cluster_innernum = DataRecvBuffer[7];
        EndPointDevice.connected = 1;
        CreatJoinRequestACKOK();
        SendPack();
        RXMode();
        if(EndPointDevice.power == 1)  //ִ�е͹���ģʽ
        {
            A7139_DeepSleep();
            DIS_INT;
        }
    }
    else if((accept == 1)&&(ackok==0))                          //��������������Ҫ�ظ�ACK-OK
    {
        EndPointDevice.cluster_id = DataRecvBuffer[6];
        EndPointDevice.cluster_innernum = DataRecvBuffer[7];
        EndPointDevice.connected = 1;
        if(EndPointDevice.power == 1)  //ִ�е͹���ģʽ
        {
            A7139_DeepSleep();
            DIS_INT;
        }
    }
    else
    {}
    
}
void ReJoinHandler()
{
    __disable_interrupt();
    A7139_Init(470.001f);
    Init_TQ();
    AD_cal(); 
    EndPointDevice.backup_channel = 0;
    EndPointDevice.channel = 0;
    EndPointDevice.cluster_id = 0;
    EndPointDevice.cluster_innernum = 0;
    EndPointDevice.connected = 0;
    EndPointDevice.csma_length = 0;
    EndPointDevice.data_ack = 0;
    EndPointDevice.des_cluster_id = 0;
    EndPointDevice.des_cluster_innernum = 0;
    EndPointDevice.device_type = 0;
    EndPointDevice.free_node = 0;
    EndPointDevice.power = 0;
    EndPointDevice.pyh_address = 0;
    EndPointDevice.state = 0;
    EndPointDevice.time_stamp = 0;
    for(int i=0;i<CHANNEL_NUM;i++)
    {
        ScanChannel[i].channel_free = 0;
        ScanChannel[i].channel_num = 0;
        ScanChannel[i].channel_rssi = 0;
        ScanChannel[i].cluster_id = 0;
    }
    ReJoinFlag = 1;
    
}