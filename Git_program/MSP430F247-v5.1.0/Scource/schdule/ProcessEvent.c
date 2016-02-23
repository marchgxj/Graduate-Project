#include "common.h"
TQStruct TQ[MAX_TASK_NUM];
unsigned int current_tsk;
unsigned int last_tsk;
TQStruct emptytask;
/**
* 初始化任务队列
*/
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
uint8 PostTask(uint8 *data,uint8 event)
{
    uint8 i=0;
    if (TQ[last_tsk].event == 0)
    {
        TQ[last_tsk].event = event;
        for(i=0;i<MAX_PACK_LENGTH;i++)
        {
            TQ[last_tsk].data[i] = *data++;		
        }
        last_tsk = (last_tsk + 1) % MAX_TASK_NUM;
        return TQ_SUCCESS;
    }
    else
    {
        //printf("TQ is FULL!\n");
        ReJoinFlag = 1;
        Exit_Sleep = 1;
        Init_TQ();
        
        return TQ_FULL;		
    }
}
uint8 isEmpty()
{
    uint8 i=0;
    uint8 emptycount = 0;
    for(i=0; i<MAX_TASK_NUM; i++)
    {
        if(TQ[i].event == 0)
        {
            emptycount++;
        }
    }
    if(emptycount == MAX_TASK_NUM)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
* 任务出队
*/
uint8 Pop_T(TQStruct* task)
{
    if (TQ[current_tsk].event != 0)
    {
        *task = TQ[current_tsk];
        TQ[current_tsk].event = 0;
        current_tsk = (current_tsk + 1) % MAX_TASK_NUM;
        return TQ_SUCCESS;
    }
    else
    {
        //printf("TQ is EMPTY!\n");
        current_tsk = (current_tsk + 1) % MAX_TASK_NUM;
        *task = emptytask;
        return TQ_EMPTY;
    }
}

uint8 Process_Event()
{
    TQStruct current_event;
    uint8 buffer[6];
    Pop_T(&current_event);
    switch(current_event.event)
    {
      case EVENT_BEACON_HANDLER:
        BeaconHandler(current_event.data);
        break;
      case EVENT_JOINREQUESTACK_HANDLER:
        JoinRequestACKHandler(current_event.data);
        break;
      case EVENT_JOINREQUEST_SEND:
        SendJoinRequest();
        break;
      case EVENT_DATA_SEND:
        DataSend();
        break;
      case EVENT_DATAACK_HANDLER:
        DataACKHandler(current_event.data);
        break;
      case EVENT_WAKE_A7139:
        A7139_WakeToRecv();
        break;
      case EVENT_CSMA_RESEND:
        CSMADataResend();
        break;
      case EVENT_COLLECT_DATA:
        //CollectData();
        break;
      case EVENT_IDENTIFY_CAR:
        IdentifyCar();
        break;
      case EVENT_REJOIN_HANDLER:
        ReJoinHandler();
        break;
      case EVENT_A7139_RESET:
        A7139_Reset();
        break;
      case EVENT_COLLECT_DATA_F:
        //SampleChannel(&Draw_DataX,&Draw_DataY);
        break;
      case EVENT_CALIBRATE_SENSOR:
        NoCarCalibration();
        break;
      case EVENT_GET_VARIANCE:
        //GetVariance();
        break;
      case EVENT_KEEPALIVE_SEND:
        KeepAliveSend();
        break;
      case EVENT_MCUSLEEP_ENABLE:
        halLedClear(4);
        LPM3;
        break;
      case EVENT_CMD_HANDLER:
        CmdHandler();
        break;
      case EVENT_5983_RESET:
        Init_HMC(buffer);
        break;

        
    }
    return current_event.event;
}