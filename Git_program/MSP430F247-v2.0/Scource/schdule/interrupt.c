#include "common.h"
int   Receive_Timeout = 0;            //接收超时重启
uint32  Frame_Time = 0;                 //超帧内计时
uint16  Collect_Time = 0;               //采集计时
uint16 Keep_Alive_Count = 0;
uint16 Keep_Alive_Detect = 0;
void Interrupt_Init(void)
{
    P1DIR &=~ pinGIO2.pin_bm;
    /*halDigioIntSetEdge(&pinGIO2, HAL_DIGIO_INT_RISING_EDGE);
    halDigioIntConnect(&pinGIO2, &EXTI9_5_IRQHandler);
    halDigioIntEnable(&pinGIO2);*/
    P1REN |= pinGIO2.pin_bm;                            // P1.4 pullup
    P1IE |= pinGIO2.pin_bm;                             // P1.4 interrupt enabled
    P1IES &= ~pinGIO2.pin_bm;                            // HAL_DIGIO_INT_RISING_EDGE
    P1IFG &= ~pinGIO2.pin_bm;                           // P1.4 IFG cleared
   
    RXMode();
    
}
int error_test = 0;
uint16 packet_count = 0;
uint8 rssi_what = 0;
#pragma vector=PORT1_VECTOR
__interrupt void port1_ISR(void)
{ 
    if(P1IFG & pinGIO2.pin_bm)
    {
        P1IFG &= ~pinGIO2.pin_bm;                           // P1.4 IFG cleared
        //DIS_TIMER1;
        A7139_ReadFIFO(DataRecvBuffer,MAX_PACK_LENGTH);
        delay_us(2);
        A7139_StrobeCmd(CMD_RX);
        delay_us(2);
        EndPointDevice.state = CMD_RX;
        
        if(PackValid())
        {
            switch (Unpack(DataRecvBuffer))
            {
              case BEACON_TYPE:
                Frame_Time = 0;
                //EN_TIMER1;
                TBCCTL0 = CCIE;
                PostTask(EVENT_BEACON_HANDLER);
                break;
              case JOINREQUESTACK_TYPE:
                PostTask(EVENT_JOINREQUESTACK_HANDLER);
                break;
              case REJOIN_TYPE:
                PostTask(EVENT_REJOIN_HANDLER);
                break;
            }
            halLedSet(1);
        }

        

        Receive_Timeout = 0;   
    } 
}

//100us
#pragma vector=TIMERB0_VECTOR
__interrupt void Timer_A (void)
{
   // TA1CCTL0 &= ~CCIFG;

    Frame_Time++;
    if(EndPointDevice.power == 0)
    {
        if(Frame_Time == BEACON_PERIOD*4)//4个周期内收不到Beacon，复位通讯
        {
            PostTask(EVENT_A7139_RESET);
            EN_INT;
            PostTask(EVENT_WAKE_A7139);
        }
        if(Frame_Time>(uint32)BEACON_PERIOD*8)//8个周期内收不到Beacon，重启单片机
        {
            REBOOT;
        }
        if(Frame_Time==BEFOR_BEACON_WAKE)//在接收beacon前使能中断
        {
            PostTask(EVENT_WAKE_A7139);
            EN_INT;
        }
#if (COLLECT_EN)                                //开启数据采集
        if(Start_Collect)
        {
            Collect_Time++;
            if(Collect_Time == COLLECT_PERIOD)              //0.5s采集一次
            {
                Collect_Time = 0;
                PostTask(EVENT_COLLECT_DATA);
                //  halLedToggle(2);
            } 
        }
#endif
    }
    else
    {
        
    }
}

//0.1s
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A0(void)
{
    TA0CCTL0 &= ~CCIFG;
    //halLedToggle(2);
    
    if(EndPointDevice.power == 0)
    //每个超帧都要发送时，Beacon接收超时则复位A7139
    {
        Receive_Timeout++;
        if(Receive_Timeout>300)
        {
            REBOOT;//很长时间没有收到数据，单片机全部复位.
        }
        /*else if(Receive_Timeout>15)
        {
            PostTask(EVENT_REJOIN_HANDLER);
            Receive_Timeout = 0;
        }*/
        else if(Receive_Timeout>50)
        {
            A7139_Reset();
            EN_INT;
        }
    }
//    else
//    {
//        Keep_Alive_Count++;
//        if(Keep_Alive_Count == KEEP_ALIVE_PERIOD)
//        {
//            Keep_Alive_Detect = 1;
//            Data_Change_Flag = 1;
//            Keep_Alive_Count = 0;
//        }
//#if (COLLECT_EN)                                //开启数据采集
//        if(Start_Collect)
//        {
//            Collect_Time++;
//            if(Collect_Time == COLLECT_PERIOD_L)              //1S采集一次
//            {
//                Collect_Time = 0;
//                PostTask(EVENT_COLLECT_DATA);
//                //  halLedToggle(2);
//            } 
//        }
//#endif
//    
//    }
    //测试低功耗时候使用
    else if(EndPointDevice.power == 1)                  
    {
        /*Receive_Timeout++;
        if(Receive_Timeout>=(EndPointDevice.cluster_innernum)*10)//若节点时间等于节点编号时发送数据
        {
            Receive_Timeout = 0;
            Data_Change_Flag = 1;
            Keep_Alive_Detect = 1;
            Frame_Time = 0;
            A7139_Deep_Wake();
            EN_INT;
            EN_TIMER1;
        }*/
        Cal_Time++;
        if(Cal_Time == CAL_PERIOD)
        {
            Cal_Time = 0;
            if((Parking_State == NOCAR))//如果是没有车状态 ，则进行校准
            {
                PostTask(EVENT_CALIBRATE_SENSOR);
            }
            
        }
        PostTask(EVENT_GET_VARIANCE);
    }
}