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
uint8 Sensor_Drift = 0;
uint8 After_Drift_Cal = 0;
FilterStruct FilterData[FILTER_LENGTH];
uint8 Filter_Count = 0;
uint8 XValue_Parking = 0;
uint8 YValue_Parking = 0;
uint16 CarStableCount = 0;
uint16 NoCarStableCount = 0;
uint16 ExtChangeCount = 0;
uint16 VarChangeCount = 0;
uint16 IntChangeCount = 0;
int Ext_Minus,Var_Minus,Int_Minus;
MagneticStruct MagneticUnit;
uint8 Parking_State = 0;
uint8 CarCaliFlag;

//first calculate the slop of the (X-Y)
//if slop less than thredhold, judge (Xavg1 - Xavg0)  and (Yavg1 - Yavg0)
//input data Xvalue and Y value
unsigned int sqrt_16(unsigned long M)
{
    uint16 result;
    uint16 i;
    unsigned long tmp, ttp;   
    if (M == 0)               
        return 0;
    
    result = 0;
    
    tmp = (M >> 30);          
    M <<= 2;
    if (tmp > 1)             
    {
        result ++;                 
        tmp -= result;
    }
    
    for (i=15; i>0; i--)     
    {
        result <<= 1;              
        
        tmp <<= 2;
        tmp += (M >> 30);     
        
        ttp = result;
        ttp = (ttp<<1)+1;
        
        M <<= 2;
        if (tmp >= ttp)       
        {
            tmp -= ttp;
            result ++;
        }
    }
    return result;
}
void CollectData()
{

   
}

void bubbledata(DataStruct *a,uint16 n) 
{ 
    uint16 i,j;
    DataStruct temp;
    for(i=0;i<n-1;i++) 
    {
        for(j=i+1;j<n;j++) 
            if(a[i].value<a[j].value) 
            { 
                temp=a[i];
                a[i]=a[j]; 
                a[j]=temp; 
            }
        
    }
    
}
#define STATE1 5                //从无车到有车
#define STATE2 8                //中间态回无车
#define STATE3 12                //有车回无车

/*方差多状态机识别*/
void VarianceMultiState()
{
    if(MagneticUnit.Variance > VAR_THRESHOLD)
    {
        if(MagneticUnit.VarState!=CAR)
        {
            VState1_Count++;
            if(VState1_Count>STATE1)
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
            if(VState2_Count>STATE2)
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
                if(VState3_Count>STATE3)
                {
                    MagneticUnit.VarState = NOCAR;
                    VState3_Count = 0;
                }
            }
            
        }
        
    }
}


/*极值多状态机识别*/
void ExtremumMultiState()
{
    
    if(MagneticUnit.Extremum > EXT_THRESHOLD)
    {
        if(MagneticUnit.ExtState!=CAR)
        {
            EState1_Count++;
            if(EState1_Count>STATE1)
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
            if(EState2_Count>10)
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
                if(EState3_Count>15)
                {
                    MagneticUnit.ExtState = NOCAR;
                    EState3_Count = 0;
                }
            }
            
        }
        
    }
    //Ext_state = MultiState(ExtremumValue,EXT_THRESHOLD);
}

void IntensityMultiState()
{
    uint16 intensity = 0;
    
    intensity = abs(MagneticUnit.Intensity - MagneticUnit.Int_Middle);
    if(intensity > INT_THRESHOLD)
    {
        if(MagneticUnit.IntState!=CAR)
        {
            IState1_Count++;
            if(IState1_Count>10)
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
            if(IState2_Count>STATE2)
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
                if(IState3_Count>10)
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
    //MagneticUnit.YValue = ybuf/FILTER_LENGTH;
    
}
FilterStruct SlopData[SLOP_LENGTH];
uint8 Slop_Count = 0;
void GetSlop(uint16 xvalue,uint16 yvalue)
{
    uint8 i=0;
    int xslopbuf = 0;
    int yslopbuf = 0;
    SlopData[Slop_Count].xvalue = xvalue;
    SlopData[Slop_Count].yvalue = yvalue;
    Slop_Count++;
    if(Slop_Count==SLOP_LENGTH)
    {
        Slop_Count = 0;
    }
    for(i=1;i<SLOP_LENGTH;i++)
    {
        xslopbuf +=(int)((SlopData[i].xvalue-SlopData[0].xvalue))/i;
        yslopbuf +=(int)((SlopData[i].yvalue-SlopData[0].yvalue))/i;
    }
    MagneticUnit.XAve_Slop = xslopbuf/SLOP_LENGTH;
    MagneticUnit.YAve_Slop = yslopbuf/SLOP_LENGTH;
    
    if((abs(MagneticUnit.XAve_Slop)>20)||(abs(MagneticUnit.YAve_Slop)>20))
    {
       EndPointDevice.parking_state = CAR;
    }
    else
    {
        //EndPointDevice.parking_state = NOCAR;
    }
}
void IdentifyCar()
{
    SampleChannel(&MagneticUnit.XValue,&MagneticUnit.YValue); 
    GetSlop(MagneticUnit.XValue,MagneticUnit.YValue);
    GetVariance();
    GetExtremum();
    GetIntensity();
    VarianceMultiState();
    ExtremumMultiState();
    IntensityMultiState();
    TotalJudge();    
    if(EndPointDevice.parking_state_m!=EndPointDevice.parking_state)
    {
        //A7139_Deep_Wake();
        Parking_State = EndPointDevice.parking_state;
        PostTask(EVENT_CSMA_RESEND);
    }
    else
    {
        
        TestSend();
    }
    EndPointDevice.parking_state_m = EndPointDevice.parking_state;
    /*Start_Collect = 1;
    //非低功耗模式测试先注释掉
    if(Data_Change_Flag == 1)
    {
        Data_Change_Flag = 0;
        A7139_Deep_Wake();
        EN_INT;
        EN_TIMER1;
    }*/
        
}
void TotalJudge()
{

    if(abs(MagneticUnit.XValue - MagneticUnit.XMiddle)>50)
    {
        XValue_Parking = 1;
    }
    else
    {
        XValue_Parking = 0;
    }
    if(abs(MagneticUnit.YValue - MagneticUnit.YMiddle)>50)
    {
        YValue_Parking = 1;
    }
    else
    {
        YValue_Parking = 0;
    }
    
    
    if(((MagneticUnit.ExtState==CAR)&&(MagneticUnit.IntState==CAR))||
       ((MagneticUnit.VarState==CAR)&&(MagneticUnit.IntState==CAR))||
       ((MagneticUnit.VarState==CAR)&&(MagneticUnit.ExtState==CAR))||
       ((MagneticUnit.VarState==CAR)&&(MagneticUnit.ExtState==CAR)&&(MagneticUnit.IntState==CAR))||
        (XValue_Parking==1)||(YValue_Parking==1))
    {
        
        CarStableCount++;
        if(CarStableCount>10)
        {
            //EndPointDevice.parking_state = CAR;
            halLedSet(4);
        }  
    }
    else
    {
        CarStableCount = 0;
    }
//    if(CarStableCount == 600)
//    {
//        CarCalibration();
//        CarStableCount = 0;
//    }
    
    
    if(((MagneticUnit.ExtState==NOCAR)&&(MagneticUnit.IntState==NOCAR))||
       ((MagneticUnit.VarState==NOCAR)&&(MagneticUnit.IntState==NOCAR))||
       ((MagneticUnit.VarState==NOCAR)&&(MagneticUnit.ExtState==NOCAR))||
       ((MagneticUnit.VarState==NOCAR)&&(MagneticUnit.ExtState==NOCAR)&&(MagneticUnit.IntState==NOCAR)))
    {
        if(NoCarStableCount>10)
        {
            EndPointDevice.parking_state = NOCAR;
            halLedClear(4);
        }
        NoCarStableCount++;
    }
    else
    {
        NoCarStableCount = 0;
    }
    if(Sensor_Drift==1)
    {
        After_Drift_Cal++;
    }
    if((NoCarStableCount >1200)||(After_Drift_Cal>60))
    {
        NoCarStableCount = 0;
        After_Drift_Cal = 0;
        NoCarCalibration();
    }
}
  
void CarCalibration()
{
    MagneticUnit.CarIntensity = MagneticUnit.Intensity;
    MagneticUnit.CarExtremum = MagneticUnit.Extremum;
    MagneticUnit.CarVariance = MagneticUnit.Variance;
    CarCaliFlag = 1;
}
void NoCarCalibration()
{
    uint8 count = 0;
    uint16 ADvalueX=0;
    uint16 ADvalueY=0;
    uint16 ADX = 0;
    uint16 ADY = 0;
    uint32 intensity = 0;
    uint8 i=0;

    
    __disable_interrupt();

    halLedToggle(3);
    for(i=0;i<4;i++)
    {
        delay_ms(50);
        SampleChannel(&ADvalueX,&ADvalueY);
        if((abs(ADvalueX-MagneticUnit.XMiddleM)<200)&&(abs(ADvalueY-MagneticUnit.YMiddleM)<200))
        {
            count++;
            ADX += ADvalueX;
            ADY += ADvalueY;
        }

        
        
    }
    if(count!=0)
    {
        ADX = ADX/count;
        ADY = ADY/count;
    }
    if(abs(ADX - MagneticUnit.XMiddleM)<100)
    {
        MagneticUnit.XMiddle = ADX;
    }
    else if(abs(ADX - MagneticUnit.XMiddleM)<200)
    {
        MagneticUnit.XMiddle = (MagneticUnit.XMiddleM + ADX)>>1;
    }
    else if(abs(ADX - MagneticUnit.XMiddleM)<300)
    {
        MagneticUnit.XMiddle = (MagneticUnit.XMiddleM << 1 + ADX)/3;
    }
    if(abs(ADY - MagneticUnit.YMiddleM)<100)
    {
        MagneticUnit.YMiddle = ADY;
    }
    else if(abs(ADY - MagneticUnit.YMiddleM)<150)
    {
        MagneticUnit.YMiddle = (MagneticUnit.YMiddleM + ADY)>>1;
    }
    else if(abs(ADY - MagneticUnit.YMiddleM)<200)
    {
        MagneticUnit.YMiddle = (MagneticUnit.YMiddleM << 1 + ADY)/3;
    }

    MagneticUnit.Ext_Middle = abs(MagneticUnit.XMiddle-MagneticUnit.YMiddle);
    MagneticUnit.XMiddleM = MagneticUnit.XMiddle;
    MagneticUnit.YMiddleM = MagneticUnit.YMiddle;
    
    intensity = sqrt_16((((uint32)MagneticUnit.XMiddle*(uint32)MagneticUnit.XMiddle)+((uint32)MagneticUnit.YMiddle*(uint32)MagneticUnit.YMiddle)));
    MagneticUnit.Int_Middle = intensity;
    Sensor_Drift = 0;
    CarCaliFlag = 0;
    MagneticUnit.CarExtremum = 0;
    MagneticUnit.CarIntensity = 0;
    MagneticUnit.CarVariance = 0;
    delay_ms(50);
    __enable_interrupt();
    
}
  

    

void GetVariance()
{
    uint32 VarianceX,VarianceY;
    VarianceX = (uint32)abs(MagneticUnit.XValue - MagneticUnit.XMiddle)*(uint32)abs(MagneticUnit.XValue - MagneticUnit.XMiddle);
    VarianceY = (uint32)abs(MagneticUnit.YValue - MagneticUnit.YMiddle)*(uint32)abs(MagneticUnit.YValue - MagneticUnit.YMiddle);
//    VarianceAve = (VarianceX + VarianceY)>>1;
    //MagneticUnit.Variance = abs(sqrt_16(VarianceY+VarianceX));
    MagneticUnit.Variance = abs(sqrt_16(VarianceY));

}
void GetExtremum()
{
    uint16 minus = 0;
    minus = abs(MagneticUnit.XValue - MagneticUnit.YValue);
    MagneticUnit.Extremum = abs(minus - MagneticUnit.Ext_Middle);
}
void GetIntensity()
{
    uint32 intensity = 0;
    intensity = ((uint32)(MagneticUnit.XValue)*(uint32)(MagneticUnit.XValue))
                +((uint32)(MagneticUnit.YValue)*(uint32)(MagneticUnit.YValue));
    MagneticUnit.Intensity = sqrt_16(intensity);
} 