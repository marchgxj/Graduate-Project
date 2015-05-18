#include "common.h"
EndPointDeviceStruct EndPointDevice;
uint8   DataRecvBuffer[MAX_PACK_LENGTH];
uint8   DataSendBuffer[MAX_PACK_LENGTH];

uint16  CSMA_BackOff_Random = 0;
uint8   CSMA_BackOff_Count = 0;

uint8   Power_Mode = 2;  //1:ÿ��ʱ϶������ʱ����   2�����ݱ仯ʱ���ѷ��ͣ�����ʱ������
uint8   Data_Change_Flag = 0;//���ݷ����仯ʱ  ��λ��һ
int16 AD_Value;
int AD_middle_value = 0;
uint8 Car_Flag = 0;
uint8 Car_Flag_Memory = 0;
uint8 Unpack(uint8 *type)
{
    type++;
    return (*type&0xFC)>>2;
}

void CSMABackOff()
{
    uint32 back_off_time = 0;
//    uint32 wait_time = 0;
//    uint32 q,w;
//    //wait_time = ((MAX_DEVICE_NUM - EndPointDevice.free_node)*SLOT_LENGTH)/10;
//    q=MAX_DEVICE_NUM - EndPointDevice.free_node;
//    w=q*SLOT_LENGTH;
//    wait_time=w/100;
//    
//    while(Frame_Time<=wait_time);      //�ȴ�����CSMAʱ϶ ��֡��ʱ��<=������*ʱ϶����
//    
    CSMA_BackOff_Random = (rand()*TAR)%(EndPointDevice.csma_length*BACKOFF_DIV);        //�˱�ʱ��   �����*TIMERA����%csmaʱ϶����/10
    back_off_time = BACKOFF_PERIOD*CSMA_BackOff_Random;
    delayus(back_off_time);
}

uint8 SendByCSMA(u8 *buff,uint8 length)
{
    uint8 cca_value = 0;
    uint32 wait_time = 0;
    uint32 q,w;
    //wait_time = ((MAX_DEVICE_NUM - EndPointDevice.free_node)*SLOT_LENGTH)/10;
    q=MAX_DEVICE_NUM - EndPointDevice.free_node;
    w=q*SLOT_LENGTH;
    wait_time=w/100;
    
    while(Frame_Time<=wait_time);      //�ȴ�����CSMAʱ϶ ��֡��ʱ��<=������*ʱ϶����
    //TIME1_LOW;
    CSMABackOff();
    //TIME1_HIGH;
    cca_value = (A7139_GetRSSI()+A7139_GetRSSI())/2;
            
    if(cca_value<CCA)
    {
        SendPack();
        RXMode();
    }
    else
    {
        CSMABackOff();
        CSMA_BackOff_Count++;
    }

    if(CSMA_BackOff_Count>CSMA_BACKOFF_TIME)
        return CSMA_FAIL;
    else
        return CSMA_SUCCESS;

}

void BeaconHandler(uint8 beacon[])
{
    EndPointDevice.free_node = beacon[6];
    EndPointDevice.csma_length = (MAX_DEVICE_NUM - EndPointDevice.free_node)/3+1;
    EndPointDevice.power = beacon[1]&0x01;
    if(EndPointDevice.connected == 0)                   //δ���ӣ����ͼ�������
    {
        PostTask(EVENT_JOINREQUEST_SEND);
    }
    else                                                //�����ӣ�ִ��TDMA����
    {
        //TIME1_HIGH;
        if(EndPointDevice.power == 0)
        {
            A7139_Sleep();
        }
        
        PostTask(EVENT_DATA_SEND);
    }
}

uint8 PackValid(void)
{
    uint16 phy_address = 0;
    phy_address = DataRecvBuffer[2]<<8|DataRecvBuffer[3];
    if((DataRecvBuffer[2]==EndPointDevice.cluster_id&&
        DataRecvBuffer[3]==EndPointDevice.cluster_innernum)||
       (DataRecvBuffer[2]==BROADCAST&&
        DataRecvBuffer[3]==BROADCAST)||
       (phy_address==EndPointDevice.pyh_address))
        return 1;
    else
        return 0;
}

void CollectData()
{
//    AD_Value=SampleChannel(0x02);
//    if(AD_middle_value-AD_Value>50)
//    {
//        halLedSet(1);//NKJFK-GPHP7-G8C3J-P6JXR-HQRJR
//        Car_Flag = 1;
//    }
//    else if(AD_Value-AD_middle_value>50)
//    {
//        halLedSet(1);//NKJFK-GPHP7-G8C3J-P6JXR-HQRJR
//        Car_Flag = 1;
//    }
//    else
//    {
//        halLedClear(1);
//        Car_Flag = 0;
//    }
//    if(Car_Flag_Memory!=Car_Flag)
//    {
//        Data_Change_Flag = 1;
//        PostTask(EVENT_DATA_SEND);
//    }
//    else
//    {
//        Data_Change_Flag = 0;
//    }
//    Car_Flag_Memory = Car_Flag;
    
    if(Data_Change_Flag == 1)
    {
        Data_Change_Flag = 0;
        A7139_Deep_Wake();
        EN_INT;
    }
}