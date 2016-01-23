//******************
// FileName: interrupt.c
//
// Description:
//  interrupt function
//
// Author: 2016/1/19, by xiaoximi
//

#include "common.h"
int   Receive_Timeout = 0;            //���ճ�ʱ����
uint32  Frame_Time = 0;                 //��֡�ڼ�ʱ
uint16  reset_HMC5983_count = 0;
uint16  in_quick_collect_count = 0;
uint8 force_quit_quick_collect = 0;

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
    //���ݰ���ʽ��
    /**********************************************
    0:0��1��X��AD
    1:2��3��Y��AD
    2:4��5��VarianceM
    3:6��7��ExtremumValueMiddle
    4:8��9��ExtremumValue
    5:10������ʶ��ͣ��״̬
    6:11����ֵʶ��ͣ��״̬
    7:12��13��MagneticUnit.XMiddle
    8:14��15��MagneticUnit.YMiddle
    9:16��17��MagneticUnit.Intensity
    10:18��19��NMagneticUnit.IntensityMiddle
    11:20:MagneticUnit.Int_State
    12:21:EndpointDeviece.parking
    13:22��23: XAve_Slpo
    14:24��25: YAve_Slop
    
    15:26��27: ZAve_Slop
    16:28��29: Z��AD
    17:30��31: MagneticUnit.XValue_Stable
    18:32��33: MagneticUnit.YValue_Stable
    19:34��35: MagneticUnit.ZValue_Stable
    20:36��37: perimeterbuf
    21:38��39��EndPointDevice.vlotage
    22:40��41��EndPointDevice.temperature
    23:42��43��MagneticUnit.ZMiddle
    24:44:XValue_Parking
    25:45:YValue_Parking
    26:46��47:Infrared
    27:48��49:diameterbuf
    28:50��51��52��53:Compatness
    29:54��55:Storage_data_length
    **********************************************/
//    MagneticUnit.XValue = 1;
//    MagneticUnit.YValue = 2;
//    MagneticUnit.Variance = 3;
//    MagneticUnit.Ext_Middle = 4;
//    MagneticUnit.Extremum = 5;
//    MagneticUnit.VarState = 6;
//    MagneticUnit.ExtState = 7;
//    MagneticUnit.XMiddle = 8;
//    MagneticUnit.YMiddle = 9;
//    MagneticUnit.distance = 10;
//    MagneticUnit.Int_Middle = 11;
//    MagneticUnit.IntState = 12;
//    EndPointDevice.parking_state = 13;
//    MagneticUnit.XAve_Slop = 14;
//    MagneticUnit.YAve_Slop = 15;
//    MagneticUnit.ZAve_Slop = 16;
//    MagneticUnit.ZValue = 17;
//    MagneticUnit.XValue_Stable = 18;
//    MagneticUnit.YValue_Stable = 19;
//    MagneticUnit.ZValue_Stable = 20;
//    MagneticUnit.GMI_YMiddle = 21;
//    EndPointDevice.vlotage = 22;
//    EndPointDevice.temperature = 23;
//    MagneticUnit.ZMiddle = 24;
//    XValue_Parking = 25;
//    YValue_Parking = 26;
//    MagneticUnit.infrared = 27;
//    MagneticUnit.distance = 28;
//    MagneticUnit.compatness = 29;
//    storage_count_send = 30;
    
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
    DataSendDraw[16] = MagneticUnit.distance>>8;
    DataSendDraw[17] = MagneticUnit.distance;
//    DataSendDraw[16] = MagneticUnit.Intensity>>8;
//    DataSendDraw[17] = MagneticUnit.Intensity;
    MagneticUnit.Int_Middle = 5000;
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
    DataSendDraw[30] = MagneticUnit.XValue_Stable>>8;
    DataSendDraw[31] = MagneticUnit.XValue_Stable;
    DataSendDraw[32] = MagneticUnit.YValue_Stable>>8;
    DataSendDraw[33] = MagneticUnit.YValue_Stable;
    DataSendDraw[34] = MagneticUnit.ZValue_Stable>>8;
    DataSendDraw[35] = MagneticUnit.ZValue_Stable;
    DataSendDraw[36] = perimeterbuf>>8;
    DataSendDraw[37] = perimeterbuf;
    DataSendDraw[38] = EndPointDevice.vlotage>>8;
    DataSendDraw[39] = EndPointDevice.vlotage;
    DataSendDraw[40] = EndPointDevice.temperature>>8;
    DataSendDraw[41] = EndPointDevice.temperature;
    DataSendDraw[42] = MagneticUnit.ZMiddle>>8;
    DataSendDraw[43] = MagneticUnit.ZMiddle;
    DataSendDraw[44] = XValue_Parking;
    DataSendDraw[45] = YValue_Parking;
    DataSendDraw[46] = MagneticUnit.infrared>>8;
    DataSendDraw[47] = MagneticUnit.infrared;
    DataSendDraw[48] = diameterbuf>>8;
    DataSendDraw[49] = diameterbuf;
    DataSendDraw[50] = MagneticUnit.compatness>>24;
    DataSendDraw[51] = MagneticUnit.compatness>>16;
    DataSendDraw[52] = MagneticUnit.compatness>>8;
    DataSendDraw[53] = MagneticUnit.compatness;
    DataSendDraw[54] = storage_count_send>>8;
    DataSendDraw[55] = storage_count_send;
    
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
    Collect_Period++;
    reset_HMC5983_count++;
    if(Quick_Collect==0)
    {
        in_quick_collect_count = 0;
        if(force_quit_quick_collect != 0)
        {
            force_quit_quick_collect++;
            if(force_quit_quick_collect > 5)
            {
                force_quit_quick_collect = 0;
            }
        }
        
        if(Collect_Period>20)
        {
            Collect_Period = 0;
            IdentifyCar();
            TestSend();
            //PostTask(EVENT_IDENTIFY_CAR);
        }
    }
    else
    {
        //PostTask(EVENT_IDENTIFY_CAR);
        IdentifyCar();
        TestSend();
        in_quick_collect_count++;
        //if(in_quick_collect_count>MAX_QUICK_COLLECT_TIME) //�����
        if(in_quick_collect_count>1200)
        {
            in_quick_collect_count = 0;
            force_quit_quick_collect = 1;
            Collect_Period = 301;
            
        }
        if(Collect_Period > 300)
        {
            Collect_Period = 0;
            Quick_CollectM = Quick_Collect;
            Quick_Collect = 0;
         }
    }
    if(reset_HMC5983_count == HMC5983_RESET_PERIOD) //ʮ����
    {
        reset_HMC5983_count = 0;
        Init_5983();
    }
    
}