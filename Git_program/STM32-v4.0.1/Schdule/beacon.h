#ifndef _BEACON_h_
#define _BEACON_h_

typedef struct {
    uint8 length;     							// 包长度
		uint8 pack_type:6;							//包类型
		uint8 ack_en:1;									//是否需要应答
		uint8 power:1;								//是否开启低功耗
		uint8 des_cluster_id;				  	//簇ID
		uint8 des_cluster_innernum;	  	//簇内编号
		uint8 cluster_id;								//目的簇ID
	  uint8 cluster_innernum;					//目的簇内编号
	  uint8 free_num:8;								//空闲负载数
	  uint8 check;										//校验
}BeaconPacketStruct;



extern uint8 PostBeacon(void);
extern void SendBeacon(u8* buf);
extern uint8 CreatBeacon(void);
extern uint8 PackValid(void);
extern uint8 errorbuf[12];

#endif
