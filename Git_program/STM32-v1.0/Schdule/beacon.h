#ifndef _BEACON_h_
#define _BEACON_h_

#define    BEACON_PACK_LENGTH    7
#define    BECAON_PERIOD 500     //��λ��ms

typedef struct {
    uint8 length;     					// ������
		uint8 pack_type:6;					//������
		uint8 ack_en:1;							//�Ƿ���ҪӦ��
		uint8 :1;										//����
		uint8 cluster_id;				  	//��ID
		uint8 cluster_innernum;	  	//���ڱ��
		uint8 des_cluster_id;				//Ŀ�Ĵ�ID
	  uint8 des_cluster_innernum;	//Ŀ�Ĵ��ڱ��
	  uint8 free_num:8;						//���и�����
}BeaconPacketStruct;

extern uint8 PostBeacon(void);
extern void SendBeacon(void);
extern uint8 CreatBeacon(void);

#endif
