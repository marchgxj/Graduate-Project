#ifndef _SCAN_h_
#define _SCAN_h_
#include "common.h"

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

extern void JoinRequestHandler(void);
extern void JoinRequestACKOKHandler(void);

#endif
