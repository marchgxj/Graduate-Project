#ifndef _DEVICE_h_
#define _DEVICE_h_

#define    ROOT     1
#define    BROADCAST    0xFF
#define    MAX_NODE_NUM  254

#define    ACK_EN     1
#define    ACK_DIS    0

//packet type define
#define    BEACON_TYPE         			1
#define    JOINREQUEST_TYPE    			2
#define    JOINREQUESTACK_TYPE    	3
#define    JOINREQUESTACKOK_TYPE    4

//packet length
#define    SCAN_PACK_LENGTH  7      //ɨ��׶η��͵İ�����
#define    JOINREQUEST_PACK_LENGTH  8  //�������������
#define    JOINREQUESTACK_PACK_LENGTH  10  //����������Ӧ������


typedef struct{
    uint8 cluster_id;		        				//��ID
    uint8 cluster_innernum;							//���ڱ��
    uint32 pyh_address;									//�����ַ
    uint8 data;													//����
	
}EndPointDeviceStruct;

typedef struct{
    uint8 cluster_id;		        				//��ID
    uint8 cluster_innernum;							//���ڱ��
    uint8 des_cluster_id;								//Ŀ�Ĵ�ID
    uint32 pyh_address;									//�����ַ
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

extern uint8 Unpack(uint8 *type);
#endif 
