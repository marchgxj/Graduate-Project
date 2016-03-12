#ifndef _CONFIG_h_
#define _CONFIG_h_
#include "common.h"

//------------------System Config----------------------
#define    LOWPOWER_THRESHOLD  600
#define    VAR_THRESHOLD       5000             //�����ж���ֵ
#define    EXT_THRESHOLD       200               //�����ֵ�ж���ֵ
#define    INT_THRESHOLD       60
#define    DIS_THRESHOLD       1000
#define    COM_THRESHOLD       5000

#define   LOWPOWER_THRESHOLD_HIGH   600
#define   LOWPOWER_THRESHOLD_LOW    500
#define   TEMP_THRESHOLD_HIGH       3500
#define   TEMP_THRESHOLD_LOW        1000

#define   COM_STABLE_COUNT    100    //adjust the last time of compactness
#define   SEND_DATA_FREQ       470.001f
#define   VALUE_BASIC         10000


//---------------Working Mode Config-------------------
#define    QOS_TEST            0
#define    NET_TEST            0
#define    TEMP_TEST           0             //0:No Test  1��Test 5983  2��Test GMI
#define    MCU_SLEEP_ENABLE    1
#define    GMI_EN              0
#define    SLEEP_EN            1 
#define    LED_EN              1
#define    WIRELESS_TEST       0
//------------------Net Work Config--------------------
/****************����beacon������Ҫ���µ��Ĳ���*************/
#define    BEACON_PERIOD       10000   //��λ  100us
#define    BEFOR_BEACON_WAKE   BEACON_PERIOD-300   //��beaconǰ1.5msʱ����оƬ

#define    KEEP_ALIVE_PERIOD   200  //��λ50 ms  
#define    WHILE_TIMEOUT       BEACON_PERIOD*12
/**********************************************************/
#define    SLOT_LENGTH         60000     //��λus
#define    BACKOFF_DIV         10
#define    BACKOFF_PERIOD      SLOT_LENGTH/BACKOFF_DIV
#define    WAKE_TIME           4000    //SLEEP������Ҫ2ms

#define    DATAACK_TIMEOUT     1000      //ack���ճ�ʱ�������Ƿ����csma�ط��׶�

#define    CCA                 200     //�ز������ֵ
#define    CSMA_BACKOFF_TIME   8
#define    CSMA_FAIL           1
#define    CSMA_SUCCESS        0
#endif