#ifndef _TDMA_h_
#define _TDMA_h_
#include "common.h"

typedef struct{
    uint8  pack_length;                  //������
    uint8  pack_type:6;                 //������
    uint8  ack_en:1;                    //�Ƿ���ҪӦ��
    uint8  :1;                          //����
    uint8  des_cluster_id;              //Ŀ�Ĵ�ID
    uint8  des_cluster_innernum;        //Ŀ�Ĵ��ڱ��
    uint8  src_cluster_id;              //Դ��ID
    uint8  src_cluster_innernum;        //Դ���ڱ��
    uint16 ab_slot_num;                 //����ʱ϶��
    uint8  data;                        //����
    uint16 acklost_count;	        //ACK ��ʧ����
}DataPacketStruct;

extern DataPacketStruct DataPacket;
extern uint16 Draw_DataX;          
extern uint16 Draw_DataY;
extern uint8 Int_Enable_Flag;
extern uint16 BeaconComing_Count;
extern uint8 Data_Send_Waiting_Flag;

extern void DataSend(void);
extern void DataACKHandler();
extern void CSMADataResend();
extern void KeepAliveSend();
#endif