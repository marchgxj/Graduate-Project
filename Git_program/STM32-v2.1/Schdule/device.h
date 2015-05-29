#ifndef _DEVICE_h_
#define _DEVICE_h_

#define    ROOT     1
#define    BROADCAST    0xFF
#define    MAX_NODE_NUM  254

#define    ACK_EN     1
#define    ACK_DIS    0 

#define 	 LOW_POWER  0


//packet type define
#define    BEACON_TYPE              1
#define    JOINREQUEST_TYPE         2
#define    JOINREQUESTACK_TYPE      3
#define    JOINREQUESTACKOK_TYPE    4
#define    DATA_TYPE                5
#define    DATAACK_TYPE             6

//packet length
#define    BEACON_PACK_LENGTH            7
#define    JOINREQUEST_PACK_LENGTH       6  
#define    JOINREQUESTACK_PACK_LENGTH    8  
#define    JOINREQUESTACKOK_PACK_LENGTH  6  
#define    DATA_PACK_LENGTH              9  
#define    DATAACK_PACK_LENGTH           8


typedef struct{
    uint8  cluster_id;		        			//��ID
    uint8  cluster_innernum;						//���ڱ��
    uint16 pyh_address;									//�����ַ
    uint8  data;												//����
	  uint16 ab_slot_num;									//����ʱ϶��
	
}EndPointDeviceStruct;

typedef struct{
    uint8 cluster_id;		        				//��ID
    uint8 cluster_innernum;							//���ڱ��
    uint8 des_cluster_id;								//Ŀ�Ĵ�ID
    uint16 pyh_address;									//�����ַ
    uint8 device_type;                  //�豸����
    uint8 free_node;                    //��ǰ�м̿��нڵ���
    uint8 csma_length;                  //CMSAʱ϶����
    uint8 state;                        //��ǰ״̬
		uint8 connected_devece_count;
		EndPointDeviceStruct endpoint_device[254];
}RootDeviceStruct;

extern uint8 DataSendBuffer[MAX_PACK_LENGTH];
extern uint8 DataRecvBuffer[MAX_PACK_LENGTH];
extern RootDeviceStruct RootDevice;
extern uint16 Frame_Time;
extern uint8 Power_Mode;

extern uint8 Unpack(uint8 *type);
#endif 
