#include "common.h"
TQStruct TQ[MAX_TASK_NUM];
unsigned int current_tsk;
unsigned int last_tsk;
/**
* 初始化任务队列
*/
TQStruct emptytask;
void Init_TQ(void)
{
    int i;
    for (i=0; i<MAX_TASK_NUM; i++)
    {
        TQ[i].event = 0;

    }
    current_tsk = 0;
    last_tsk = 0;
		for(i=0;i<MAX_PACK_LENGTH;i++)
		{
				emptytask.data[i] = 0;
		}
		emptytask.event = 0;
}

/**
* 任务进队
*/
uint8 harderr = 0;
uint8 PostTask(uint8 *data,uint8 event)
{
    uint8 i=0;
	  DisableInterrupt();
		if (TQ[last_tsk].event == 0)
    {
        TQ[last_tsk].event = event;
				for(i=0;i<MAX_PACK_LENGTH;i++)
				{
						TQ[last_tsk].data[i] = *data++;		
				}
        last_tsk = (last_tsk + 1) % MAX_TASK_NUM;
				EnableInterrupt();
        return TQ_SUCCESS;
    }
    else
    {
        //printf("TQ is FULL!\n");
				DebugMsg("Task Quene Full");
			  EnableInterrupt();
        return TQ_FULL;		
    }
}

/**
* 任务出队
*/

uint8 Pop_T(TQStruct* task)
{
	  
	  //DisableInterrupt();
    if (TQ[current_tsk].event != 0)
    {
        *task = TQ[current_tsk];
        TQ[current_tsk].event = 0;
			  current_tsk = (current_tsk + 1) % MAX_TASK_NUM;
			  //EnableInterrupt();
        return TQ_SUCCESS;
    }
    else
    {
        //printf("TQ is EMPTY!\n");
			  //EnableInterrupt();
			  *task = emptytask;
			  return TQ_FULL;
    }
		
		
		
}
TQStruct current_event;
uint8 Process_Event()
{
    
    Pop_T(&current_event);

    switch(current_event.event)
    {
			case EVENT_BEACON_SEND:
				SendBeacon(current_event.data);
				break;
			case EVENT_JOINREQUEST_HANDLER:
				JoinRequestHandler(current_event.data);
				break;
//			case EVENT_JOINREQUESTACKOK_HANDLER:
//				DisableInterrupt();
//				JoinRequestACKOKHandler(current_event.data);
//				EnableInterrupt();
//				break;
			case EVENT_DATA_HANDLER:
				DataHandler(current_event.data);
				break;
			case EVENT_UPLOAD_DATA:
				Upload_Data();
				break;
			case EVENT_REJOIN_SEND:
				SendReJoin();
				break;
		  case EVENT_KEEPALIVE_CHECK:
        KeepAliveCheck();
				break;
			case EVENT_A7139_RESET:
        A7139_Reset();
        break;
			case EVENT_LINK_SEND:
				LinkSend();
				break;
    }
    return current_event.event;
}
