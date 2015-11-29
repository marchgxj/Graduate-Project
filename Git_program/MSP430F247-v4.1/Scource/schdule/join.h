#ifndef _JOIN_h_
#define _JOIN_h_
#include "common.h"
extern const float ChannelList[];

#define    CHANNEL_1     1
#define    CHANNEL_2     2
#define    CHANNEL_3     3
#define    CHANNEL_4     4
#define    CHANNEL_5     5
#define    CHANNEL_6     6
#define    CHANNEL_7     7
#define    CHANNEL_8     8
#define    CHANNEL_9     9
#define    CHANNEL_10    10
#define    CHANNEL_11    11
#define    CHANNEL_12    12
#define    CHANNEL_13    13
#define    CHANNEL_14    14
#define    CHANNEL_15    15
#define    CHANNEL_16    16
#define    CHANNEL_NUM   16



#define    SCAN_TIME_OUT     3000  //ɨ�賬ʱʱ��  ��λms
#define    SCAN_PERIOD   10000  //ɨ���ŵ����ڣ��ڵ�һ��ɨ��ʧ��ʱʹ�� ��λ��ms

typedef struct {
    uint8 channel_num;     // �ŵ����
    uint8 channel_rssi;    // �ŵ��ź�ǿ��
    uint8 channel_free;   // Sink���нڵ���
    uint8 cluster_id;     // ��ID
}ChannelTable;

typedef struct{
    uint8  length;                      //������
    uint8  des_cluster_id;              //Ŀ�Ĵ�ID
    uint8  des_cluster_innernum;        //Ŀ�Ĵ��ڱ��
    uint16 phy_address;                 //�����ַ
    uint8  pack_type:6;                 //������
    uint8  ack_en:1;                    //�Ƿ���ҪӦ��
    uint8  :1;                          //����
}JoinRequestPacketStruct;

typedef struct{
    uint8  length;                      //������
    uint8  pack_type:6;                 //������
    uint8  ack_en:1;                    //�Ƿ���ҪӦ��
    uint8  :1;                          //����
    uint8  src_cluster_id;              //Դ��ID
    uint8  src_cluster_innernum;        //Դ���ڱ��
    uint8  des_cluster_id;              //Ŀ�Ĵ�ID
    uint8  des_cluster_innernum;        //Ŀ�Ĵ��ڱ��

}JoinRequestACKOKPacketStruct;

extern uint8 StartChannel;
extern uint8 EndChannel;

extern ChannelTable SortedChannel[CHANNEL_NUM];
extern ChannelTable ScanChannel[CHANNEL_NUM];
extern uint8 ReJoinFlag;
extern uint8 RejoinChannel;

extern uint8 Scan_Channel(uint8 startch,uint8 endch);
extern void SortChannel(uint8 startch,uint8 endch);
extern void CreatJoinRequest();
extern void JoinRequestACKHandler();
extern void SendJoinRequest();
extern void ChannelSelection(uint8 start,uint8 end);
extern void ReJoinHandler();
extern void ChannelSelectionOnce(uint8 start,uint8 end);

#endif