#include "common.h"

BeaconPacketStruct BeaconPacket;
uint8 BeaconPacketFIFO[BEACON_PACK_LENGTH];

uint8 CreatBeacon()
{
		BeaconPacket.length = BEACON_PACK_LENGTH;				//beacon���ɳ���Ϊ5bytes
		BeaconPacket.cluster_id = SINK_NUM;							//sink�ڵ���
		BeaconPacket.cluster_innernum = 0;							//����ID sink�ڵ�Ϊ0
		BeaconPacket.des_cluster_id = BROADCAST;						//�㲥��
	  BeaconPacket.des_cluster_innernum = BROADCAST;						//�㲥��
		BeaconPacket.pack_type = BEACON_TYPE;						//��������beacon
		BeaconPacket.ack_en = ACK_EN;									//����ҪӦ��
		BeaconPacket.free_num = 16;											//���нڵ�����
	
		
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
