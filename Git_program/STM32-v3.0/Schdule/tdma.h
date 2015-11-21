#ifndef _TDMA_h_
#define _TDMA_h_
#include "common.h"

typedef struct{
		uint8  pack_length;                  //������
	  uint8  pack_type:6;                 //������
    uint8  ack_en:1;                    //�Ƿ���ҪӦ��
    uint8  rejoin:1;                    //��������
	  uint8  des_cluster_id;              //Ŀ�Ĵ�ID
    uint8  des_cluster_innernum;        //Ŀ�Ĵ��ڱ��
    uint8  src_cluster_id;              //Դ��ID
    uint8  src_cluster_innernum;        //Դ���ڱ��
	  uint16  time_stamp;									//ʱ���
	  uint8  cmd;													//��������
}DataACKPacketStruct;
extern DataACKPacketStruct DataACKPacket;

//��������
#define SEND_TEST        1
#define RECAL            2

#define KEEPALIBEPERIOD  120    //��λ��2.5s

extern void DataHandler(u8 buf[]);
extern void KeepAliveCheck(void);

extern uint16 Draw_DataX;
extern uint16 Draw_DataY;
extern uint16 Cmd_Address;
extern uint8 Cmd_Command;
#endif

