#ifndef _SCAN_h_
#define _SCAN_h_
#include "common.h"
#define SEND_REJOIN_EN 0		//���м������ϵ���Ƿ��ýڵ����·��ͼ�������


typedef struct{
		uint8  length;											//������
		uint8  pack_type:6;									//������
		uint8  ack_en:1;                    //�Ƿ���ҪӦ��
    uint8  accept:1;                		//�Ƿ��������    
		uint16 des_phy_address;             //Ŀ�ĵ�ַ
		uint8  src_cluster_id;              //Դ��ID
    uint8  src_cluster_innernum;        //Դ���ڱ��
		uint8  cluster_id;              		//�����ID
    uint8  cluster_innernum;       		  //������ڱ��
}JoinRequestACKPacketStruct;

typedef struct{
    uint8  length;                      //������
		uint8  pack_type:6;                 //������
    uint8  ack_en:1;                    //�Ƿ���ҪӦ��
    uint8  :1;                          //����
    uint8  des_cluster_id;              //Ŀ�Ĵ�ID
    uint8  des_cluster_innernum;        //Ŀ�Ĵ��ڱ��
		uint8  src_cluster_id;							//Դ��ID
		uint8  src_cluster_innernum; 				//Դ���ڱ��
    uint16 des_phy_address;                 //Ŀ�������ַ
}ReJoinPacketStruct;

extern void JoinRequestHandler(u8 buf[]);
extern void JoinRequestACKOKHandler(u8 buf[]);
extern void SendReJoin(void);

#endif
