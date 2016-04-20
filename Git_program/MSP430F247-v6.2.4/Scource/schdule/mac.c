#include "common.h"
EndPointDeviceStruct EndPointDevice;
uint8   DataRecvBuffer[MAX_PACK_LENGTH];
uint8   DataSendBuffer[MAX_PACK_LENGTH];

uint16  CSMA_BackOff_Random = 0;
uint8   CSMA_BackOff_Count = 0;

uint8   Power_Mode = 2;  //1:ÿ��ʱ϶������ʱ����   2�����ݱ仯ʱ���ѷ��ͣ�����ʱ������
uint8   Data_Change_Flag = 0;//���ݷ����仯ʱ  ��λ��һ
    
uint8 GetCheck(uint8* buf)
{

    uint8 i=0;
    uint8 checksum=0;
    for(i=0;i<MAX_PACK_LENGTH-1;i++)
    {
        checksum+=*buf++;
    }
    return 0xFF-checksum;
    
}
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
    uint8 cca_value = 0;
    uint32 wait_time = 0;
    uint32 q,w;
    
uint8 SendByCSMA(u8 *buff,uint8 length)
{
    uint32 timeout = 0;
    //wait_time = ((MAX_DEVICE_NUM - EndPointDevice.free_node)*SLOT_LENGTH)/10;
    q=MAX_DEVICE_NUM - EndPointDevice.free_node;
    w=q*SLOT_LENGTH;
    wait_time=w/100;
    
    EN_TIMER1;
    while(Frame_Time<=wait_time)     //�ȴ�����CSMAʱ϶ ��֡��ʱ��<=������*ʱ϶����
    {
        if(timeout>(uint32)WHILE_TIMEOUT)
        {
            
            ReJoinFlag = 1;
            Exit_Sleep = 1;
            Init_TQ();
            DIS_TIMER1;
            return CSMA_FAIL;
        }
        
        timeout++;
        delay_100us();
    }
    DIS_TIMER1;
    CSMABackOff();
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
        //halLedToggle(CSMA_BackOff_Count%5);
        
    }

    if(CSMA_BackOff_Count>CSMA_BACKOFF_TIME)
    {
        CSMA_BackOff_Count = 0;
        return CSMA_FAIL;
    } 
    else
    {
        return CSMA_SUCCESS;
    }
        
}

void BeaconHandler(uint8 beacon[])
{
    if(Unpack(beacon)!=BEACON_TYPE)
    {
        return;
    }
    //TIME2_HIGH;
    EndPointDevice.free_node = beacon[6];
    EndPointDevice.csma_length = (MAX_DEVICE_NUM - EndPointDevice.free_node)/3+1;
    EndPointDevice.power =  1;   
    //EndPointDevice.power = 0;
    if(EndPointDevice.connected == 0)                   //δ���ӣ����ͼ�������
    {
        PostTask(NULL,EVENT_JOINREQUEST_SEND);
    }
    else                                                //�����ӣ�ִ��TDMA����
    {
        if(EndPointDevice.power == 0)//�����Լ�����ʱ϶����˯��
        {
            A7139_Sleep(); 
        }

        PostTask(NULL,EVENT_DATA_SEND);
    }
    //TIME2_LOW;
    
}
uint8 valid = 0;
uint8 PackValid(uint8 data[])
{
    uint16 phy_address = 0;
    if(data[0]==0)
    {
        return 0;
    }
    phy_address = data[2]<<8|data[3];
    if((data[2]==EndPointDevice.cluster_id&&data[3]==EndPointDevice.cluster_innernum)||
       (data[2]==BROADCAST&&data[3]==BROADCAST)||
       (phy_address==EndPointDevice.pyh_address)||
       (data[2]==EndPointDevice.cluster_id && data[3]==BROADCAST))
    {
        if(GetCheck(data)==data[11])
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
        return 0;
}

