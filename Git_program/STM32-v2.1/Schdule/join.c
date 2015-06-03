#include "common.h"
JoinRequestACKPacketStruct JoinRequestACKPacket;
uint32 new_node_pyh_address = 0;
void CreateJoinRequectACK(uint8 joinstatus,uint8 accept,uint8 id,uint32 des_address)
{
		JoinRequestACKPacket.length = JOINREQUESTACK_PACK_LENGTH;
		JoinRequestACKPacket.pack_type = JOINREQUESTACK_TYPE;
	  JoinRequestACKPacket.ack_en = ACK_EN;
	  if((accept==1)&&(joinstatus==0))									//�����������¼���ڵ�
		{
				JoinRequestACKPacket.accept = 1;
				JoinRequestACKPacket.cluster_id = ROOT;
				JoinRequestACKPacket.cluster_innernum = id;
		}
		else if((accept==1)&&(joinstatus==1))							//�����������Ѽ���ڵ�
		{
				JoinRequestACKPacket.accept = 1;
				JoinRequestACKPacket.cluster_id = ROOT;
				JoinRequestACKPacket.cluster_innernum = id;
				JoinRequestACKPacket.ack_en = ACK_DIS;
		}
		else
		{
				JoinRequestACKPacket.accept = 0;
				JoinRequestACKPacket.cluster_id = ROOT;
				JoinRequestACKPacket.cluster_innernum = 0;
		}
		JoinRequestACKPacket.src_cluster_id = RootDevice.cluster_id;
		JoinRequestACKPacket.src_cluster_innernum = 0;
		JoinRequestACKPacket.des_phy_address = des_address;
		
		DataSendBuffer[0] = JoinRequestACKPacket.length;
		DataSendBuffer[1] = JoinRequestACKPacket.pack_type<<2|JoinRequestACKPacket.ack_en<<1|JoinRequestACKPacket.accept;
		DataSendBuffer[2] = JoinRequestACKPacket.des_phy_address>>8;
		DataSendBuffer[3] = JoinRequestACKPacket.des_phy_address;
		DataSendBuffer[4] = JoinRequestACKPacket.src_cluster_id;
		DataSendBuffer[5] = JoinRequestACKPacket.src_cluster_innernum;
		DataSendBuffer[6] = JoinRequestACKPacket.cluster_id;
		DataSendBuffer[7] = JoinRequestACKPacket.cluster_innernum;
		DataSendBuffer[8] = 0;
		DataSendBuffer[9] = 0;
		DataSendBuffer[10] = 0;
		DataSendBuffer[11] = 0;
}

void JoinRequestHandler()
{
		
	  uint8 i=0;
		uint8 joinstatus = 0;
	  uint8 accept = 0;
		uint8 current_node_num = RootDevice.connected_devece_count;
		new_node_pyh_address = DataRecvBuffer[4]<<8|DataRecvBuffer[5];
	  for(i=0;i<RootDevice.connected_devece_count;i++)
		{
				if(RootDevice.endpoint_device[i].pyh_address == new_node_pyh_address)	//���豸�Ѿ�����
				{
						joinstatus = 1;
						accept = 1;
						CreateJoinRequectACK(joinstatus,accept,i+1,new_node_pyh_address);
						SendPack();
						RXMode();
						return ;
				}
		}
		if(RootDevice.connected_devece_count<=MAX_NODE_NUM)		//������Դ�����Լ���
		{
				accept = 1;
			  current_node_num++;
				CreateJoinRequectACK(joinstatus,accept,current_node_num,new_node_pyh_address);
				SendPack();
				RXMode();
		}
}
void JoinRequestACKOKHandler()
{
		RootDevice.connected_devece_count++;
		RootDevice.endpoint_device[RootDevice.connected_devece_count].cluster_id = DataRecvBuffer[4];
		RootDevice.endpoint_device[RootDevice.connected_devece_count].cluster_innernum = DataRecvBuffer[5];
	  RootDevice.endpoint_device[RootDevice.connected_devece_count].pyh_address = new_node_pyh_address;
		RootDevice.free_node = MAX_NODE_NUM-RootDevice.connected_devece_count;
}

