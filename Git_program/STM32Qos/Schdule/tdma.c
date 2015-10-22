#include "common.h"
DataACKPacketStruct DataACKPacket;
uint16 Draw_DataX = 0;
uint16 Draw_DataY = 0;
uint16 Cmd_Address = 0;
uint8 Cmd_Command = 0;


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
	
		DataSendBuffer[0] = DataACKPacket.pack_length;
		DataSendBuffer[1] = DataACKPacket.pack_type<<2|DataACKPacket.ack_en<<1|DataACKPacket.rejoin;
		DataSendBuffer[2] = DataACKPacket.des_cluster_id;
		DataSendBuffer[3] = DataACKPacket.des_cluster_innernum;
		DataSendBuffer[4] = DataACKPacket.src_cluster_id;
		DataSendBuffer[5] = DataACKPacket.src_cluster_innernum;
		DataSendBuffer[6] = DataACKPacket.time_stamp>>8;
		DataSendBuffer[7] = DataACKPacket.time_stamp;
		DataSendBuffer[8] = DataACKPacket.cmd;
		DataSendBuffer[9] = 0;
		DataSendBuffer[10] = 0;
		DataSendBuffer[11] = 0;
		DataACKPacket.cmd = 0;
		

}
uint8 XBEE_Data[23];
uint16 PacketSend_Count = 0;
uint16 AckLost_Count = 0;
void CreatXBEEData()
{
		uint8 i=0;
		uint8 checksum = 0;
		XBEE_Data[0] = 0x7E;
		XBEE_Data[1] = 0x00;
		XBEE_Data[2] = 0x13;
		XBEE_Data[3] = 0x10;
		XBEE_Data[4] = 0x01;
		XBEE_Data[5] = 0x00;
		XBEE_Data[6] = 0x13;
		XBEE_Data[7] = 0xA2;
		XBEE_Data[8] = 0x00;
		XBEE_Data[9] = 0x40;
		XBEE_Data[10] = 0xB5;
		XBEE_Data[11] = 0x9E;
		XBEE_Data[12] = 0x05;
		XBEE_Data[13] = 0xFF;
		XBEE_Data[14] = 0xFE;
		XBEE_Data[15] = 0x00;
		XBEE_Data[16] = 0x00;
		XBEE_Data[17] = PacketSend_Count>>8;
		XBEE_Data[18] = PacketSend_Count;
		XBEE_Data[19] = AckLost_Count>>8;
		XBEE_Data[20] = AckLost_Count;
		XBEE_Data[21] = rssisee;
		for(i=3;i<22;i++)
		{
				checksum+=XBEE_Data[i];
		}
		XBEE_Data[22] = 0xFF-checksum;
		
	
}

UartDataStruct bufnode;
void DataHandler(void)
{
		uint8 inner_num = 0;
		uint8 ab_slot_num = 0;
		uint8 src_cluster_id = 0;
	  uint8 src_cluster_innernum = 0;
		uint8 rejoin = 0;
		uint8 i=0;
		
		LED2_REV();
	  PacketSend_Count = DataRecvBuffer[5]<<8|DataRecvBuffer[6];
		AckLost_Count = DataRecvBuffer[7]<<8|DataRecvBuffer[8];
	 
	
		
		CreateDataACK(0xFF,0xFF,0,1);
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		SendPack();
		RXMode();
		TIME2_LOW;
#if XBEE_ENABLE == 1
		CreatXBEEData();
	  Usart1_PutData(XBEE_Data,23);
#else
		for(i=0;i<15;i++)
		{
				XBEE_Data[i] = 0;
		}
		XBEE_Data[0] = 0x7E;
		XBEE_Data[15] = PacketSend_Count>>8;
		XBEE_Data[16] = PacketSend_Count;
		XBEE_Data[17] = AckLost_Count>>8;
		XBEE_Data[18] = AckLost_Count;
		Usart1_PutData(XBEE_Data,21);

#endif

		
}

void KeepAliveCheck(void)
{
		uint8 i = 0;
		UartDataStruct bufnode;
		for(i=1;i<=RootDevice.connected_devece_count;i++)
		{
				if(RootDevice.endpoint_device[i].keep_alive == 0)
				{
						bufnode.address = RootDevice.endpoint_device[i].pyh_address;
						bufnode.data = NET_LOST;
					  PostUploadNode(&bufnode);
				}
				RootDevice.endpoint_device[i].keep_alive = 0;
		}

}
