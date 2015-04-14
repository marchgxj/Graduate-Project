#include "common.h"
void (*TQ[MAX_TASK_NUM])();
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
int PostTask(void (*T)())
{
    if (TQ[last_tsk] == 0)
    {
        TQ[last_tsk] = T;
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
int Pop_T(void)
{
    if (TQ[current_tsk] != 0)
    {
        TQ[current_tsk]();
        TQ[current_tsk] = 0;
        current_tsk = (current_tsk + 1) % MAX_TASK_NUM;
        return TQ_SUCCESS;
    }
    else
    {
        //printf("TQ is EMPTY!\n");
        return TQ_EMPTY;
    }
}

/**
* ִ���������
*/
void Run_TQ(void)
{
    //int f;
    while (1)
    {
        Pop_T();
    }
}