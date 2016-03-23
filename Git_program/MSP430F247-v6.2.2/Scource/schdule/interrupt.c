//******************
// FileName: interrupt.c
//
// Description:
//  interrupt function
//
// Author: 2016/1/19, by xiaoximi
//

#include "common.h"
int   Receive_Timeout = 0;            //接收超时重启
uint32  Frame_Time = 0;                 //超帧内计时
uint16  Collect_Time = 0;               //采集计时
uint16 Keep_Alive_Count = 0;
uint16 Keep_Alive_Detect = 0;
uint16 Int_Enable_Count = 0;
uint8 Send_Error_Flag = 0;
uint16 Send_Error_Count = 0;
uint8 Start_Sleep_Flag = 0;
uint16 Reset5983_Count = 0;
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
    uint8 pack_type = 0;
    if(P1IFG & pinGIO2.pin_bm)
    {
        P1IFG &= ~pinGIO2.pin_bm;                           // P1.4 IFG cleared
        //DIS_TIMER1;
        A7139_ReadFIFO(DataRecvBuffer,MAX_PACK_LENGTH);
        delay_us(2);
        A7139_StrobeCmd(CMD_RX);
        delay_us(2);
        EndPointDevice.state = CMD_RX;
        
        if(PackValid(DataRecvBuffer))
        {
            pack_type = Unpack(DataRecvBuffer);
            switch (pack_type)
            {
                case BEACON_TYPE:
                Frame_Time = 0;
                BeaconComing_Count = 0;
                EN_TIMER1;
                //TBCCTL0 = CCIE;
                PostTask(DataRecvBuffer,EVENT_BEACON_HANDLER);
                break;
                case JOINREQUESTACK_TYPE:
                PostTask(DataRecvBuffer,EVENT_JOINREQUESTACK_HANDLER);
                break;
            }
            halLedSet(1);
            Send_Error_Flag = 1;
            
        }
        
        
        
        Receive_Timeout = 0;   
    } 
}

//100us
#pragma vector=TIMERB0_VECTOR
__interrupt void Timer_A (void)
{
    TBCCTL0 &= ~CCIFG;
    
    Frame_Time++;
    //    if(EndPointDevice.power == 0)
    //    {
    //        if(Frame_Time==BEFOR_BEACON_WAKE)//在接收beacon前使能中断
    //        {
    //            PostTask(EVENT_WAKE_A7139);
    //            halLedSet(3);
    //            Int_Enable_Flag = 1;
    //            EN_INT;
    //        }
    //    }
}
uint8 DataSendDraw[TEST_LENGTH];
//00:x_middle_quene[0] 01：x_middle_quene[1] 02：x_middle_quene[2]
//10:y_middle_quene[0] 01：y_middle_quene[1] 02：y_middle_quene[2]
//20:z_middle_quene[0] 01：Z_middle_quene[1] 02：z_middle_quene[2]
uint8 switch_middle = 0; 
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
    9:16、17：MagneticUnit.distance
    10:18、19：xyz_middle_quene
    11:20:MagneticUnit.Int_State
    12:21:EndpointDeviece.parking
    13:22、23: update_middle_times
    14:24、25: XValue_parked_stable
    15:26、27: YValue_parked_stable
    16:28、29: Z轴AD
    17:30、31: MagneticUnit.XValue_Stable
    18:32、33: MagneticUnit.YValue_Stable
    19:34、35: MagneticUnit.ZValue_Stable
    20:36、37: toggle_distance_test
    21:38、39：Z_parked_distance
    22:40、41：ZValue_parked_stable
    23:42、43：MagneticUnit.ZMiddle
    24:44:switch_middle
    25:45:toggle_reason
    26:46、47:MagneticUnit.parked_distance
    27:48、49:diameterbuf
    28:50、51、52、53:Compatness
    29:54、55:perimeterbuf
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
    DataSendDraw[16] = MagneticUnit.distance>>8;
    DataSendDraw[17] = MagneticUnit.distance;
    if(switch_middle/10==0)
    {
        DataSendDraw[18] = x_middle_quene[switch_middle%10]>>8;
        DataSendDraw[19] = x_middle_quene[switch_middle%10];
    }
    else if(switch_middle/10==1)
    {
        DataSendDraw[18] = y_middle_quene[switch_middle%10]>>8;
        DataSendDraw[19] = y_middle_quene[switch_middle%10];
    }
    else if(switch_middle/10==2)
    {
        DataSendDraw[18] = z_middle_quene[switch_middle%10]>>8;
        DataSendDraw[19] = z_middle_quene[switch_middle%10];
    }
    DataSendDraw[20] = MagneticUnit.IntState;
    DataSendDraw[21] = EndPointDevice.parking_state;
    DataSendDraw[22] = update_middle_times>>8;
    DataSendDraw[23] = update_middle_times;
    DataSendDraw[24] = MagneticUnit.XValue_parked_stable>>8;
    DataSendDraw[25] = MagneticUnit.XValue_parked_stable;
    DataSendDraw[26] = MagneticUnit.YValue_parked_stable>>8;
    DataSendDraw[27] = MagneticUnit.YValue_parked_stable;
    DataSendDraw[28] = MagneticUnit.ZValue>>8;
    DataSendDraw[29] = MagneticUnit.ZValue;
    DataSendDraw[30] = MagneticUnit.XValue_Stable>>8;
    DataSendDraw[31] = MagneticUnit.XValue_Stable;
    DataSendDraw[32] = MagneticUnit.YValue_Stable>>8;
    DataSendDraw[33] = MagneticUnit.YValue_Stable;
    DataSendDraw[34] = MagneticUnit.ZValue_Stable>>8;
    DataSendDraw[35] = MagneticUnit.ZValue_Stable;
    DataSendDraw[36] = toggle_distance_test>>8;
    DataSendDraw[37] = toggle_distance_test;
    //    DataSendDraw[36] = perimeterbuf>>8;
    //    DataSendDraw[37] = perimeterbuf;
    DataSendDraw[38] = MagneticUnit.Z_parked_distance>>8;
    DataSendDraw[39] = MagneticUnit.Z_parked_distance;
    DataSendDraw[40] = MagneticUnit.ZValue_parked_stable>>8;
    DataSendDraw[41] = MagneticUnit.ZValue_parked_stable;
    DataSendDraw[42] = MagneticUnit.ZMiddle>>8;
    DataSendDraw[43] = MagneticUnit.ZMiddle;
    DataSendDraw[44] = switch_middle;
    DataSendDraw[45] = toggle_reason;
    DataSendDraw[46] = MagneticUnit.parked_distance>>8;
    DataSendDraw[47] = MagneticUnit.parked_distance;
    DataSendDraw[48] = diameterbuf_latest>>8;
    DataSendDraw[49] = diameterbuf_latest;
    DataSendDraw[50] = compactness_latest>>24;
    DataSendDraw[51] = compactness_latest>>16;
    DataSendDraw[52] = compactness_latest>>8;
    DataSendDraw[53] = compactness_latest;
    DataSendDraw[54] = perimeterbuf_latest>>8;
    DataSendDraw[55] = perimeterbuf_latest;
    
    A7139_WriteFIFO(DataSendDraw,TEST_LENGTH);
    delay_us(1);
    A7139_StrobeCmd(CMD_TX);
    delay_us(1);
    
    while(GIO1)
    {}
    if(switch_middle%10 == MIDDLE_QUENE_LENGTH-1)
    {
        switch_middle -= MIDDLE_QUENE_LENGTH-1;
        if(switch_middle < 20)
        {
            switch_middle += 10;
        }
        else
        {
            switch_middle = 0;
        }
    }
    else if(switch_middle%10 < MIDDLE_QUENE_LENGTH-1)
    {
        switch_middle++;
    }
    
}
//0.1s
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A0(void)
{
    TA0CCTL0 &= ~CCIFG;
    //    halLedToggle(2);
    //    return ;
#if (MCU_SLEEP_ENABLE == 1)
    if(Exit_Sleep == 1)
    {
        Exit_Sleep = 0;
        Start_Sleep_Flag = 0;
        halLedSet(4);
        LPM3_EXIT;
    }
    if(Start_Sleep_Flag == 1)
    {
        if(isEmpty()==1)
        {
            Start_Sleep_Flag = 0;
            PostTask(NULL,EVENT_MCUSLEEP_ENABLE);
        }
    }
#endif                 
    
    Keep_Alive_Count++;
#if NET_TEST == 1 || QOS_TEST == 1 || TEMP_TEST != 0
    if(Keep_Alive_Count > 40)
#else
        if(Keep_Alive_Count > KEEP_ALIVE_PERIOD)
#endif
            //if(Keep_Alive_Count == 5+EndPointDevice.pyh_address*2) 
        {
            Keep_Alive_Count = 0;
            Exit_Sleep  = 1;
#if QOS_TEST == 1
            DataPacket.ab_slot_num++;
#endif
            
            PostTask(NULL,EVENT_KEEPALIVE_SEND);
        }
    /****************复位*******************/
    if(Int_Enable_Flag == 1)
    {
        Int_Enable_Count++;
        if(Int_Enable_Count > 200)
        {
            Int_Enable_Count = 0;
            __disable_interrupt();
            halLedSet(4);
            LPM3_EXIT;
            ReJoinFlag = 1;
            Init_TQ();
            PostTask(NULL,EVENT_A7139_RESET);
        }
    }
    else
    {
        Int_Enable_Count = 0;
    }
    if(Send_Error_Flag == 1)
    {
        Send_Error_Count++;
        if(Send_Error_Count > 200)
        {
            Send_Error_Count = 0;
            __disable_interrupt();
            halLedSet(4);
            LPM3_EXIT;
            ReJoinFlag = 1;
            Init_TQ();
            PostTask(NULL,EVENT_A7139_RESET);
        }
    }
    else
    {
        Send_Error_Count = 0;
    }
    /*************************************/
    IdentifyCar();
    if(EndPointDevice.parking_state == CAR)
    {
        parking_time++;
        if(parking_time > 4000)
        {
             parking_time = 4000;
        }
    }
    else
    {
        parking_time = 0;
    }
}
