#include "common.h"
#define STATE1 5                //从无车到有车
#define STATE2 10                //中间态回无车
#define STATE3 15                //有车回无车
uint8 VState1_Count = 0;
uint8 VState2_Count = 0;
uint8 VState3_Count = 0;
uint8 EState1_Count = 0;
uint8 EState2_Count = 0;
uint8 EState3_Count = 0;
uint8 IState1_Count = 0;
uint8 IState2_Count = 0;
uint8 IState3_Count = 0;
uint16 Draw_DataX = 0;
uint16 Draw_DataY = 0;
MagneticStruct MagneticUnit;
uint16 CarStableCount = 0;
uint16 NoCarStableCount = 0;
uint16 ExtChangeCount = 0;
uint16 VarChangeCount = 0;
int Ext_Minus,Var_Minus,Int_Minus;
uint8 CarCaliFlag = 0;



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
            if(EState2_Count>20)
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
                if(EState3_Count>25)
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
        if(MagneticUnit.Int_State!=CAR)
        {
            IState1_Count++;
            if(IState1_Count>8)
            {
                MagneticUnit.Int_State = CAR;
                IState1_Count = 0;
            }
            else
            {
                MagneticUnit.Int_State = NOCAR2CAR;
            }
        }
        
    }
    else
    {
        if(MagneticUnit.Int_State == NOCAR2CAR)
        {
            IState2_Count++;
            if(IState2_Count>STATE2)
            {
                MagneticUnit.Int_State = NOCAR;
                IState2_Count = 0;
                IState1_Count = 0;
            }
        }
        else
        {
            if(MagneticUnit.Int_State != NOCAR)
            {
                IState3_Count++;
                if(IState3_Count>30)
                {
                    MagneticUnit.Int_State = NOCAR;
                    IState3_Count = 0;
                }
            }
        }
    }

}

void IdentifyCar()
{
    SampleChannel(&MagneticUnit.XValue,&MagneticUnit.YValue); 
    GetVariance();
    GetExtremum();
    GetIntensity();
    VarianceMultiState();
    ExtremumMultiState();
    IntensityMultiState();
    TotalJudge();    
    if(EndPointDevice.parking_state_m!=EndPointDevice.parking_state)
    {
        A7139_Deep_Wake();
        EN_INT;
        EN_TIMER1;
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

    if(((MagneticUnit.ExtState==CAR)&&(MagneticUnit.Int_State==CAR))||
       ((MagneticUnit.VarState==CAR)&&(MagneticUnit.Int_State==CAR))||
       ((MagneticUnit.VarState==CAR)&&(MagneticUnit.ExtState==CAR))||
       ((MagneticUnit.VarState==CAR)&&(MagneticUnit.ExtState==CAR)&&(MagneticUnit.Int_State==CAR)))
    {
        
        CarStableCount++;
        if(CarStableCount>60)
        {
            EndPointDevice.parking_state = CAR;
            halLedSet(4);
        }  
    }
    else
    {
        CarStableCount = 0;
    }
    if(CarStableCount == 600)
    {
        CarCalibration();
        CarStableCount = 0;
    }
    if(CarCaliFlag == 1)
    {
        Ext_Minus = MagneticUnit.CarExtremum - MagneticUnit.Extremum;
        if((Ext_Minus)>50)
        {
            ExtChangeCount++;
            if(ExtChangeCount>60)
            {
                MagneticUnit.ExtState=NOCAR;
                ExtChangeCount = 0;
                EState1_Count = 0;
                EState2_Count = 0;
                EState3_Count = 0;
            }
        }
        else
        {
            ExtChangeCount = 0;
        }
        Var_Minus = MagneticUnit.CarVariance - MagneticUnit.Variance;
        if((Var_Minus)>50)
        {
            VarChangeCount++;
            if(VarChangeCount>60)
            {
                MagneticUnit.VarState=NOCAR;
                VarChangeCount = 0;
                VState1_Count = 0;
                VState2_Count = 0;
                VState3_Count = 0;
            }
            
        }
        else
        {
            VarChangeCount = 0;
        }
    }
    
    if(((MagneticUnit.ExtState==NOCAR)&&(MagneticUnit.Int_State==NOCAR))||
       ((MagneticUnit.VarState==NOCAR)&&(MagneticUnit.Int_State==NOCAR))||
       ((MagneticUnit.VarState==NOCAR)&&(MagneticUnit.ExtState==NOCAR))||
       ((MagneticUnit.VarState==NOCAR)&&(MagneticUnit.ExtState==NOCAR)&&(MagneticUnit.Int_State==NOCAR)))
    {
        if(NoCarStableCount>60)
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
    if(NoCarStableCount >240)
    {
        NoCarStableCount = 0;
        PostTask(EVENT_CALIBRATE_SENSOR);
        //NoCarCalibration();
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
        SampleChannel(&ADvalueX,&ADvalueY);
        if((abs(ADvalueX-MagneticUnit.XMiddleM)<80)||(abs(ADvalueY-MagneticUnit.YMiddleM)<80))
        {
            count++;
            ADX += ADvalueX;
            ADY += ADvalueY;
        }

        delay_ms(10);
        
    }
    if(count!=0)
    {
        MagneticUnit.XMiddle = ADX/count;
        MagneticUnit.YMiddle = ADY/count;
        MagneticUnit.Ext_Middle = abs(MagneticUnit.XMiddle-MagneticUnit.YMiddle);
        MagneticUnit.XMiddleM = MagneticUnit.XMiddle;
        MagneticUnit.YMiddleM = MagneticUnit.YMiddle;
    }
    intensity = sqrt_16((((uint32)MagneticUnit.XMiddle*(uint32)MagneticUnit.XMiddle)+((uint32)MagneticUnit.YMiddle*(uint32)MagneticUnit.YMiddle)));
    MagneticUnit.Int_Middle = intensity;
    __enable_interrupt();
    
}


void GetVariance()
{
    uint32 VarianceX,VarianceY;
    VarianceX = (uint32)abs(MagneticUnit.XValue - MagneticUnit.XMiddle)*(uint32)abs(MagneticUnit.XValue - MagneticUnit.XMiddle);
    VarianceY = (uint32)abs(MagneticUnit.YValue - MagneticUnit.YMiddle)*(uint32)abs(MagneticUnit.YValue - MagneticUnit.YMiddle);
//    VarianceAve = (VarianceX + VarianceY)>>1;
    MagneticUnit.Variance = abs(sqrt_16(VarianceY+VarianceX));
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