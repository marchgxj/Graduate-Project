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
uint8 Camera1_Statue_Buf = 0;
uint8 Camera1_Statue = 0;
uint8 Camera2_Statue = 0;
uint8 Direction = 0;
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
        Direction = DataRecvBuffer[3];
        if(Direction == 1)
        {
            Camera1_Statue = DataRecvBuffer[2];
            halLedSet(1);
            SendPack();
            Receive_Timeout = 0;  
            PostTask(EVENT_SWITCH_CAMERA);
        }
        else if(Direction == 2)
        {
            Camera2_Statue = DataRecvBuffer[2];
            halLedSet(1);
            SendPack();
            Receive_Timeout = 0;  
            PostTask(EVENT_SWITCH_CAMERA);
        }
        RXMode();
        
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
    halLedToggle(2);
    
}