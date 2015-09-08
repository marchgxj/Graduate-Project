#include "common.h"
int   Receive_Timeout = 0;            //接收超时重启
uint32  Frame_Time = 0;                 //超帧内计时
uint16  Collect_Time = 0;               //采集计时
uint16 Keep_Alive_Count = 0;
uint16 Keep_Alive_Detect = 0;
uint16 Time_Count = 0;
uint8  ADCal_Flag = 0;
uint16 Direction_Count = 0;
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
    /*Direction_Count++;
    if(Direction_Count>300)
    {
        Direction_Count = 700;
    }*/
    //if(Direction_Count==700)
    {
        IdentifyCar();
        
        
        if(EndPointDevice.parking_state==NOCAR)
        {
            cal_count++;
            if(cal_count == 100)
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
        0:0、1：X轴AD
        1:2、3：Y轴AD
        2:4、5：VarianceM
        3:6、7：ExtremumValueMiddle
        4:8、9：ExtremumValue
        5:10：方差识别停车状态
        6:11：极值识别停车状态
        7:12、13：NoCar_Magnetic.XMiddle
        8:14、15：NoCar_Magnetic.YMiddle
        9:16、17：NoCar_Magnetic.Intensity
        10:18、19：NoCar_Magnetic.IntensityMiddle
        11:20:NoCar_Magnetic.Int_State
        12:21:EndpointDeviece.parking
        **********************************************/
        DataSendDraw[0] = MagneticUnit.XValue>>8;
        DataSendDraw[1] = MagneticUnit.XValue;
        DataSendDraw[2] = MagneticUnit.YValue>>8;
        DataSendDraw[3] = MagneticUnit.YValue;
        DataSendDraw[4] = MagneticUnit.Variance>>8;
        DataSendDraw[5] = MagneticUnit.Variance;
        DataSendDraw[6] = MagneticUnit.Ext_Middle>>8;
        DataSendDraw[7] = MagneticUnit.Ext_Middle;
        DataSendDraw[8] = MagneticUnit.Extremum>>8;
        DataSendDraw[9] = MagneticUnit.Extremum;
        DataSendDraw[10] = MagneticUnit.VarState;
        DataSendDraw[11] = MagneticUnit.ExtState;
        DataSendDraw[12] = MagneticUnit.XMiddle>>8;
        DataSendDraw[13] = MagneticUnit.XMiddle;
        DataSendDraw[14] = MagneticUnit.YMiddle>>8;
        DataSendDraw[15] = MagneticUnit.YMiddle;
        DataSendDraw[16] = MagneticUnit.Intensity>>8;
        DataSendDraw[17] = MagneticUnit.Intensity;
        DataSendDraw[18] = MagneticUnit.Int_Middle>>8;
        DataSendDraw[19] = MagneticUnit.Int_Middle;
        DataSendDraw[20] = MagneticUnit.IntState;
        DataSendDraw[21] = EndPointDevice.parking_state;

        A7139_WriteFIFO(DataSendDraw,TEST_LENGTH);
        delay_us(1);
        A7139_StrobeCmd(CMD_TX);
        delay_us(1);
        halLedToggle(1);
        while(GIO1)
        {}
    }
    
    
}