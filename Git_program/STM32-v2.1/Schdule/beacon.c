#include "common.h"

BeaconPacketStruct BeaconPacket;
uint8 BeaconPacketFIFO[BEACON_PACK_LENGTH];

uint8 CreatBeacon()
{
		BeaconPacket.length = BEACON_PACK_LENGTH;				//beacon���ɳ���Ϊ5bytes
		BeaconPacket.cluster_id = ROOT;							//sink�ڵ���
		BeaconPacket.cluster_innernum = 0;							//����ID sink�ڵ�Ϊ0
		BeaconPacket.des_cluster_id = BROADCAST;						//�㲥��
	  BeaconPacket.des_cluster_innernum = BROADCAST;						//�㲥��
		BeaconPacket.pack_type = BEACON_TYPE;						//��������beacon
		BeaconPacket.ack_en = ACK_EN;									//����ҪӦ��
		BeaconPacket.free_num = RootDevice.free_node;	//���нڵ�����
	
		DataSendBuffer[0] = BeaconPacket.length ;
    DataSendBuffer[1] = BeaconPacket.pack_type<<2|BeaconPacket.ack_en<<1;
    DataSendBuffer[2] = BeaconPacket.cluster_id;
    DataSendBuffer[3] = BeaconPacket.cluster_innernum;
    DataSendBuffer[4] = BeaconPacket.des_cluster_id;
    DataSendBuffer[5] = BeaconPacket.des_cluster_innernum;
    DataSendBuffer[6] = BeaconPacket.free_num;
		
		return 1;
}
uint8 PostBeacon(void)
{
		CreatBeacon();
		return PostTask(EVENT_BEACON_SEND);
}

void SendBeacon(void)
{
		A7139_StrobeCmd(CMD_PLL);
		delay_us(1);
		A7139_WriteFIFO(DataSendBuffer,BEACON_PACK_LENGTH);
	  delay_us(1);
	  A7139_StrobeCmd(CMD_TX);
		//delay_ms(11);
		while(GIO1);
		LED1_REV();
		RXMode();
}
