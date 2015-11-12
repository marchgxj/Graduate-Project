#include "common.h"
DataACKPacketStruct DataACKPacket;
uint16 Draw_DataX = 0;
uint16 Draw_DataY = 0;
uint16 Cmd_Address = 0;
uint8 Cmd_Command = 0;
uint8 DataAckBuf[MAX_PACK_LENGTH];
uint8 QosTestData[21];
uint16 Qos_DataReceive_Count = 0;


void CreateDataACK(uint8 src_cluster_id,uint8 src_cluster_innernum,uint8 rejoin,uint16 phy_address)
{
		DataACKPacket.pack_length = DATAACK_PACK_LENGTH;
	  DataACKPacket.pack_type = DATAACK_TYPE;
	  DataACKPacket.ack_en = ACK_DIS;
	  DataACKPacket.rejoin = rejoin;
	  DataACKPacket.des_cluster_id = src_cluster_id;
	  DataACKPacket.des_cluster_innernum = src_cluster_innernum;
	  DataACKPacket.src_cluster_id = RootDevice.cluster_id;
	  DataACKPacket.src_cluster_innernum = RootDevice.cluster_innernum;
	  DataACKPacket.time_stamp = Frame_Time;
		if(Cmd_Address == phy_address)
		{
				DataACKPacket.cmd = Cmd_Command;
				Cmd_Address = 0;
				Cmd_Command = 0;
		}
	
		DataAckBuf[0] = DataACKPacket.pack_length;
		DataAckBuf[1] = DataACKPacket.pack_type<<2|DataACKPacket.ack_en<<1|DataACKPacket.rejoin;
		DataAckBuf[2] = DataACKPacket.des_cluster_id;
		DataAckBuf[3] = DataACKPacket.des_cluster_innernum;
		DataAckBuf[4] = DataACKPacket.src_cluster_id;
		DataAckBuf[5] = DataACKPacket.src_cluster_innernum;
		DataAckBuf[6] = DataACKPacket.time_stamp>>8;
		DataAckBuf[7] = DataACKPacket.time_stamp;
		DataAckBuf[8] = DataACKPacket.cmd;
		DataAckBuf[9] = 0;
		DataAckBuf[10] = 0;
		DataAckBuf[11] = GetCheck(DataAckBuf);
		DataACKPacket.cmd = 0;
		

}

void DataHandler(u8* buf)
{
		uint8 inner_num = 0;
		uint8 ab_slot_num = 0;
		uint8 src_cluster_id = 0;
	  uint8 src_cluster_innernum = 0;
		uint8 rejoin = 0;
		uint8 i = 0;
		uint8 acklost_count = 0;
		UartDataStruct bufnode;
	
		if(Unpack(buf)!=DATA_TYPE)
    {
        DebugMsg("Data DataRecvBuffer Changed");
				return;
    }

	  src_cluster_id = *(buf+4);
	  src_cluster_innernum = *(buf+5);
	  ab_slot_num = *(buf+6)<<8|*(buf+7);
		inner_num = *(buf+5);
#if QOS_TEST == 0
		if((RootDevice.endpoint_device[inner_num].pyh_address==0)||
			 (abs(ab_slot_num-RootDevice.endpoint_device[inner_num].ab_slot_num)>10)
			)
		{
				rejoin = 1;
		}
		else
#endif
		{
				rejoin = 0;
				RootDevice.endpoint_device[inner_num].ab_slot_num++;
				if(RootDevice.endpoint_device[inner_num].ab_slot_num == 256)
				{
						RootDevice.endpoint_device[inner_num].ab_slot_num = 0;
				}
				RootDevice.endpoint_device[inner_num].data = *(buf+8);
				RootDevice.endpoint_device[inner_num].keep_alive = KeepAliveCount;
				bufnode.address = RootDevice.endpoint_device[inner_num].pyh_address;
				bufnode.data = RootDevice.endpoint_device[inner_num].data;
#if ((UPLOAD_DATA_EN == 1)&&(QOS_TEST==0))
				UploadTQ[Node_Inwaiting].address = bufnode.address;
				UploadTQ[Node_Inwaiting].data = bufnode.data;
				Node_Inwaiting++;
				if(Node_Inwaiting>UPLOAD_NODE_NUM)
				{
						Node_Inwaiting = 0;
				}
				//PostUploadNode(bufnode);
#endif
				TIME2_HIGH;
		}
				CreateDataACK(src_cluster_id,src_cluster_innernum,rejoin,RootDevice.endpoint_device[inner_num].pyh_address);
				__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
				TIME1_HIGH;
				SendPack(DataAckBuf);
				RXMode();
#if QOS_TEST == 1
				acklost_count = *(buf+8)<<8|*(buf+9);
				for(i=0;i<15;i++)
				{
						QosTestData[i] = 0;
				}
				QosTestData[0] = 0x7E;
				QosTestData[13] = Qos_DataReceive_Count>>8;
				QosTestData[14] = Qos_DataReceive_Count;
				QosTestData[15] = ab_slot_num>>8;
				QosTestData[16] = ab_slot_num;
				QosTestData[17] = acklost_count>>8;
				QosTestData[18] = acklost_count;
				Usart1_PutData(QosTestData,21);
				if(ab_slot_num==0)
				{
						Qos_DataReceive_Count = ab_slot_num;
				}
				Qos_DataReceive_Count++;
#endif
				TIME1_LOW;
				TIME2_LOW;
}

void KeepAliveCheck(void)
{
		uint8 i = 0;
		uint32 countbuf = KeepAliveCount;
		UartDataStruct bufnode;
		for(i=1;i<=RootDevice.connected_devece_count;i++)
		{
				if(abs(countbuf-RootDevice.endpoint_device[i].keep_alive)>KEEPALIBEPERIOD)
				{
						bufnode.address = RootDevice.endpoint_device[i].pyh_address;
						bufnode.data = NET_LOST;
					  //PostUploadNode(bufnode);
						UploadTQ[Node_Inwaiting].address = bufnode.address;
						UploadTQ[Node_Inwaiting].data = bufnode.data;
						Node_Inwaiting++;
						if(Node_Inwaiting>UPLOAD_NODE_NUM)
						{
								Node_Inwaiting = 0;
						}
				}
		}

}
