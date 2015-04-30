#ifndef _SCAN_h_
#define _SCAN_h_
#include "common.h"
#define    CHANNEL_1     0
#define    CHANNEL_2     1
#define    CHANNEL_3     2
#define    CHANNEL_4     3
#define    CHANNEL_5     4
#define    CHANNEL_6     5
#define    CHANNEL_7     6
#define    CHANNEL_8     7
#define    CHANNEL_9     8
#define    CHANNEL_10    9
#define    CHANNEL_11    10
#define    CHANNEL_12    11
#define    CHANNEL_13    12
#define    CHANNEL_14    13
#define    CHANNEL_15    14
#define    CHANNEL_16    15

#define    SCAN_PACK_LENGTH  5

typedef struct {
    uint8 channel_num;     // �ŵ����
    uint8 channel_rssi;    // �ŵ��ź�ǿ��
    uint8 channel_free;   // Sink���нڵ���
}ChannelTable;


extern ChannelTable Scan_Channel(uint8 startch,uint8 endch);
extern void Scan_Channel1();



#endif