#include "common.h"
DataACKPacketStruct DataACKPacket;
uint16 Draw_DataX = 0;
uint16 Draw_DataY = 0;


void CreateDataACK(uint8 src_cluster_id,uint8 src_cluster_innernum,uint8 rejoin)
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
	
		DataSendBuffer[0] = DataACKPacket.pack_length;
		DataSendBuffer[1] = DataACKPacket.pack_type<<2|DataACKPacket.ack_en<<1|DataACKPacket.rejoin;
		DataSendBuffer[2] = DataACKPacket.des_cluster_id;
		DataSendBuffer[3] = DataACKPacket.des_cluster_innernum;
		DataSendBuffer[4] = DataACKPacket.src_cluster_id;
		DataSendBuffer[5] = DataACKPacket.src_cluster_innernum;
		DataSendBuffer[6] = DataACKPacket.time_stamp>>8;
		DataSendBuffer[7] = DataACKPacket.time_stamp;
		DataSendBuffer[8] = 0;
		DataSendBuffer[9] = 0;
		DataSendBuffer[10] = 0;
		DataSendBuffer[11] = 0;
}
UartDataStruct bufnode;
void DataHandler(void)
{
		uint8 inner_num = 0;
		uint8 ab_slot_num = 0;
		uint8 src_cluster_id = 0;
	  uint8 src_cluster_innernum = 0;
		uint8 rejoin = 0;

	  src_cluster_id = DataRecvBuffer[4];
	  src_cluster_innernum = DataRecvBuffer[5];
	  ab_slot_num = DataRecvBuffer[6]<<8|DataRecvBuffer[7];
		inner_num = DataRecvBuffer[5];
		if((RootDevice.endpoint_device[inner_num].pyh_address==0)||
			 (ab_slot_num>RootDevice.endpoint_device[inner_num].ab_slot_num)
			)
		{
				rejoin = 1;
		}
		else
		{
				rejoin = 0;
				RootDevice.endpoint_device[inner_num].ab_slot_num++;
				RootDevice.endpoint_device[inner_num].data = DataRecvBuffer[8];
				bufnode.address = RootDevice.endpoint_device[inner_num].pyh_address;
				bufnode.data = RootDevice.endpoint_device[inner_num].data;
#if (UPLOAD_DATA_EN == 1)
				if(Power_Mode == 0)
				{
						Draw_DataX = DataRecvBuffer[8]<<8|DataRecvBuffer[9];
					  Draw_DataY = DataRecvBuffer[10]<<8|DataRecvBuffer[11];
					  PostTask(EVENT_UPLOAD_DRAWDATA);
				}
				else
				{
						PostUploadNode(&bufnode);
				}
			  
#endif
				TIME2_HIGH;
		}
				CreateDataACK(src_cluster_id,src_cluster_innernum,rejoin);
				
				SendPack();
				RXMode();
				TIME2_LOW;

		
}
