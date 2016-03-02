//******************
// FileName: detect.c
//
// Description:
//  Detect Car Process
//
// Author: 2016/1/19, by xiaoximi
//

#include "common.h"

uint8 VState1_Count = 0;
uint8 VState2_Count = 0;
uint8 VState3_Count = 0;
uint8 EState1_Count = 0;
uint8 EState2_Count = 0;
uint8 EState3_Count = 0;
uint8 IState1_Count = 0;
uint8 IState2_Count = 0;
uint8 IState3_Count = 0;
MagneticStruct MagneticUnit;
FilterStruct FilterData[FILTER_LENGTH];
uint8 Filter_Count = 0;
uint16 CarStableCount = 0;
uint16 NoCarStableCount = 0;
uint16 ExtChangeCount = 0;
uint16 VarChangeCount = 0;
uint16 IntChangeCount = 0;
int Ext_Minus,Var_Minus,Int_Minus;

uint16 Collect_Period = 0;
uint8 Quick_Collect = 0;
uint8 Quick_CollectM = 0;
FilterStruct SlopData[SLOP_LENGTH];
uint8 Slop_Count = 0;
uint8 ReCal_Count = 0;
uint16 XReCal = 0;
uint16 YReCal = 0;
uint16 ZReCal = 0;

uint16 Ext_Threshold = 0;
uint16 Int_Threshold = 0;
uint16 Var_Threshold = 0;
uint16 Dis_Threshold = 0;

uint32 compatness_memory = 0;
uint16 compatness_stable_count = 0;
uint16 yvalue_memory = 0;
uint16 zvalue_memory = 0;

uint16 x_middle_quene[MIDDLE_QUENE_LENGTH];
uint16 y_middle_quene[MIDDLE_QUENE_LENGTH];
uint16 z_middle_quene[MIDDLE_QUENE_LENGTH];
uint8 middle_quene_count = 1;

uint16 parking_time = 0;

//0: detection results is final result
//1: oppsite of detection results is final result
uint8 reverse_flag = 0; 

void VarianceMultiState(uint16 state1,uint16 state2,uint16 state3)
{
    // FunctionName: VarianceMulitState
    //{
    // Parameter:state1,state2,state3 
    //           three counters of state machine
    //
    // Return:none
    //
    // Description:
    //  z axis state machine
    //
    // Created: 2016/3/1
    //
    // Author:xiaoximi
    //}

    MagneticUnit.VarStateM = MagneticUnit.VarState;
    if(MagneticUnit.Variance > Var_Threshold)
    {
        if(MagneticUnit.VarState!=CAR)
        {
            VState1_Count++;
            if(VState1_Count>state1)
            {
                MagneticUnit.VarState = CAR;
                VState1_Count = 0;
            }
            else
            {
                MagneticUnit.VarState = NOCAR2CAR;
            }
        }
        
    }
    else
    {
        if(MagneticUnit.VarState == NOCAR2CAR)
        {
            VState2_Count++;
            if(VState2_Count>state2)
            {
                MagneticUnit.VarState = NOCAR;
                VState2_Count = 0;
                VState1_Count = 0;
            }
        }
        else
        {
            if(MagneticUnit.VarState != NOCAR)
            {
                VState3_Count++;
                if(VState3_Count>state3)
                {
                    MagneticUnit.VarState = NOCAR;
                    VState3_Count = 0;
                }
            }
        }
    }
}

void ExtremumMultiState(uint16 state1,uint16 state2,uint16 state3)
{
    // FunctionName: ExtremumMulitiState
    //{
    // Parameter:state1,state2,state3 
    //           three counters of state-machine
    //
    // Return:none
    //
    // Description:
    //  XZ axes state-machine
    //
    // Created: 2016/3/1
    //
    // Author:xiaoximi
    //}

    MagneticUnit.ExtStateM = MagneticUnit.ExtState;
    if(MagneticUnit.Extremum > Ext_Threshold)
    {
        if(MagneticUnit.ExtState!=CAR)
        {
            EState1_Count++;
            if(EState1_Count>state1)
            {
                MagneticUnit.ExtState = CAR;
                EState1_Count = 0;
            }
            else
            {
                MagneticUnit.ExtState = NOCAR2CAR;
            }
        }
        
    }
    else
    {
        if(MagneticUnit.ExtState == NOCAR2CAR)
        {
            EState2_Count++;
            if(EState2_Count>state2)
            {
                MagneticUnit.ExtState = NOCAR;
                EState2_Count = 0;
                EState1_Count = 0;
            }
        }
        else
        {
            if(MagneticUnit.ExtState != NOCAR)
            {
                EState3_Count++;
                if(EState3_Count>state3)
                {
                    MagneticUnit.ExtState = NOCAR;
                    EState3_Count = 0;
                }
            }
            
        }
        
    }
    //Ext_state = MultiState(ExtremumValue,EXT_THRESHOLD);
}

void IntensityMultiState(uint16 state1,uint16 state2,uint16 state3)
{
    // FunctionName: IntensityMultiState
    //{
    // Parameter:state1,state2,state3 
    //           three counters of state-machine
    //
    // Return:none 
    //
    // Description:
    //  XYZ axes state-machine
    //
    // Created: 2016/3/1
    //
    // Author:xiaoximi
    //}

    uint16 intensity = 0;
    MagneticUnit.IntStateM = MagneticUnit.IntState;
    intensity = abs(MagneticUnit.Intensity - MagneticUnit.Int_Middle);
    if(intensity > Int_Threshold)
    {
        if(MagneticUnit.IntState!=CAR)
        {
            IState1_Count++;
            if(IState1_Count>state1)
            {
                MagneticUnit.IntState = CAR;
                IState1_Count = 0;
            }
            else
            {
                MagneticUnit.IntState = NOCAR2CAR;
            }
        }
        
    }
    else
    {
        if(MagneticUnit.IntState == NOCAR2CAR)
        {
            IState2_Count++;
            if(IState2_Count>state2)
            {
                MagneticUnit.IntState = NOCAR;
                IState2_Count = 0;
                IState1_Count = 0;
            }
        }
        else
        {
            if(MagneticUnit.IntState != NOCAR)
            {
                IState3_Count++;
                if(IState3_Count>state3)
                {
                    MagneticUnit.IntState = NOCAR;
                    IState3_Count = 0;
                }
            }
        }
    }

}

void DistanceMultiState(uint16 state1,uint16 state2,uint16 state3)
{
    // FunctionName: DistanceMuliState
    //{
    // Parameter:state1,state2,state3 
    //           three counters of state-machine
    //
    // Return:none
    //
    // Description:
    //  XYZ axes state-machine
    //
    // Created: 2016/3/1
    //
    // Author:xiaoximi
    //}

    if(MagneticUnit.distance > Dis_Threshold)
    {
        if(MagneticUnit.IntState!=CAR)
        {
            IState1_Count++;
            if(IState1_Count>state1)
            {
                MagneticUnit.IntState = CAR;
                IState1_Count = 0;
            }
            else
            {
                MagneticUnit.IntState = NOCAR2CAR;
            }
        }
        
    }
    else
    {
        if(MagneticUnit.IntState == NOCAR2CAR)
        {
            IState2_Count++;
            if(IState2_Count>state2)
            {
                MagneticUnit.IntState = NOCAR;
                IState2_Count = 0;
                IState1_Count = 0;
            }
        }
        else
        {
            if(MagneticUnit.IntState != NOCAR)
            {
                IState3_Count++;
                if(IState3_Count>state3)
                {
                    MagneticUnit.IntState = NOCAR;
                    IState3_Count = 0;
                }
            }
        }
    }

}

void Filter(uint16 xvalue,uint16 yvalue)
{
    // FunctionName: Filter
    //{
    // Parameter:xvalue:x axis value  yvalue:y axis value
    //
    // Return:none
    //
    // Description:
    //  x,y axes average filter
    //
    // Created: 2016/3/1
    //
    // Author:xiaoximi
    //}

    uint16 xbuf,ybuf;
    xbuf = 0;
    ybuf = 0;
    FilterData[Filter_Count].xvalue = xvalue;
    FilterData[Filter_Count].yvalue = yvalue;
    Filter_Count++;
    if(Filter_Count==FILTER_LENGTH)
    {
        Filter_Count = 0;
    }
    
    for(int i=0;i<FILTER_LENGTH;i++)
    {
        xbuf += FilterData[i].xvalue;
        ybuf += FilterData[i].yvalue;
    }
    MagneticUnit.XValue = xbuf/FILTER_LENGTH;
    
    
}

void GetSlop(uint16 xvalue,uint16 yvalue,uint16 zvalue)
{
    // FunctionName: GetSlop
    //{
    // Parameter:xvalue,yvalue,zvalue three axes value
    //
    // Return:none
    //
    // Description:
    //  get the slop of magnetic field. the stability of magnetic field
    //
    // Created: 2016/3/1
    //
    // Author:xiaoximi
    //}

    uint8 i=0;
    int xslopbuf = 0;
    int yslopbuf = 0;
    int zslopbuf = 0;
    SlopData[Slop_Count].xvalue = xvalue;
    SlopData[Slop_Count].yvalue = yvalue;
    SlopData[Slop_Count].zvalue = zvalue;
    Slop_Count++;
    if(Slop_Count==SLOP_LENGTH)
    {
        Slop_Count = 0;
    }
    for(i=1;i<SLOP_LENGTH;i++)
    {
        xslopbuf +=(int)((SlopData[i].xvalue-SlopData[0].xvalue))/i;
        yslopbuf +=(int)((SlopData[i].yvalue-SlopData[0].yvalue))/i;
        zslopbuf +=(int)((SlopData[i].zvalue-SlopData[0].zvalue))/i;
    }
    MagneticUnit.XAve_Slop = xslopbuf/SLOP_LENGTH;
    MagneticUnit.YAve_Slop = yslopbuf/SLOP_LENGTH;
    MagneticUnit.ZAve_Slop = zslopbuf/SLOP_LENGTH;
    
    if((abs(MagneticUnit.XAve_Slop)>5)||(abs(MagneticUnit.YAve_Slop)>5)||(abs(MagneticUnit.ZAve_Slop)>10))
    {
        if(force_quit_quick_collect == 0)
        {
            Quick_CollectM = Quick_Collect;
            Quick_Collect = 1;
            Collect_Period = 0;
        }
        
        
    }
}

void quickCalibrate()
{
// FunctionName: quickCalibrate
//{
// Parameter:none
//
// Return:none
//
// Description:
//  update middle value and reset state machine
//
// Created: 2016/3/2
//
// Author:xiaoximi
//}
    uint16 ADX[4];
    uint16 ADY[4];
    uint16 ADZ[4];
    uint16 ADvalueX=0;
    uint16 ADvalueY=0;
    uint16 ADvalueZ=0;
    uint8 i=0;
    uint16 xvariance = 0;
    uint16 yvariance = 0;
    uint16 zvariance = 0;
    uint16 xave = 0;
    uint16 yave = 0;
    uint16 zave = 0;
    for(i=0;i<4;i++)
    {
        Multi_Read_HMC(&ADvalueX,&ADvalueY,&ADvalueZ);
        ADX[i] = ADvalueX;
        ADY[i] = ADvalueY;
        ADZ[i] = ADvalueZ;
        xave += ADvalueX;
        yave += ADvalueY;
        zave += ADvalueZ;
    }
    xave = xave >> 2;
    yave = yave >> 2;
    zave = zave >> 2;
    xvariance = getVariance(ADX,4);
    yvariance = getVariance(ADY,4);
    zvariance = getVariance(ADZ,4);
    if((xvariance<5)&&(yvariance<5)&&(zvariance<15))
    {
        MagneticUnit.XMiddle = xave;
        MagneticUnit.YMiddle = yave;
        MagneticUnit.ZMiddle = zave;
        MagneticUnit.XValue_Stable = MagneticUnit.XMiddle;
        MagneticUnit.YValue_Stable = MagneticUnit.YMiddle;
        MagneticUnit.ZValue_Stable = MagneticUnit.ZMiddle;
        MagneticUnit.Ext_Middle = abs(MagneticUnit.ZMiddle-MagneticUnit.YMiddle);
        MagneticUnit.Int_Middle = sqrt_16(
            (((uint32)MagneticUnit.XMiddle*(uint32)MagneticUnit.XMiddle)+
             ((uint32)MagneticUnit.YMiddle*(uint32)MagneticUnit.YMiddle))+
             ((uint32)MagneticUnit.ZMiddle*(uint32)MagneticUnit.ZMiddle));
        MagneticUnit.CarExtremum = 0;
        MagneticUnit.CarIntensity = 0;
        MagneticUnit.CarVariance = 0;
    }
}

uint8 ReCal()
{
    // FunctionName: ReCal
    //{
    // Parameter:none
    //
    // Return:none
    //
    // Description:
    //  recalibrate three times after start, because the first several times 
    //  could get wrong value of magnetic field
    //
    // Created: 2016/3/1
    //
    // Author:xiaoximi
    //}

    
    
    if(ReCal_Count<3)
    {
        //SampleChannel(&MagneticUnit.GMI_XValue,&MagneticUnit.GMI_YValue);
        ReCal_Count++;
        XReCal+=MagneticUnit.XValue;
        YReCal+=MagneticUnit.YValue;
        ZReCal+=MagneticUnit.ZValue;
        //GMI_XReCal+=MagneticUnit.GMI_XValue;
       //GMI_YReCal+=MagneticUnit.GMI_YValue;
        return 0;
    }
    else if(ReCal_Count==3)
    {
        MagneticUnit.XMiddle = XReCal/3;
        MagneticUnit.YMiddle = YReCal/3;
        MagneticUnit.ZMiddle = ZReCal/3;
        MagneticUnit.XValue_Stable = MagneticUnit.XMiddle;
        MagneticUnit.YValue_Stable = MagneticUnit.YMiddle;
        MagneticUnit.ZValue_Stable = MagneticUnit.ZMiddle;
        MagneticUnit.XMiddleMF = MagneticUnit.XMiddle;
        MagneticUnit.YMiddleMF = MagneticUnit.YMiddle;
        MagneticUnit.ZMiddleMF = MagneticUnit.ZMiddle;
        MagneticUnit.Int_Middle = MagneticUnit.Intensity;
        MagneticUnit.Ext_Middle = abs(MagneticUnit.ZMiddle-MagneticUnit.YMiddle);
        ReCal_Count = 20;
        Quick_CollectM = Quick_Collect;
        Quick_Collect = 0;
        MagneticUnit.ExtState = 0;
        MagneticUnit.IntState = 0;
        MagneticUnit.VarState = 0;
        halLedClearAll();
        delay_ms(10);
        halLedSetAll();
        delay_ms(10);
        halLedClearAll();
        delay_ms(10);
        halLedSetAll();
        delay_ms(10);
        halLedClearAll();
        delay_ms(10);
    }
    return 1;
    
}

void GetVoltage()
{
    // FunctionName: GetVoltage
    //{
    // Parameter:none
    //
    // Return:none
    //
    // Description:
    //  get the voltage of battery
    //
    // Created: 2016/3/1
    //
    // Author:xiaoximi
    //}

    SampleVoltage(&EndPointDevice.vlotage,&EndPointDevice.temperature);
    if((EndPointDevice.vlotage<LOWPOWER_THRESHOLD_HIGH)&&((EndPointDevice.vlotage>LOWPOWER_THRESHOLD_LOW)))
    {
        EndPointDevice.parking_state = LOWPOWER;
    }
}

uint8 compactnessStable()
{
    // FunctionName: compactnessStable
    //{
    // Parameter:none
    //
    // Return:bool if stable return 1  else return 0
    //
    // Description:
    //  to determine whether compactness is stable
    //
    // Created: 2016/3/1
    //
    // Author:xiaoximi
    //}

    if(compatness_memory == MagneticUnit.compatness)
    {
        compatness_stable_count++;
    }
    else
    {
        compatness_stable_count = 0;
    }
    if(compatness_stable_count > 70)
    {
        compatness_stable_count = 0;
        return 1;
    }
    compatness_memory = MagneticUnit.compatness;
    return 0;
}

void IdentifyCar()
{
    // FunctionName: IdentifyCar
    //{
    // Parameter:none
    //
    // Return:none
    //
    // Description:
    //  detection algorithm
    //
    // Created: 2016/3/1
    //
    // Author:xiaoximi
    //}

    halLedSet(2);

    yvalue_memory = MagneticUnit.YValue;
    zvalue_memory = MagneticUnit.ZValue;
    Multi_Read_HMC(&MagneticUnit.XValue,&MagneticUnit.YValue,&MagneticUnit.ZValue);
    //MagneticUnit.infrared = getInfrared();
    
    
    if(ReCal())
    {
        MagneticUnit.compatness = getCompatness(
                                                MagneticUnit.YValue,
                                                MagneticUnit.ZValue,
                                                yvalue_memory ,
                                                zvalue_memory
                                                    );
    }
    else
    {
        diameterbuf = 0;
        perimeterbuf = 0;
        max_x = 0;
        max_y = 0;
    }
    GetSlop(MagneticUnit.XValue,MagneticUnit.YValue,MagneticUnit.ZValue);
    //Filter(MagneticUnit.XValue,MagneticUnit.YValue);
    MagneticUnit.Variance = getAbsoluteValue();
    MagneticUnit.Extremum = getExtremum();
    //MagneticUnit.Intensity = getIntensity();
    
    MagneticUnit.distance = getEuclideanMetric(
                                               MagneticUnit.XValue_Stable,
                                               MagneticUnit.YValue_Stable,
                                               MagneticUnit.ZValue_Stable,
                                               MagneticUnit.XMiddle,
                                               MagneticUnit.YMiddle,
                                               MagneticUnit.ZMiddle
                                                   );
    if(compactnessStable())
    {
        diameterbuf = 0;
        perimeterbuf = 0;
        max_x = 0;
        max_y = 0;
    }
    
    if(EndPointDevice.parking_state == NOCAR)
    {
        if((MagneticUnit.compatness > COM_THRESHOLD)&&(diameterbuf>100))
        {
            MagneticUnit.XValue_Stable = MagneticUnit.XValue;
            MagneticUnit.YValue_Stable = MagneticUnit.YValue;
            MagneticUnit.ZValue_Stable = MagneticUnit.ZValue;
        }
    }
    else
    {
        if((MagneticUnit.compatness > COM_THRESHOLD/3))
        {
            MagneticUnit.XValue_Stable = MagneticUnit.XValue;
            MagneticUnit.YValue_Stable = MagneticUnit.YValue;
            MagneticUnit.ZValue_Stable = MagneticUnit.ZValue;
        }
    }
    
    
    if(Quick_Collect == 1)
    {
        VarianceMultiState(10,20,25);
        ExtremumMultiState(10,20,30);
        //IntensityMultiState(20,20,30);
        DistanceMultiState(20,20,30);
    }
    else
    {
        VarianceMultiState(1,1,1);
        ExtremumMultiState(1,1,1);
        //IntensityMultiState(1,1,1);
        DistanceMultiState(1,1,1);
    }
    leaveIdentify();
    TotalJudge();
    if(reverse_flag == 1)
    {
        if(EndPointDevice.parking_state == NOCAR)
        {
            EndPointDevice.parking_state = CAR;
        }
    }
    //GetVoltage();
    halLedClear(2);

}

void TotalJudge()
{
    // FunctionName: TotalJudge
    //{
    // Parameter:none
    //
    // Return: identify result 1:car 0:freecar
    //
    // Description:
    //  synthetic judgment£¬ determin final result
    //
    // Created: 2016/3/1
    //
    // Author:xiaoximi
    //}
    uint32 distance = 0;

    if((MagneticUnit.ExtState==CAR)||
       (MagneticUnit.IntState==CAR)||
       (MagneticUnit.VarState==CAR))
    {
        if(CarStableCount<65534)
        {
            CarStableCount++;
        }
        if(Quick_Collect == 1)
        {
            if(CarStableCount>60)
            {
                if(vsEnvironment()==0)
                {
                    EndPointDevice.parking_state = CAR;
                    if(reverse_flag == 1)
                    {
                        for(uint8 i=0;i<MIDDLE_QUENE_LENGTH;i++)
                        {
                            distance += getEuclideanMetric(
                               MagneticUnit.XValue_Stable,
                               MagneticUnit.YValue_Stable,
                               MagneticUnit.ZValue_Stable,
                               x_middle_quene[i],
                               y_middle_quene[i],
                               z_middle_quene[i]
                                   );
                        }
                        if(distance/MIDDLE_QUENE_LENGTH < (MagneticUnit.parked_distance)*2/3)
                        {
                            reverse_flag = 0;
                            parking_time = 0;
                            quickCalibrate();
                        }
                    }
                }
            }
        }
        else
        {
            if(CarStableCount>1)
            {
                if(vsEnvironment()==0)
                {
                    EndPointDevice.parking_state = CAR;
                    
                    
                }
            }
            if(CarStableCount>20)
            {
                CarStableCount = 2;
                vsEnvironment();
            }
        }
    }
    else
    {
        CarStableCount = 0;
    }
    
    if((MagneticUnit.ExtState==NOCAR)&&
       (MagneticUnit.IntState==NOCAR)&&
       (MagneticUnit.VarState!=CAR))
    {
        if(Quick_Collect==1)
        {
            if(NoCarStableCount>30)
            {
                EndPointDevice.parking_state = NOCAR;
            }
        }
        else
        {
            if(NoCarStableCount>1)
            {
                if(reverse_flag == 1)
                {
                    EndPointDevice.parking_state = CAR;
                }
                else
                {
                    EndPointDevice.parking_state = NOCAR;
                }
                
            }
        }
        if(NoCarStableCount<65534)
        {
            NoCarStableCount++;
        }
        
    }
    else
    {
        NoCarStableCount = 0;
    }
    if(Quick_CollectM != Quick_Collect)
    {
        NoCarStableCount = 0;
    }
    Quick_CollectM = Quick_Collect;
    if(Quick_Collect != 1)
    {
        if((NoCarStableCount >10))
        {
            NoCarStableCount = 0;
            NoCarCalibration();
        }
    }
}
 
uint8 vsEnvironment()
{
// FunctionName: vsEnvironment
//{
// Parameter:none
//
// Return:0:different with environment value
//        1£ºclose to the environment value
//        2£ºclose to the first environment value
//
// Description:
// determine the stability of magneitc filed, if stable and eaqual to environment value
// the final result is freecar else is car    
//
// Created: 2016/1/23
//
// Author:xiaoximi
//}
    uint16 ADX[4];
    uint16 ADY[4];
    uint16 ADZ[4];
    uint16 ADvalueX=0;
    uint16 ADvalueY=0;
    uint16 ADvalueZ=0;
    uint8 i=0;
    uint16 xvariance = 0;
    uint16 yvariance = 0;
    uint16 zvariance = 0;
    uint16 xave = 0;
    uint16 yave = 0;
    uint16 zave = 0;
    if(EndPointDevice.parking_state == NOCAR)
    {
        return 0;
    }
    else
    {
        for(i=0;i<4;i++)
        {
            Multi_Read_HMC(&ADvalueX,&ADvalueY,&ADvalueZ);
            ADX[i] = ADvalueX;
            ADY[i] = ADvalueY;
            ADZ[i] = ADvalueZ;
            xave += ADvalueX;
            yave += ADvalueY;
            zave += ADvalueZ;
        }
        xave = xave >> 2;
        yave = yave >> 2;
        zave = zave >> 2;
        xvariance = getVariance(ADX,4);
        yvariance = getVariance(ADY,4);
        zvariance = getVariance(ADZ,4);
        if((xvariance<5)&&(yvariance<5)&&(zvariance<15))
        {
            for(i=0;i<4;i++)
            {
                if((abs(xave - x_middle_quene[i])<30)&&
                   (abs(yave - y_middle_quene[i])<30)&&
                       (abs(zave - z_middle_quene[i])<30))
                {
                    MagneticUnit.XMiddle = xave;
                    MagneticUnit.YMiddle = yave;
                    MagneticUnit.ZMiddle = zave;
                    MagneticUnit.XValue_Stable = MagneticUnit.XMiddle;
                    MagneticUnit.YValue_Stable = MagneticUnit.YMiddle;
                    MagneticUnit.ZValue_Stable = MagneticUnit.ZMiddle;
                    MagneticUnit.Ext_Middle = abs(MagneticUnit.ZMiddle-MagneticUnit.YMiddle);
                    MagneticUnit.Int_Middle = sqrt_16(
                        (((uint32)MagneticUnit.XMiddle*(uint32)MagneticUnit.XMiddle)+
                        ((uint32)MagneticUnit.YMiddle*(uint32)MagneticUnit.YMiddle))+
                        ((uint32)MagneticUnit.ZMiddle*(uint32)MagneticUnit.ZMiddle));
                    reverse_flag = 0;
                    return 1;
                }
            }
        }
        else
        {
            if((abs(xave - MagneticUnit.XMiddleMF)<30)&&(abs(yave - MagneticUnit.YMiddleMF)<30)&&(abs(zave - MagneticUnit.ZMiddleMF)<30))
            {
                return 2;
            }
        }
        return 0;
    }
    
}

void saveMiddle()
{
// FunctionName: saveMiddle
//{
// Parameter:none
//
// Return:none
//
// Description:
// iterates the minddle quene, judge whether need to enquene.
// condition of enquene:EuclideanMetric > 25
//
// Created: 2016/1/23
//
// Author:xiaoximi
//}
    uint8 i=0;
    if(reverse_flag == 1)
    {
        return;
    }
    for(i=0;i<MIDDLE_QUENE_LENGTH;i++)
    {
        if(getEuclideanMetric(MagneticUnit.XMiddle,
                              MagneticUnit.YMiddle,
                              MagneticUnit.ZMiddle,
                              x_middle_quene[i],
                              y_middle_quene[i],
                              z_middle_quene[i]
                                  )<25)
        {
            return;
        }

    }

    {
        x_middle_quene[middle_quene_count] = MagneticUnit.XMiddle;
        y_middle_quene[middle_quene_count] = MagneticUnit.YMiddle;
        z_middle_quene[middle_quene_count] = MagneticUnit.ZMiddle;
        middle_quene_count++;
        if(middle_quene_count==MIDDLE_QUENE_LENGTH)
        {
            middle_quene_count = 1;
        }
    }
}

void NoCarCalibration()
{
// FunctionName: NoCarCalibration
//{
// Parameter:none
//
// Return:none
//
// Description:
//  update middle value when carfree
//
// Created: 2016/3/1
//
// Author:xiaoximi
//}
    if(reverse_flag == 1)
    {
        return;
    }
    
    __disable_interrupt();
    
    quickCalibrate();
    saveMiddle();
    
    __enable_interrupt();
    
}

void CmdCalibration()
{
// FunctionName: CmdCalibration
//{
// Parameter:none
//
// Return:none
//
// Description:
//  call back function of calibration command
//
// Created: 2016/3/1
//
// Author:xiaoximi
//}

    __disable_interrupt();
    quickCalibrate();
    saveMiddle();
    halLedClear(4);
    TA0CCTL0 &= ~CCIFG;
    __enable_interrupt();
    
}

void leaveIdentify()
{
    // FunctionName: leaveIdentify
    //{
    // Parameter:none
    //
    // Return:none
    //
    // Description:
    //  if state of parked car is stable,set reverse_flag to 1, 
    //  and update middle value, so when vehicle is leaving, parking state
    //  change from NOCAR to CAR.
    //
    // Created: 2016/3/2
    //
    // Author:xiaoximi
    //}

    uint16 ADX[4];
    uint16 ADY[4];
    uint16 ADZ[4];
    uint16 ADvalueX=0;
    uint16 ADvalueY=0;
    uint16 ADvalueZ=0;
    uint8 i=0;
    uint16 xvariance = 0;
    uint16 yvariance = 0;
    uint16 zvariance = 0;
    uint16 xave = 0;
    uint16 yave = 0;
    uint16 zave = 0;
    uint32 distance = 0;

    if((EndPointDevice.parking_state == CAR)&&
       (parking_time > 60)&&
       (reverse_flag == 0)&&
       (Quick_Collect == 0))
    {
        //update middle value
        for(i=0;i<4;i++)
        {
            Multi_Read_HMC(&ADvalueX,&ADvalueY,&ADvalueZ);
            ADX[i] = ADvalueX;
            ADY[i] = ADvalueY;
            ADZ[i] = ADvalueZ;
            xave += ADvalueX;
            yave += ADvalueY;
            zave += ADvalueZ;
        }
        xave = xave >> 2;
        yave = yave >> 2;
        zave = zave >> 2;
        xvariance = getVariance(ADX,4);
        yvariance = getVariance(ADY,4);
        zvariance = getVariance(ADZ,4);
        if((xvariance<5)&&(yvariance<5)&&(zvariance<15))
        {
            MagneticUnit.XMiddle = xave;
            MagneticUnit.YMiddle = yave;
            MagneticUnit.ZMiddle = zave;
            MagneticUnit.XValue_Stable = MagneticUnit.XMiddle;
            MagneticUnit.YValue_Stable = MagneticUnit.YMiddle;
            MagneticUnit.ZValue_Stable = MagneticUnit.ZMiddle;
            MagneticUnit.Ext_Middle = abs(MagneticUnit.ZMiddle-MagneticUnit.YMiddle);
            MagneticUnit.Int_Middle = sqrt_16(
                (((uint32)MagneticUnit.XMiddle*(uint32)MagneticUnit.XMiddle)+
                ((uint32)MagneticUnit.YMiddle*(uint32)MagneticUnit.YMiddle))+
                ((uint32)MagneticUnit.ZMiddle*(uint32)MagneticUnit.ZMiddle));
            //reverse flag set
            reverse_flag = 1;
            for(i=0;i<MIDDLE_QUENE_LENGTH;i++)
            {
                distance += getEuclideanMetric(
                                               MagneticUnit.XValue_Stable,
                                               MagneticUnit.YValue_Stable,
                                               MagneticUnit.ZValue_Stable,
                                               x_middle_quene[i],
                                               y_middle_quene[i],
                                               z_middle_quene[i]
                                                   );
            }
            MagneticUnit.parked_distance = distance / MIDDLE_QUENE_LENGTH;
        }
    }
}
 