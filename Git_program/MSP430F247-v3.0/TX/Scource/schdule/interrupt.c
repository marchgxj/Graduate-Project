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
uint8 DataSendDraw[TEST_LENGTH];
uint16 cal_count = 0;
void TestSend()
{
    //数据包格式：
    /**********************************************
    0:0、1：X轴AD
    1:2、3：Y轴AD
    2:4、5：VarianceM
    3:6、7：ExtremumValueMiddle
    4:8、9：ExtremumValue
    5:10：方差识别停车状态
    6:11：极值识别停车状态
    7:12、13：MagneticUnit.XMiddle
    8:14、15：MagneticUnit.YMiddle
    9:16、17：MagneticUnit.Intensity
    10:18、19：NMagneticUnit.IntensityMiddle
    11:20:MagneticUnit.Int_State
    12:21:EndpointDeviece.parking
    13:22、23:XAve_Slpo
    14:24、25:YAve_Slop
    
    15:26、27:ZAve_Slop
    16:28、29:Z轴AD
    17:30、31:X轴AD GMI
    18:32、33:Y轴AD GMI
    19:34、35:XMiddle GMI
    20:36、37:YMiddle GMI
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
    DataSendDraw[22] = MagneticUnit.XAve_Slop>>8;
    DataSendDraw[23] = MagneticUnit.XAve_Slop;
    DataSendDraw[24] = MagneticUnit.YAve_Slop>>8;
    DataSendDraw[25] = MagneticUnit.YAve_Slop;
    DataSendDraw[26] = MagneticUnit.ZAve_Slop>>8;
    DataSendDraw[27] = MagneticUnit.ZAve_Slop;
    DataSendDraw[28] = MagneticUnit.ZValue>>8;
    DataSendDraw[29] = MagneticUnit.ZValue;
    DataSendDraw[30] = MagneticUnit.GMI_XValue>>8;
    DataSendDraw[31] = MagneticUnit.GMI_XValue;
    DataSendDraw[32] = MagneticUnit.GMI_YValue>>8;
    DataSendDraw[33] = MagneticUnit.GMI_YValue;
    DataSendDraw[34] = MagneticUnit.GMI_XMiddle>>8;
    DataSendDraw[35] = MagneticUnit.GMI_XMiddle;
    DataSendDraw[36] = MagneticUnit.GMI_YMiddle>>8;
    DataSendDraw[37] = MagneticUnit.GMI_YMiddle;
    
    A7139_WriteFIFO(DataSendDraw,TEST_LENGTH);
    delay_us(1);
    A7139_StrobeCmd(CMD_TX);
    delay_us(1);
    
    while(GIO1)
    {}
}
//1s

#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A0(void)
{
    
    
//    Direction_Count++;
//    if(Direction_Count>300)
//    {
//        Direction_Count = 700;
//    }
//    if(Direction_Count==700)
    {
        
        Quick_Collect = 1;
        
        
        Collect_Period++;
        if(Quick_Collect==0)
        {
            if(Collect_Period==20)
            {
                Collect_Period = 0;
                IdentifyCar();
                TestSend();
            }
        }
        else
        {
            IdentifyCar();
            TestSend();
            if(Collect_Period == 200)
            {
                Collect_Period = 0;
                //Quick_Collect = 0;
            }
        }
        
        
        
//        if(EndPointDevice.parking_state==NOCAR)
//        {
//            cal_count++;
//            if(cal_count == 100)
//            {
//                cal_count = 0;
//                ADCal_Flag = 1;
//            }
//        }
//        else
//        {
//            cal_count = 0;
//        }
        
        
        
    }
    
    
}