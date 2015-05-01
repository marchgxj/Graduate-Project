#include "common.h"

BeaconPacketStruct BeaconPacket;
uint8 BeaconPacketFIFO[BEACON_PACK_LENGTH];

uint8 CreatBeacon()
{
		BeaconPacket.length = BEACON_PACK_LENGTH;				//beacon包成长度为5bytes
		BeaconPacket.cluster_id = SINK_NUM;							//sink节点编号
		BeaconPacket.cluster_innernum = 0;							//簇内ID sink节点为0
		BeaconPacket.des_cluster_id = BROADCAST;						//广播包
	  BeaconPacket.des_cluster_innernum = BROADCAST;						//广播包
		BeaconPacket.pack_type = BEACON_TYPE;						//包类型是beacon
		BeaconPacket.ack_en = ACK_EN;									//不需要应答
		BeaconPacket.free_num = 16;											//空闲节点数量
	
		
		return 1;
}
uint8 PostBeacon(void)
{
		CreatBeacon();
		return PostTask(EVENT_SEND_BEACON);
}

void SendBeacon(void)
{
		A7139_StrobeCmd(CMD_PLL);
		delay_us(1);
		A7139_WriteFIFO(&BeaconPacket.length,BEACON_PACK_LENGTH);
	  delay_us(1);
	  A7139_StrobeCmd(CMD_TX);
		delay_ms(11);
		LED1_REV();
		RXMode();
}
