#ifndef _UPLOAD_h_
#define _UPLOAD_h_
#include "common.h"

#define UPLOAD_NODE_NUM  MAX_NODE_NUM
typedef struct{
		uint16	address;
		uint8 	data;
}UartDataStruct;

extern void Upload_Data(void);
extern UartDataStruct PopUploadNode(void);
extern uint8 PostUploadNode(UartDataStruct node);
extern uint8 Upload_Ack;
extern uint8 Node_Inwaiting;
extern UartDataStruct UploadTQ[UPLOAD_NODE_NUM];

extern void Init_Upload_TQ(void);
extern void UploadDrawData(void);

#endif
