#ifndef _DEVICE_h_
#define _DEVICE_h_
#include "common.h"
/****************����beacon������Ҫ���µ��Ĳ���*************/
#define    BEACON_PERIOD     10000   //��λ  100us
#define    BEFOR_BEACON_WAKE BEACON_PERIOD-300   //��beaconǰ1.5msʱ����оƬ

#define    KEEP_ALIVE_PERIOD   80  //��λ50 ms  
#define    WHILE_TIMEOUT BEACON_PERIOD*12
/**********************************************************/
#define    SLOT_LENGTH     60000     //��λus
#define    BACKOFF_DIV     10
#define    BACKOFF_PERIOD  SLOT_LENGTH/BACKOFF_DIV
#define    WAKE_TIME         4000    //SLEEP������Ҫ2ms

#define    DATAACK_TIMEOUT 3000      //ack���ճ�ʱ�������Ƿ����csma�ط��׶�

#define    SLEEP_EN        1 

#define    CCA   200     //�ز������ֵ
#define    CSMA_BACKOFF_TIME 8
#define    CSMA_FAIL    1
#define    CSMA_SUCCESS 0

#define    BROADCAST    0xFF
#define    ROOT     1
#define    DEVICE   2

#define    ACK_EN    1
#define    ACK_DIS   0

//packet type define
#define    BEACON_TYPE              1
#define    JOINREQUEST_TYPE         2
#define    JOINREQUESTACK_TYPE      3
#define    JOINREQUESTACKOK_TYPE    4
#define    DATA_TYPE                5
#define    DATAACK_TYPE             6
#define    REJOIN_TYPE              7


//packet length
#define    BEACON_PACK_LENGTH            7
#define    JOINREQUEST_PACK_LENGTH       6  
#define    JOINREQUESTACK_PACK_LENGTH    8  
#define    JOINREQUESTACKOK_PACK_LENGTH  6  
#define    DATA_PACK_LENGTH              9  
#define    DATAACK_PACK_LENGTH           8

typedef struct{
    uint8 cluster_id;		        //��ID
    uint8 cluster_innernum;		//���ڱ��
    uint8 des_cluster_id;		//Ŀ�Ĵ�ID
    uint8 des_cluster_innernum;		//Ŀ�Ĵر��
    uint8 channel;			//��ǰ�ŵ�
    uint8 backup_channel;		//�����ŵ�
    uint16 pyh_address;			//�����ַ
    uint8 device_type;                  //�豸����
    uint8 free_node;                    //��ǰ�м̿��нڵ���
    uint8 connected;                    //����״̬
    uint8 csma_length;                  //CMSAʱ϶����
    uint8 state;                        //��ǰ״̬
    uint8 time_stamp;                   //ʱ���
    uint8 data_ack;                     //����ack
    uint8 power;			//�Ƿ����͹���
    uint8 parking_state;                //ͣ��״̬
    uint8 parking_state_m;              //��һ��ͣ��״̬
    uint8 cmd;                          //����
    uint16 vlotage;                     //����
    uint16 temperature;                 //�¶�
}EndPointDeviceStruct;

extern EndPointDeviceStruct EndPointDevice;
extern uint8 Unpack(uint8 *type);
extern void BeaconHandler(uint8 beacon[]);
extern uint8 PackValid(void);
extern uint8 SendByCSMA(u8 *buff,uint8 length);
extern uint8 GetCheck(uint8* buf);



extern uint8 DataRecvBuffer[MAX_PACK_LENGTH];
extern uint8 DataSendBuffer[MAX_PACK_LENGTH];
extern uint8 Power_Mode;


#endif