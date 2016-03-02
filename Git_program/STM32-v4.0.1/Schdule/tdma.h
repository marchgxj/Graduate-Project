#ifndef _TDMA_h_
#define _TDMA_h_
#include "common.h"

typedef struct{
		uint8  pack_length;                  //包长度
	  uint8  pack_type:6;                 //包类型
    uint8  ack_en:1;                    //是否需要应答
    uint8  rejoin:1;                    //重新入网
	  uint8  des_cluster_id;              //目的簇ID
    uint8  des_cluster_innernum;        //目的簇内编号
    uint8  src_cluster_id;              //源簇ID
    uint8  src_cluster_innernum;        //源簇内编号
	  uint16  time_stamp;									//时间戳
	  uint8  cmd;													//控制命令
}DataACKPacketStruct;
extern DataACKPacketStruct DataACKPacket;

//控制命令
#define SEND_TEST        1
#define RECAL            2

#define KEEPALIBEPERIOD  120    //单位：2.5s

extern void DataHandler(u8 buf[]);
extern void KeepAliveCheck(void);

extern uint16 Draw_DataX;
extern uint16 Draw_DataY;
extern uint16 Cmd_Address;
extern uint8 Cmd_Command;
#endif

