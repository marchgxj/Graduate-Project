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
    }
    //测试低功耗时候使用
    else if(EndPointDevice.power == 1)                  
    {
        //PostTask(EVENT_IDENTIFY_CAR);
        Keep_Alive_Count++;
        //if(Keep_Alive_Count == KEEP_ALIVE_PERIOD)
        //if(Keep_Alive_Count == 5+EndPointDevice.pyh_address*2) 
        if(Keep_Alive_Count == 100)
        {
            Keep_Alive_Count = 0;
            PostTask(EVENT_KEEPALIVE_SEND);
        }
        
        
        Collect_Period++;
        if(Quick_Collect==0)
        {
            if(Collect_Period==20)
            {
                Collect_Period = 0;
                //IdentifyCar();
                PostTask(EVENT_IDENTIFY_CAR);
            }
        }
        else
        {
            PostTask(EVENT_IDENTIFY_CAR);
            if(Collect_Period == 200)
            {
                Collect_Period = 0;
                Quick_Collect = 0;
            }
        }
    }
}