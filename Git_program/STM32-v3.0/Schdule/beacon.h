#ifndef _BEACON_h_
#define _BEACON_h_


#define    BECAON_PERIOD 500     //��λ��ms

typedef struct {
    uint8 length;     							// ������
		uint8 pack_type:6;							//������
		uint8 ack_en:1;									//�Ƿ���ҪӦ��
		uint8 power:1;								//�Ƿ����͹���
		uint8 des_cluster_id;				  	//��ID
		uint8 des_cluster_innernum;	  	//���ڱ��
		uint8 cluster_id;								//Ŀ�Ĵ�ID
	  uint8 cluster_innernum;					//Ŀ�Ĵ��ڱ��
	  uint8 free_num:8;								//���и�����
	  uint8 check;										//У��
}BeaconPacketStruct;



extern uint8 PostBeacon(void);
extern void SendBeacon(u8* buf);
extern uint8 CreatBeacon(void);
extern uint8 PackValid(void);
extern uint8 errorbuf[12];

#endif
