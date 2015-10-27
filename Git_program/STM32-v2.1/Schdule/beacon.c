#include "common.h"

BeaconPacketStruct BeaconPacket;
uint8 BeaconPacketBuf[MAX_PACK_LENGTH];

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
	
		BeaconPacketBuf[0] = BeaconPacket.length ;
    BeaconPacketBuf[1] = BeaconPacket.pack_type<<2|BeaconPacket.ack_en<<1|BeaconPacket.power;
    BeaconPacketBuf[2] = BeaconPacket.des_cluster_id;
    BeaconPacketBuf[3] = BeaconPacket.des_cluster_innernum;
    BeaconPacketBuf[4] = BeaconPacket.cluster_id;
    BeaconPacketBuf[5] = BeaconPacket.cluster_innernum;
    BeaconPacketBuf[6] = BeaconPacket.free_num;
	  BeaconPacketBuf[7] = 0;
		BeaconPacketBuf[8] = 0;
		BeaconPacketBuf[9] = 0;
		BeaconPacketBuf[10] = 0;
		BeaconPacketBuf[11] = 0;
		
		return 1;
}
uint8 PostBeacon(void)
{
		uint8 buf;
	  CreatBeacon();
		buf = PostTask(BeaconPacketBuf,EVENT_BEACON_SEND);
	 return buf;
}
uint8 errorbuf[12];
void SendBeacon(u8* buf)
{
		uint8 i=0;
	  if(Unpack(buf)!=BEACON_TYPE)
    {
        DebugMsg("Beacon DataRecvBuffer Changed");
			  for (i=0;i<12;i++)
			  {
						errorbuf[i] = *buf++;
				}
				i=0;
				return;
    }
		
		TIME2_HIGH;
	  SendPack(buf);
		LED1_REV();
		RXMode();
		TIME2_LOW;

}
