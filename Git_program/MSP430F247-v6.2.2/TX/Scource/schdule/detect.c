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
uint16 CarStableCount = 0;         //involve of the car detection
uint16 no_car_stable_count = 0;
uint16 ExtChangeCount = 0;
uint16 VarChangeCount = 0;
uint16 IntChangeCount = 0;
int Ext_Minus,Var_Minus,Int_Minus;
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
uint8 Exit_Sleep = 0;
uint8 On_Test= 0;                       //if be set 1, is sending data to another node

uint32 compatness_memory = 0;
uint16 compatness_stable_count = 0;
uint16 xvalue_memory = 0;
uint16 zvalue_memory = 0;

uint16 x_middle_quene[MIDDLE_QUENE_LENGTH];
uint16 y_middle_quene[MIDDLE_QUENE_LENGTH];
uint16 z_middle_quene[MIDDLE_QUENE_LENGTH];
uint8 middle_quene_count = 1;

uint16 parking_time = 0;

//0: detection results is final result
//1: oppsite of detection results is final result
uint8 parking_stable_flag = 0; 
uint16 update_middle_times = 0;

uint8 leave_recognition_count = 0;

//1:normal state machine toggle
//2:distance < (MagneticUnit.parked_distance)/2
//3:abs(MagneticUnit.ZValue_Stable - z_middle_quene[i])< MagneticUnit.Z_parked_distance/3
//4:vsEnvironment(REVERSE_ENV_THRESHOLD)
uint8 toggle_reason = 0; 
uint16 toggle_distance_test = 0;
uint32 compactness_latest = 0;
uint8 quickcollect_trigger_count = 0;

uint16 xcheck = 0;
uint16 ycheck = 0;
uint16 zcheck = 0;
uint8 check_error = 0;

uint8 environment_over_count = 0;
uint8 force_assign_once = 0;


uint8 dataCheck(uint16 x,uint16 y,uint16 z)
{
// FunctionName: dataCheck
//{
// Parameter:latest three axes value
//
// Return:0: error 1:no error 2:frequently error, need to check sensor
//
// Description:
//  --
//
// Created: 2016/3/12
//
// Author:xiaoximi
//}
    return 1;
    if((abs(x-xcheck)>5000)||(abs(y-ycheck)>5000)||(abs(z-zcheck)>5000))
    {
        check_error++;
        if(check_error>10)
        {
            return 2;
        }
        return 0;
    }
    else
    {
        check_error = 0;
        xcheck = x;
        ycheck = y;
        zcheck = z;
        return 1;
    }
}

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

uint8 quickCalibrate(uint8 force)
{
// FunctionName: quickCalibrate
//{
// Parameter:force 1:force to calibrate don't cate variance
//                 0:perform calibrate when variance is stable
//
// Return: 1:environment is stable, performs a calibrate.
//         0:environment is not stable, do nothing
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
    uint32 xave = 0;
    uint32 yave = 0;
    uint32 zave = 0;
    for(i=0;i<4;i++)
    {
        PNI_read_data(&ADvalueX,&ADvalueY,&ADvalueZ);
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
    if((xvariance<25)&&(yvariance<25)&&(zvariance<35)|| force)
    {
        MagneticUnit.XMiddle = xave;
        MagneticUnit.YMiddle = yave;
        MagneticUnit.ZMiddle = zave;
        MagneticUnit.XValue_Stable = MagneticUnit.XMiddle;
        MagneticUnit.YValue_Stable = MagneticUnit.YMiddle;
        MagneticUnit.ZValue_Stable = MagneticUnit.ZMiddle;
        MagneticUnit.Ext_Middle = abs(MagneticUnit.ZMiddle-MagneticUnit.XMiddle);
        MagneticUnit.Int_Middle = sqrt_16(
            (((uint32)MagneticUnit.XMiddle*(uint32)MagneticUnit.XMiddle)+
             ((uint32)MagneticUnit.YMiddle*(uint32)MagneticUnit.YMiddle))+
             ((uint32)MagneticUnit.ZMiddle*(uint32)MagneticUnit.ZMiddle));
        MagneticUnit.CarExtremum = 0;
        MagneticUnit.CarIntensity = 0;
        MagneticUnit.CarVariance = 0;
        return 1;
    }
    return 0;
}

void calibrateOnce(uint16 ADvalueX,uint16 ADvalueY,uint16 ADvalueZ)
{
// FunctionName: calibrateOnce
//{
// Parameter:x,y,z value
//
// Return:none 
//
// Description:
//  update middle value
//
// Created: 2016/3/19
//
// Author:xiaoximi
//}
    MagneticUnit.XMiddle = ADvalueX;
    MagneticUnit.YMiddle = ADvalueY;
    MagneticUnit.ZMiddle = ADvalueZ;
    MagneticUnit.XValue_Stable = MagneticUnit.XMiddle;
    MagneticUnit.YValue_Stable = MagneticUnit.YMiddle;
    MagneticUnit.ZValue_Stable = MagneticUnit.ZMiddle;
    MagneticUnit.Ext_Middle = abs(MagneticUnit.ZMiddle-MagneticUnit.XMiddle);
    MagneticUnit.Int_Middle = sqrt_16(
                                      (((uint32)MagneticUnit.XMiddle*(uint32)MagneticUnit.XMiddle)+
                                       ((uint32)MagneticUnit.YMiddle*(uint32)MagneticUnit.YMiddle))+
                                          ((uint32)MagneticUnit.ZMiddle*(uint32)MagneticUnit.ZMiddle));
    parking_stable_flag = 0;
    parking_time = 0;
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
        ReCal_Count++;
        return 0;
        XReCal+=MagneticUnit.XValue;
        YReCal+=MagneticUnit.YValue;
        ZReCal+=MagneticUnit.ZValue;
    }
    else if(ReCal_Count==3)
    {
        ReCal_Count++;
        return 1;
        MagneticUnit.XMiddle = XReCal/3;
        MagneticUnit.YMiddle = YReCal/3;
        MagneticUnit.ZMiddle = ZReCal/3;
        MagneticUnit.XValue_Stable = MagneticUnit.XMiddle;
        MagneticUnit.YValue_Stable = MagneticUnit.YMiddle;
        MagneticUnit.ZValue_Stable = MagneticUnit.ZMiddle;
        
        MagneticUnit.Int_Middle = MagneticUnit.Intensity;
        MagneticUnit.Ext_Middle = abs(MagneticUnit.ZMiddle-MagneticUnit.XMiddle);
        ReCal_Count = 20;
        
        MagneticUnit.ExtState = 0;
        MagneticUnit.IntState = 0;
        MagneticUnit.VarState = 0;
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

//    SampleVoltage(&EndPointDevice.vlotage,&EndPointDevice.temperature);
//    if((EndPointDevice.vlotage<LOWPOWER_THRESHOLD_HIGH)&&((EndPointDevice.vlotage>LOWPOWER_THRESHOLD_LOW)))
//    {
//        EndPointDevice.parking_state = LOWPOWER;
//    }
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
        if(compatness_stable_count>60000)
        {
            compatness_stable_count = 0;
        }
    }
    else
    {
        compatness_stable_count = 0;
    }
    //could adjust compactness last time
    if(compatness_stable_count > COM_STABLE_COUNT)
    {
        compatness_stable_count = 0;
        return 1;
    }
    compatness_memory = MagneticUnit.compatness;
    if(MagneticUnit.compatness>0)
    {
        compactness_latest = MagneticUnit.compatness;
    }
    return 0;
}

uint8 leaveRecognition()
{
    // FunctionName: leaveRecognition
    //{
    // Parameter:none
    //
    // Return:bool 1:car is leaving 0:nothing leaving
    //
    // Description:
    //  --
    //
    // Created: 2016/3/7
    //
    // Author:xiaoximi
    //}

    uint32 distance = 0;
    uint16 minus = 0;
    if(MagneticUnit.compatness < 1200)
    {
        return 0;
    }
    
    for(uint8 i=0;i<MIDDLE_QUENE_LENGTH;i++)
    {
        distance = getEuclideanMetric(
           MagneticUnit.XValue_Stable,
           MagneticUnit.YValue_Stable,
           MagneticUnit.ZValue_Stable,
           x_middle_quene[i],
           y_middle_quene[i],
           z_middle_quene[i]
               );
        //compare the distance with parked distance
        if(distance < (MagneticUnit.parked_distance)/2)
        {
            toggle_reason = 2;
            toggle_distance_test = distance;
            return 1;
        }
        //compare the distance with the distance at the time just parked
        minus = abs(MagneticUnit.ZValue_Stable - z_middle_quene[i]);
        if(minus < MagneticUnit.Z_parked_distance/3)
        {
            toggle_reason = 3;
            toggle_distance_test = minus;
            return 1;
        }
        if(minus<50)
        {
            toggle_reason = 4;
            toggle_distance_test = minus;
            return 1;
        }
    }
    //compare with environment value and raise threshold
    if(vsEnvironment(REVERSE_ENV_THRESHOLD))
    {
        toggle_reason = 4;
        return 1;
    }
    return 0;
}

void parkingCalibrate()
{
// FunctionName: parkingCalibrate
//{
// Parameter:none
//
// Return:none
//
// Description:
//  calibrate magnetic value when parked
//
// Created: 2016/3/7
//
// Author:xiaoximi
//}
    
    if(parking_stable_flag)
    {
        MagneticUnit.XValue_parked_stable = MagneticUnit.XValue;
        MagneticUnit.YValue_parked_stable = MagneticUnit.YValue;
        MagneticUnit.ZValue_parked_stable = MagneticUnit.ZValue;
    }
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
    uint16 xvalue = 0;
    uint16 yvalue = 0;
    uint16 zvalue = 0;
    uint8  data_check = 0;
    halLedSet(2);

    xvalue_memory = MagneticUnit.XValue;
    zvalue_memory = MagneticUnit.ZValue;
    
    PNI_read_data(&xvalue,&yvalue,&zvalue);
    data_check = dataCheck(xvalue,yvalue,zvalue);
    if(data_check)
    {
        MagneticUnit.XValue = xvalue;
        MagneticUnit.YValue = yvalue;
        MagneticUnit.ZValue = zvalue;
    }
    else if(data_check == 2)
    {
        //don't run detection part, send sensor error;
    }
    
    //MagneticUnit.infrared = getInfrared();

    
    if(ReCal())
    {
        MagneticUnit.compatness = getCompatness(
                                                MagneticUnit.XValue,
                                                MagneticUnit.ZValue,
                                                xvalue_memory ,
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
    
    MagneticUnit.Variance = getAbsoluteValue();
    MagneticUnit.Extremum = getExtremum();   
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
        if(((abs(MagneticUnit.ZValue - z_middle_quene[0])>700)
            ||abs(MagneticUnit.XValue - x_middle_quene[0])>700)
            &&(force_assign_once==0)
                )
        {
            force_assign_once = 1;
            MagneticUnit.compatness = COM_THRESHOLD+1;
            diameterbuf = 1001;
        }
        if(((MagneticUnit.compatness > COM_THRESHOLD)&&(diameterbuf>1000))
           //may be there is a vehicle above the sensor, but it doesn't meet the last condition
           //||(abs(MagneticUnit.ZValue-MagneticUnit.ZMiddle)>300)
           )
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

    
    VarianceMultiState(10,20,25);
    ExtremumMultiState(10,20,30);
    DistanceMultiState(20,20,30);
    parkingStableSet();
    TotalJudge();
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
    

    if((MagneticUnit.ExtState==CAR)||
       (MagneticUnit.IntState==CAR)||
       (MagneticUnit.VarState==CAR))
    {
        if((CarStableCount<65534)&&(vsEnvironment(NORMAL_ENV_THRESHOLD)==0))
        {
            CarStableCount++;
        }
        else
        {
            CarStableCount = 0;
        }
        if(CarStableCount>60)
        {
            EndPointDevice.parking_state = CAR;
        }
        if(vsEnvironment(NORMAL_ENV_THRESHOLD))
        {
            environment_over_count++;
            if(environment_over_count>100)  //5 s
            {
                calibrateOnce(MagneticUnit.XValue,MagneticUnit.YValue,MagneticUnit.ZValue);
                environment_over_count = 0;
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
        if(no_car_stable_count>30)
        {
            EndPointDevice.parking_state = NOCAR;
            parking_stable_flag = 0;
            force_assign_once = 0;
            toggle_reason = 1;
        }
        if(no_car_stable_count<65534)
        {
            no_car_stable_count++;
        }
    }
    else
    {
        no_car_stable_count = 0;
    }

    
    if((no_car_stable_count > 2400)&&(On_Test == 0)
       &&(abs(MagneticUnit.XValue-x_middle_quene[0])<300)
       &&(abs(MagneticUnit.ZValue-z_middle_quene[0])<600))
    {
        no_car_stable_count = 0;
        NoCarCalibration();
    }
    if((parking_time > 2400)&&(On_Test == 0))
    {
        //parkingCalibrate();
    }
    //judge whether car is leaving but faile to recognition.
    if(parking_stable_flag == 1)
    {
        if(leaveRecognition())
        {
            leave_recognition_count++;
            if(leave_recognition_count>50)
            {
                quickCalibrate(1);
                parking_time = 0;
                parking_stable_flag = 0;
                leave_recognition_count = 0;
            }
        }
        else
        {
            leave_recognition_count = 0;
        }
    }
}
  
uint8 vsEnvironment(uint16 threshold)
{
// FunctionName: vsEnvironment
//{
// Parameter:none
//
// Return:0:different with environment value
//        1£ºclose to the environment value
//
// Description:
// determine the stability of magneitc filed, if stable and eaqual to environment value
// the final result is freecar else is car    
//
// Created: 2016/1/23
//
// Author:xiaoximi
//}
    uint16 ADvalueX = MagneticUnit.XValue;
    uint16 ADvalueY = MagneticUnit.YValue;
    uint16 ADvalueZ = MagneticUnit.ZValue;
    uint8 i=0;
    if(EndPointDevice.parking_state == NOCAR)
    {
        return 0;
    }
    else
    {
            //PNI_read_data(&ADvalueX,&ADvalueY,&ADvalueZ);
            for(i=0;i<MIDDLE_QUENE_LENGTH;i++)
            {
                if((abs(ADvalueX - x_middle_quene[i])<threshold)&&
                   (abs(ADvalueY - y_middle_quene[i])<threshold)&&
                   (abs(ADvalueZ - z_middle_quene[i])<threshold))
                {
                    return 1;
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
    if(parking_stable_flag == 1)
    {
        return;
    }
    for(i=0;i<MIDDLE_QUENE_LENGTH;i++)
    {
        if((getEuclideanMetric(MagneticUnit.XMiddle,
                              MagneticUnit.YMiddle,
                              MagneticUnit.ZMiddle,
                              x_middle_quene[i],
                              y_middle_quene[i],
                              z_middle_quene[i]
                                  )<200) ||
           (abs(MagneticUnit.ZMiddle-z_middle_quene[i])>1000)
           )
        {
            //if to close or z axis changed dramatically don't save middle value
            return;
        }

    }
    
    
    x_middle_quene[middle_quene_count] = MagneticUnit.XMiddle;
    y_middle_quene[middle_quene_count] = MagneticUnit.YMiddle;
    z_middle_quene[middle_quene_count] = MagneticUnit.ZMiddle;
    middle_quene_count++;
    if(middle_quene_count==MIDDLE_QUENE_LENGTH)
    {
        middle_quene_count = 1;
    }
    if(update_middle_times > 65530)
    {
        update_middle_times = 0;
    }
    update_middle_times++;
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
    if((parking_stable_flag==1))
    {
        return;
    }
    
    __disable_interrupt();
    
    if(quickCalibrate(0))
    {
        saveMiddle();
    }
    else
    {
        no_car_stable_count = 1000;
    }
    __enable_interrupt();
    
}

void parkingStableSet()
{
    // FunctionName: parkingStableSet
    //{
    // Parameter:none
    //
    // Return:none
    //
    // Description:
    //  if state of parked car is stable,set parking_stable_flag to 1, 
    //  and write down the distance between parked and vacant
    //
    // Created: 2016/3/2
    //
    // Author:xiaoximi
    //}
    uint8 i=0;
    uint32 distance = 0;

    if((EndPointDevice.parking_state == CAR)&&
       (parking_time > 1200)&&  //1 min
       (parking_stable_flag == 0))
    {
        //parking_stable_flag  set
        parking_stable_flag = 1;
        MagneticUnit.XValue_parked_stable = MagneticUnit.XValue;
        MagneticUnit.YValue_parked_stable = MagneticUnit.YValue;
        MagneticUnit.ZValue_parked_stable = MagneticUnit.ZValue;
        MagneticUnit.Z_parked_distance = 
            abs(MagneticUnit.ZMiddle - MagneticUnit.ZValue_parked_stable);
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

