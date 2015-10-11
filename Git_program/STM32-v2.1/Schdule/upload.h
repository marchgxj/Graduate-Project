#ifndef _UPLOAD_h_
#define _UPLOAD_h_
#include "common.h"
#define UPLOAD_DATA_EN 1
#define UPLOAD_NODE_NUM  100
typedef struct{
		uint16	address;
		uint8 	data;
}UartDataStruct;

extern void Upload_Data(void);
extern UartDataStruct PopUploadNode(void);
extern uint8 PostUploadNode(UartDataStruct* node);
extern uint8 Upload_Ack;

extern void Init_Upload_TQ(void);
extern void UploadDrawData(void);

#endif
