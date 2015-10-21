#include "common.h"

BeaconPacketStruct BeaconPacket;
uint8 BeaconPacketFIFO[BEACON_PACK_LENGTH];

uint8 CreatBeacon()
{
		RootDevice.free_node = MAX_NODE_NUM - RootDevice.connected_devece_count;
		BeaconPacket.length = BEACON_PACK_LENGTH;				//beacon���ɳ���Ϊ5bytes
		BeaconPacket.cluster_id = ROOT;							//sink�ڵ���
		BeaconPacket.cluster_innernum = 0;							//����ID sink�ڵ�Ϊ0
		BeaconPacket.des_cluster_id = BROADCAST;						//�㲥��
	  BeaconPacket.des_cluster_innernum = BROADCAST;						//�㲥��
		BeaconPacket.pack_type = BEACON_TYPE;						//��������beacon
		BeaconPacket.ack_en = ACK_EN;									//����ҪӦ��
		BeaconPacket.free_num = RootDevice.free_node;	//���нڵ�����
		//BeaconPacket.power = LOW_POWER;
		BeaconPacket.power = Power_Mode;
	
		DataSendBuffer[0] = BeaconPacket.length ;
    DataSendBuffer[1] = BeaconPacket.pack_type<<2|BeaconPacket.ack_en<<1|BeaconPacket.power;
    DataSendBuffer[2] = BeaconPacket.des_cluster_id;
    DataSendBuffer[3] = BeaconPacket.des_cluster_innernum;
    DataSendBuffer[4] = BeaconPacket.cluster_id;
    DataSendBuffer[5] = BeaconPacket.cluster_innernum;
    DataSendBuffer[6] = BeaconPacket.free_num;
		
		return 1;
}
uint8 PostBeacon(void)
{
		CreatBeacon();
		return PostTask(DataSendBuffer,EVENT_BEACON_SEND);
}

void SendBeacon(u8* buf)
{
		TIME2_HIGH;
		
	  SendPack(buf);
		LED1_REV();
		RXMode();
		TIME2_LOW;

}
