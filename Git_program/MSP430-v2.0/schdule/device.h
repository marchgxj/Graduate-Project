#ifndef _DEVICE_h_
#define _DEVICE_h_

#define    SLOT_LENGTH     5000     //��λus
#define    BACKOFF_DIV     5
#define    BACKOFF_PERIOD  SLOT_LENGTH/BACKOFF_DIV
#define    CCA   28     //�ز������ֵ
#define    CSMA_BACKOFF_TIME 8
#define    CSMA_FAIL    1
#define    CSMA_SUCCESS 0

#define ROOT     1
#define DEVICE   2

#define    ACK_EN    1
#define    ACK_DIS   0

//packet type define
#define    BEACON_TYPE         1
#define    JOINREQUEST_TYPE    2

//event handler define
#define    BEACON_HANDLER         1
#define    JOINREQUEST_HANDLER    2

typedef struct{
    uint8 cluster_id;		        //��ID
    uint8 cluster_innernum;		//���ڱ��
    uint8 des_cluster_id;		//Ŀ�Ĵ�ID
    uint8 channel;			//��ǰ�ŵ�
    uint8 backup_channel;		//�����ŵ�
    uint8 pyh_address;			//�����ַ
    uint8 device_type;                  //�豸����
    uint8 free_node;                    //��ǰ�м̿��нڵ���
    uint8 connected;                    //����״̬
    uint8 csma_length;                  //CMSAʱ϶����
    uint8 state;                        //��ǰ״̬
}EndPointDeviceStruct;

extern EndPointDeviceStruct EndPointDevice;
extern uint8 Unpack(uint8 *type);
extern void BeaconHandler(uint8 beacon[]);


extern uint8 DataRecvBuffer[MAX_PACK_LENGTH];
extern uint8 DataSendBuffer[MAX_PACK_LENGTH];

#endif