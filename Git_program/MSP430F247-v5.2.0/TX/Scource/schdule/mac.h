#ifndef _DEVICE_h_
#define _DEVICE_h_
/****************更改beacon周期需要重新调的参数*************/
#define    BEACON_PERIOD     5000
#define    BEFOR_BEACON_WAKE BEACON_PERIOD-300   //在beacon前1.5ms时唤醒芯片
#define    KEEP_ALIVE_PERIOD   60  //单位s

/**********************************************************/
#define    SLOT_LENGTH     20000     //单位us
#define    BACKOFF_DIV     10
#define    BACKOFF_PERIOD  SLOT_LENGTH/BACKOFF_DIV
#define    WAKE_TIME         4000    //SLEEP唤醒需要2ms

#define    DATAACK_TIMEOUT 300      //ack接收超时，决定是否进入csma重发阶段

#define    SLEEP_EN        1 

#define    CCA   100     //载波监测阈值
#define    CSMA_BACKOFF_TIME 8
#define    CSMA_FAIL    1
#define    CSMA_SUCCESS 0

#define    BROADCAST    0xFF
#define    ROOT     1
#define    DEVICE   2

#define    ACK_EN    1
#define    ACK_DIS   0

//packet type define
#define    BEACON_TYPE              1
#define    JOINREQUEST_TYPE         2
#define    JOINREQUESTACK_TYPE      3
#define    JOINREQUESTACKOK_TYPE    4
#define    DATA_TYPE                5
#define    DATAACK_TYPE             6
#define    REJOIN_TYPE              7


//packet length
#define    BEACON_PACK_LENGTH            7
#define    JOINREQUEST_PACK_LENGTH       6  
#define    JOINREQUESTACK_PACK_LENGTH    8  
#define    JOINREQUESTACKOK_PACK_LENGTH  6  
#define    DATA_PACK_LENGTH              9  
#define    DATAACK_PACK_LENGTH           8

typedef struct{
    uint8 cluster_id;		        //簇ID
    uint8 cluster_innernum;		//簇内编号
    uint8 des_cluster_id;		//目的簇ID
    uint8 des_cluster_innernum;		//目的簇编号
    uint8 channel;			//当前信道
    uint8 backup_channel;		//备用信道
    uint16 pyh_address;			//物理地址
    uint8 device_type;                  //设备类型
    uint8 free_node;                    //当前中继空闲节点数
    uint8 connected;                    //入网状态
    uint8 csma_length;                  //CMSA时隙个数
    uint8 state;                        //当前状态
    uint8 time_stamp;                   //时间戳
    uint8 data_ack;                     //数据ack
    uint8 power;			//是否开启低功耗
    uint8 parking_state;                //停车状态
    uint8 parking_state_m;              //上一次停车状态
    uint8 cmd;                          //命令
    uint16 vlotage;                     //电量
    uint16 temperature;                 //温度
}EndPointDeviceStruct;

extern EndPointDeviceStruct EndPointDevice;
extern uint8 Unpack(uint8 *type);
extern void BeaconHandler(uint8 beacon[]);
extern uint8 PackValid(void);
extern uint8 SendByCSMA(u8 *buff,uint8 length);



extern uint8 DataRecvBuffer[MAX_PACK_LENGTH];
extern uint8 DataSendBuffer[MAX_PACK_LENGTH];
extern uint8 Power_Mode;


#endif