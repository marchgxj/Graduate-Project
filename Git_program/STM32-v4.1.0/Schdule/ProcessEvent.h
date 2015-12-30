#ifndef _TASKQ_h_
#define _TASKQ_h_

#include "common.h"
#define    MAX_PACK_LENGTH   12
typedef struct{
    uint8  data[MAX_PACK_LENGTH];		
    uint8  event;						
}TQStruct;


#define    MAX_TASK_NUM    20
#define    TQ_SUCCESS      0
#define    TQ_FULL         1
#define    TQ_EMPTY        2

//event handler define
#define    EVENT_SCAN_CHANNEL             3
#define    EVENT_BEACON_SEND              4
#define    EVENT_BEACON_HANDLER           5
#define    EVENT_JOINREQUEST_SEND         6
#define    EVENT_JOINREQUEST_HANDLER      7
#define    EVENT_JOINREQUESTACK_HANDLER   8
#define    EVENT_JOINREQUESTACKOK_HANDLER 9
#define    EVENT_DATA_SEND                10
#define    EVENT_DATA_HANDLER             11
#define    EVENT_DATAACK_HANDLER          12
#define    EVENT_WAKE_A7139               13
#define    EVENT_CSMA_RESEND              14
#define    EVENT_COLLECT_DATA             15
#define    EVENT_IDENTIFY_CAR             16
#define    EVENT_UPLOAD_DATA		  				17
#define    EVENT_REJOIN_SEND		  				18
#define    EVENT_REJOIN_HANDLER           19
#define    EVENT_A7139_RESET              20
#define    EVENT_COLLECT_DATA_F           21
#define    EVENT_UPLOAD_DRAWDATA          22
#define    EVENT_CALIBRATE_SENSOR         23
#define    EVENT_GET_VARIANCE             24
#define    EVENT_KEEPALIVE_SEND           25
#define    EVENT_KEEPALIVE_CHECK          26
#define    EVENT_MCUSLEEP_ENABLE          27
#define    EVENT_CMD_HANDLER              28
#define    EVENT_LINK_SEND                29

extern void Init_TQ(void);
extern uint8 PostTask(uint8 *data,uint8 event);
extern uint8 Process_Event(void);

extern TQStruct TQ[MAX_TASK_NUM];
#endif
