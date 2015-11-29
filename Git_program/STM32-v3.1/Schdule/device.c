#include "common.h"
uint8 DataSendBuffer[MAX_PACK_LENGTH];
uint8 DataRecvBuffer[MAX_PACK_LENGTH];
uint8 EmptyBuffer[MAX_PACK_LENGTH]={0,0,0,0,0,0,0,0,0,0,0,0};
RootDeviceStruct RootDevice;
uint16 Frame_Time = 0;
uint8 Power_Mode = 1;    //0：每个超帧都发送  1：数据变化时发送

uint8 GetCheck(uint8* buf)
{
		
	  uint8 i=0;
		uint8 checksum=0;
	  for(i=0;i<MAX_PACK_LENGTH-1;i++)
		{
				checksum+=*buf++;
		}
		
		return checksum = 0xFF-checksum;
	  
}
uint8 Unpack(uint8 *type)
{
    type++;
    return (*type&0xFC)>>2;
}
uint8 PackValid(void)
{
		if((DataRecvBuffer[2]==RootDevice.cluster_id&&
				DataRecvBuffer[3]==RootDevice.cluster_innernum)||
		   (DataRecvBuffer[2]==BROADCAST&&
		    DataRecvBuffer[3]==BROADCAST))
		{
				if(GetCheck(DataRecvBuffer)==DataRecvBuffer[11])
        {
            return 1;
        }
				else
				{
						return 0;
				}
		}
		else
				return 0;
}
int abs(int x)
{
		if(x>0)
			return x;
		else
			return -x;
		
}
