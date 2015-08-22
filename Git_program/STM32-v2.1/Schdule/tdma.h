#ifndef _TDMA_h_
#define _TDMA_h_
#include "common.h"

typedef struct{
		uint8 pack_length;                  //������
	  uint8  pack_type:6;                 //������
    uint8  ack_en:1;                    //�Ƿ���ҪӦ��
    uint8  rejoin:1;                    //��������
	  uint8  des_cluster_id;              //Ŀ�Ĵ�ID
    uint8  des_cluster_innernum;        //Ŀ�Ĵ��ڱ��
    uint8  src_cluster_id;              //Դ��ID
    uint8  src_cluster_innernum;        //Դ���ڱ��
	  uint16  time_stamp;									//ʱ���
}DataACKPacketStruct;

extern void DataHandler(void);

extern uint16 Draw_DataX;
extern uint16 Draw_DataY;
#endif
