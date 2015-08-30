#include "common.h"
int   Receive_Timeout = 0;            //接收超时重启
uint32  Frame_Time = 0;                 //超帧内计时
uint16  Collect_Time = 0;               //采集计时
uint16 Keep_Alive_Count = 0;
uint16 Keep_Alive_Detect = 0;
uint16 Time_Count = 0;
uint8  ADCal_Flag = 0;
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

    
}

//1s
uint8 DataSendDraw[TEST_LENGTH];
uint16 cal_count = 0;
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A0(void)
{
    IdentifyCar();
    if(EndPointDevice.parking_state == NOCAR)
    {
        cal_count++;
        if(cal_count == 600)
        {
            cal_count = 0;
            ADCal_Flag = 1;
        }
    }
    else
    {
        cal_count = 0;
    }
    
    
    //数据包格式：
    /**********************************************
    0、1：X轴AD
    2、3：Y轴AD
    4、5：VarianceM
    6、7：ExtremumValueMiddle
    8、9：ExtremumValue
    10：方差识别停车状态
    11：极值识别停车状态
    12、13：AD_middle_valueX
    14、15：AD_middle_valueY
    **********************************************/
    DataSendDraw[0] = XValue>>8;
    DataSendDraw[1] = XValue;
    DataSendDraw[2] = YValue>>8;
    DataSendDraw[3] = YValue;
    DataSendDraw[4] = VarianceM>>8;
    DataSendDraw[5] = VarianceM;
    DataSendDraw[6] = ExtremumValueMiddle>>8;
    DataSendDraw[7] = ExtremumValueMiddle;
    DataSendDraw[8] = ExtremumValue>>8;
    DataSendDraw[9] = ExtremumValue;
    DataSendDraw[10] = Var_state;
    DataSendDraw[11] = Ext_state;
    DataSendDraw[12] = AD_middle_valueX>>8;
    DataSendDraw[13] = AD_middle_valueX;
    DataSendDraw[14] = AD_middle_valueY>>8;
    DataSendDraw[15] = AD_middle_valueY;

    

    A7139_WriteFIFO(DataSendDraw,TEST_LENGTH);
    delay_us(1);
    A7139_StrobeCmd(CMD_TX);
    delay_us(1);
    halLedToggle(1);
    while(GIO1)
    {}
}