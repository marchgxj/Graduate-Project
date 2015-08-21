#include "common.h"
uint8 TQ[MAX_TASK_NUM];
unsigned int current_tsk;
unsigned int last_tsk;
/**
* ��ʼ���������
*/
void Init_TQ(void)
{
    int i;
    for (i=0; i<MAX_TASK_NUM; i++)
    {
        TQ[i] = 0;
    }
    current_tsk = 0;
    last_tsk = 0;
}

/**
* �������
*/
uint8 PostTask(uint8 event)
{
    if (TQ[last_tsk] == 0)
    {
        TQ[last_tsk] = event;
        last_tsk = (last_tsk + 1) % MAX_TASK_NUM;
        return TQ_SUCCESS;
    }
    else
    {
        //printf("TQ is FULL!\n");
        return TQ_FULL;		
    }
}

/**
* �������
*/
uint8 Pop_T(void)
{
    uint8 event;
    if (TQ[current_tsk] != 0)
    {
        event = TQ[current_tsk];
        TQ[current_tsk] = 0;
        current_tsk = (current_tsk + 1) % MAX_TASK_NUM;
        return event;
    }
    else
    {
        //printf("TQ is EMPTY!\n");
        return TQ_EMPTY;
    }
}

uint8 Process_Event()
{
    uint8 current_event;
    current_event = Pop_T();
    switch(current_event)
    {
      case EVENT_BEACON_HANDLER:
        BeaconHandler(DataRecvBuffer);
        break;
      case EVENT_JOINREQUESTACK_HANDLER:
        JoinRequestACKHandler();
        break;
      case EVENT_JOINREQUEST_SEND:
        SendJoinRequest();
        break;
      case EVENT_DATA_SEND:
        DataSend();
        break;
      case EVENT_DATAACK_HANDLER:
        DataACKHandler();
        break;
      case EVENT_WAKE_A7139:
        A7139_WakeToRecv();
        break;
      case EVENT_CSMA_RESEND:
        CSMADataResend();
        break;
      case EVENT_COLLECT_DATA:
        CollectData();
        break;
      case EVENT_IDENTIFY_CAR:
        IdentifyCar();
        break;
      case EVENT_REJOIN_HANDLER:
        REBOOT;
        break;
      case EVENT_A7139_RESET:
        A7139_Reset();
        break;
      case EVENT_COLLECT_DATA_F:
        SampleChannel(&Draw_DataX,&Draw_DataY);
        break;
      case EVENT_CALIBRATE_SENSOR:
        Calibration();
        break;
      case EVENT_GET_VARIANCE:
        GetVariance();
        break;
        
    }
    return current_event;
}