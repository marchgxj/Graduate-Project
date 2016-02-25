#ifndef _CONFIG_h_
#define _CONFIG_h_
#include "common.h"

//------------------System Config----------------------
#define    LOWPOWER_THRESHOLD  600
#define    CAL_PERIOD          6000          //10分钟校准一次
#define    VAR_THRESHOLD       50             //方差判断阈值
#define    EXT_THRESHOLD       40               //两轴差值判断阈值
#define    INT_THRESHOLD       60
#define    DIS_THRESHOLD       30
#define    COM_THRESHOLD       1500

#define   LOWPOWER_THRESHOLD_HIGH   600
#define   LOWPOWER_THRESHOLD_LOW    500
#define   TEMP_THRESHOLD_HIGH       3500
#define   TEMP_THRESHOLD_LOW        1000

//---------------Working Mode Config-------------------
#define    QOS_TEST            0
#define    NET_TEST            0
#define    TEMP_TEST           0             //0:No Test  1：Test 5983  2：Test GMI
#define    MCU_SLEEP_ENABLE    1
#define    GMI_EN              0
#define    SLEEP_EN            1 
#define    LED_EN              1
#define    WIRELESS_TEST       1
//------------------Net Work Config--------------------
/****************更改beacon周期需要重新调的参数*************/
#define    BEACON_PERIOD       10000   //单位  100us
#define    BEFOR_BEACON_WAKE   BEACON_PERIOD-300   //在beacon前1.5ms时唤醒芯片

#define    KEEP_ALIVE_PERIOD   200  //单位50 ms  
#define    WHILE_TIMEOUT       BEACON_PERIOD*12
/**********************************************************/
#define    SLOT_LENGTH         60000     //单位us
#define    BACKOFF_DIV         10
#define    BACKOFF_PERIOD      SLOT_LENGTH/BACKOFF_DIV
#define    WAKE_TIME           4000    //SLEEP唤醒需要2ms

#define    DATAACK_TIMEOUT     1000      //ack接收超时，决定是否进入csma重发阶段

#define    CCA                 200     //载波监测阈值
#define    CSMA_BACKOFF_TIME   8
#define    CSMA_FAIL           1
#define    CSMA_SUCCESS        0
#endif