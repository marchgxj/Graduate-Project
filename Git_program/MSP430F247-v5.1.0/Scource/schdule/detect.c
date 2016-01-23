#include "common.h"
#define STATE1 10                //从无车到有车
#define STATE2 15                //中间态回无车
#define STATE3 20                //有车回无车
//uint8  State1_Count = 0;        
//uint8  State2_Count = 0;
//uint8  State3_Count = 0;
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
uint8 CarCaliFlag = 0;
uint16 Collect_Period = 0;
uint8 Quick_Collect = 0;
uint8 Quick_CollectM = 0;
FilterStruct SlopData[SLOP_LENGTH];
uint8 Slop_Count = 0;
uint8 ReCal_Count = 0;
uint16 XReCal = 0;
uint16 YReCal = 0;
uint16 ZReCal = 0;
uint16 GMI_XReCal = 0;
uint16 GMI_YReCal = 0;
uint8 HMC_Identify_Fail = 0;
uint16 OpenGMI_Count = 0;
uint8 HMC_Changed = 0;
uint16 Ext_Threshold = 0;
uint16 Int_Threshold = 0;
uint16 Var_Threshold = 0;
uint8 Exit_Sleep = 0;
uint8 On_Test= 0;                       //发送测试数据
uint8 Cmd_CarCal_Flag = 0;
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

uint16 getAverage(uint16 *data,uint8 length)
{
    uint8 i = 0;
    uint32 count = 0;
    for(i=0;i<length;i++)
    {
        count += *data++;
    }
    return (uint16)(count / length);
}

uint16 getVariance(uint16 *data,uint8 length)
{
    uint16 average = 0;
    uint32 sum = 0;
    uint8 i = 0;
    average = getAverage(data,length);
    for(i=0;i<length;i++)
    {
        sum += (*data-average)*(*data-average);
        data++;
    }
    return (uint16)(sum / length);
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
void VarianceMultiState(uint16 state1,uint16 state2,uint16 state3)
{
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
/*极值多状态机识别*/
void ExtremumMultiState(uint16 state1,uint16 state2,uint16 state3)
{
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
void GetSlop(uint16 xvalue,uint16 yvalue,uint16 zvalue)
{
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
    
    if((abs(MagneticUnit.XAve_Slop)>5)||(abs(MagneticUnit.YAve_Slop)>5)||(abs(MagneticUnit.ZAve_Slop)>5))
    {
        Quick_CollectM = Quick_Collect;
        Quick_Collect = 1;
        Collect_Period = 0;
#if (GMI_EN == 1)
        HMC_Changed = 1;
#endif
    }
}
void ReCal()
{
    //刚开始上电校准可能和此处不一样，重新校准
    
    if(ReCal_Count<3)
    {
        SampleChannel(&MagneticUnit.GMI_XValue,&MagneticUnit.GMI_YValue);
        ReCal_Count++;
        XReCal+=MagneticUnit.XValue;
        YReCal+=MagneticUnit.YValue;
        ZReCal+=MagneticUnit.ZValue;
        GMI_XReCal+=MagneticUnit.GMI_XValue;
        GMI_YReCal+=MagneticUnit.GMI_YValue;
    }
    else if(ReCal_Count==3)
    {
        MagneticUnit.XMiddle = XReCal/3;
        MagneticUnit.YMiddle = YReCal/3;
        MagneticUnit.ZMiddle = ZReCal/3;
        MagneticUnit.GMI_XMiddle = GMI_XReCal/3;
        MagneticUnit.GMI_YMiddle = GMI_YReCal/3;
        MagneticUnit.XMiddleM = MagneticUnit.XMiddle;
        MagneticUnit.YMiddleM = MagneticUnit.YMiddle;
        MagneticUnit.ZMiddleM = MagneticUnit.ZMiddle;
        MagneticUnit.XMiddleMF = MagneticUnit.XMiddle;
        MagneticUnit.YMiddleMF = MagneticUnit.YMiddle;
        MagneticUnit.ZMiddleMF = MagneticUnit.ZMiddle;
        MagneticUnit.GMI_XMiddleM = MagneticUnit.GMI_XMiddle;
        MagneticUnit.GMI_YMiddleM = MagneticUnit.GMI_YMiddle;
        MagneticUnit.Int_Middle = MagneticUnit.Intensity;
        MagneticUnit.Ext_Middle = abs(MagneticUnit.XMiddle-MagneticUnit.YMiddle);
        ReCal_Count = 20;
        MagneticUnit.GMI_XValue = 0;
        MagneticUnit.GMI_YValue = 0;
        HMC_Changed = 0;
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
}
void GMI_Identify()
{
    SampleChannel(&MagneticUnit.GMI_XValue,&MagneticUnit.GMI_YValue);
    if(abs(MagneticUnit.GMI_XValue - MagneticUnit.GMI_XMiddle)>100)
    {
        XValue_Parking = 1;
    }
    else if(abs(MagneticUnit.GMI_XValue - MagneticUnit.GMI_XMiddle)<130)
    {
        XValue_Parking = 2;
        if(OpenGMI_Count >= CLOSE_GMI_COUNT)
        {
            HMC_Identify_Fail = 0;
            HMC_Changed = 0;
            OpenGMI_Count = 0;
            MagneticUnit.GMI_XValue = 0;
            MagneticUnit.GMI_YValue = 0;
        }
    }
    else
    {
        XValue_Parking = 0;
        if(OpenGMI_Count >= CLOSE_GMI_COUNT)
        {
            HMC_Identify_Fail = 0;
            HMC_Changed = 0;
            OpenGMI_Count = 0;
            MagneticUnit.GMI_XValue = 0;
            MagneticUnit.GMI_YValue = 0;
        }
    }
    if(abs(MagneticUnit.GMI_YValue - MagneticUnit.GMI_YMiddle)>80)
    {
        YValue_Parking = 1;
    }
    else if(abs(MagneticUnit.GMI_YValue - MagneticUnit.GMI_YMiddle)<130)
    {
        YValue_Parking = 2;
        if(OpenGMI_Count >= CLOSE_GMI_COUNT)
        {
            HMC_Identify_Fail = 0;
            HMC_Changed = 0;
            OpenGMI_Count = 0;
            MagneticUnit.GMI_XValue = 0;
            MagneticUnit.GMI_YValue = 0;
        }
    }
    else
    {
        YValue_Parking = 0;
        if(OpenGMI_Count >= CLOSE_GMI_COUNT)
        {
            HMC_Identify_Fail = 0;
            HMC_Changed = 0;
            OpenGMI_Count = 0;
            MagneticUnit.GMI_XValue = 0;
            MagneticUnit.GMI_YValue = 0;
        }
    }
}

uint8 isEnviroment()
{
    if((abs(MagneticUnit.XValue - MagneticUnit.XMiddleMF)<15)&&(abs(MagneticUnit.YValue - MagneticUnit.YMiddleMF)<15)&&(abs(MagneticUnit.ZValue - MagneticUnit.ZMiddleMF)<15))
    {
        MagneticUnit.XMiddle = MagneticUnit.XValue;
        MagneticUnit.YMiddle = MagneticUnit.YValue;
        MagneticUnit.ZMiddle = MagneticUnit.ZValue;
        MagneticUnit.Ext_Middle = abs(MagneticUnit.XMiddle-MagneticUnit.YMiddle);
        MagneticUnit.Int_Middle = sqrt_16((((uint32)MagneticUnit.XMiddle*(uint32)MagneticUnit.XMiddle)+((uint32)MagneticUnit.YMiddle*(uint32)MagneticUnit.YMiddle))+((uint32)MagneticUnit.ZMiddle*(uint32)MagneticUnit.ZMiddle));
        return 1;
    }
    else
    {
        return 0;
    }
} 

void GetVoltage()
{
    SampleVoltage(&EndPointDevice.vlotage,&EndPointDevice.temperature);
    if(EndPointDevice.vlotage<LOWPOWER_THRESHOLD)
    {
        EndPointDevice.parking_state = LOWPOWER;
    }
    if((EndPointDevice.temperature>TEMP_THRESHOLD_HIGH)&&((EndPointDevice.vlotage<TEMP_THRESHOLD_LOW)))
    {
        EndPointDevice.parking_state = TEMPOVER;
    }
}
void IdentifyCar()
{
    halLedSet(2);
    if(OpenGMI_Count >= OPEN_GMI_COUNT)
    {
        if(EndPointDevice.parking_state == NOCAR)
        {
            HMC_Identify_Fail = 1;
            //启动GMI
        }
        
    }
    //SampleChannel(&MagneticUnit.GMI_XValue,&MagneticUnit.GMI_YValue);
    Multi_Read_HMC(&MagneticUnit.XValue,&MagneticUnit.YValue,&MagneticUnit.ZValue);
    ReCal();
    GetSlop(MagneticUnit.XValue,MagneticUnit.YValue,MagneticUnit.ZValue);
    //Filter(MagneticUnit.XValue,MagneticUnit.YValue);
    GetVariance();
    GetExtremum();
    GetIntensity();
    
    
    if(Quick_Collect == 1)
    {
        VarianceMultiState(10,20,25);
        ExtremumMultiState(10,20,30);
        IntensityMultiState(20,20,30);
    }
    else
    {
        VarianceMultiState(1,1,1);
        ExtremumMultiState(1,1,1);
        IntensityMultiState(1,1,1);
    }
    TotalJudge();
    GetVoltage();
    halLedClear(2);
    if(On_Test == 0)
    {
        if(EndPointDevice.parking_state_m!=EndPointDevice.parking_state)
        {
            Exit_Sleep  = 1;
            Data_Send_Waiting_Flag = 1;
            A7139_Deep_Wake();
            halLedSet(3);
            Int_Enable_Flag = 1;
            if((EndPointDevice.parking_state_m==CAR)&&(EndPointDevice.parking_state==NOCAR)&&(Cmd_CarCal_Flag==1))
            {
                Cmd_CarCal_Flag = 0;
                REBOOT;
            }
            EN_INT;
            //EN_TIMER1;
        }
        EndPointDevice.parking_state_m = EndPointDevice.parking_state;
    }
    

        
}


void TotalJudge()
{
    if((MagneticUnit.ExtState==CAR)||(MagneticUnit.IntState==CAR)||(MagneticUnit.VarState==CAR))
    {
        if(CarStableCount<65534)
        {
            CarStableCount++;
        }
        if(Quick_Collect == 1)
        {
            if(CarStableCount>60)
            {
                //halLedSet(4);
                
                if(Cmd_CarCal_Flag == 0)
                {
                    if(isEnviroment()==0)
                    {
                        EndPointDevice.parking_state = CAR;
                    }
                    
                }
                else
                {
                    EndPointDevice.parking_state = NOCAR;
                }
                if(HMC_Identify_Fail == 1)
                {
                    HMC_Changed = 0;
                    HMC_Identify_Fail = 0;
                    MagneticUnit.GMI_XValue = 0;
                    MagneticUnit.GMI_YValue = 0;
                }
                
            }  
        }
        else
        {
            if(CarStableCount>1)
            {
                //halLedSet(4);
                if(Cmd_CarCal_Flag == 0)
                {
                    if(isEnviroment()==0)
                    {
                        EndPointDevice.parking_state = CAR;
                    }
                    
                }
                else
                {
                    EndPointDevice.parking_state = NOCAR;
                }
                if(HMC_Identify_Fail == 1)
                {
                    HMC_Changed = 0;
                    HMC_Identify_Fail = 0;
                    MagneticUnit.GMI_XValue = 0;
                    MagneticUnit.GMI_YValue = 0;
                }
            }  
        }
		
        
    }
    else
    {
        CarStableCount = 0;
    }
    if((MagneticUnit.ExtState==NOCAR)&&(MagneticUnit.IntState==NOCAR)&&(MagneticUnit.VarState==NOCAR))
    {
        if(Quick_Collect==1)
        {
            if(NoCarStableCount>30)
            {
                //halLedClear(4);
                if(Cmd_CarCal_Flag == 0)
                {
                    EndPointDevice.parking_state = NOCAR;
                }
                else
                {
                    EndPointDevice.parking_state = CAR;
                }
                
            }
        }
        else
        {
            if(NoCarStableCount>1)
            {
                if(Cmd_CarCal_Flag == 0)
                {
                    EndPointDevice.parking_state = NOCAR;
                }
                else
                {
                    EndPointDevice.parking_state = CAR;
                }
                //halLedClear(4);
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
	if(Quick_CollectM != Quick_CollectM)
    {
        NoCarStableCount = 0;
    }
    
    if(On_Test == 0)
    {
        if(Quick_Collect == 1)
        {
//           if((NoCarStableCount >400))//||(After_Drift_Cal>60))
//            {
//               NoCarStableCount = 0;
//                //After_Drift_Cal = 0;
//                NoCarCalibration();
//            }
        }
        else
        {
            if((NoCarStableCount >60))//||(After_Drift_Cal>60))
            {
                NoCarStableCount = 0;
                //After_Drift_Cal = 0;
                NoCarCalibration();
            }
        }
    }
    
}

uint8 CarCalibration()
{
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
    if((xvariance<5)&&(yvariance<5)&&(zvariance<5))
    {
        if((abs(xave - MagneticUnit.XMiddleMF)<15)&&(abs(yave - MagneticUnit.YMiddleMF)<15)&&(abs(zave - MagneticUnit.ZMiddleMF)<15))
        {
            MagneticUnit.XMiddle = xave;
            MagneticUnit.YMiddle = yave;
            MagneticUnit.ZMiddle = zave;
            MagneticUnit.Ext_Middle = abs(MagneticUnit.XMiddle-MagneticUnit.YMiddle);
            MagneticUnit.Int_Middle = sqrt_16((((uint32)MagneticUnit.XMiddle*(uint32)MagneticUnit.XMiddle)+((uint32)MagneticUnit.YMiddle*(uint32)MagneticUnit.YMiddle))+((uint32)MagneticUnit.ZMiddle*(uint32)MagneticUnit.ZMiddle));
            return 1;
        }
    }
    else
    {
        if((abs(xave - MagneticUnit.XMiddleMF)<15)&&(abs(yave - MagneticUnit.YMiddleMF)<15)&&(abs(zave - MagneticUnit.ZMiddleMF)<15))
        {
            return 2;
        }
    }
    return 0;
}

void NoCarCalibration()
{
    uint8  count = 0;
    uint8  count1 = 0;
    uint16 ADvalueX=0;
    uint16 ADvalueY=0;
    uint16 ADvalueZ=0;
    uint16 GMI_ADvalueX=0;
    uint16 GMI_ADvalueY=0;
    uint16 ADX[4];
    uint16 ADY[4];
    uint16 ADZ[4];
    uint16 xsum = 0;
    uint16 ysum = 0;
    uint16 zsum = 0;
    uint16 xmiddle = 0;
    uint16 ymiddle = 0;
    uint16 zmiddle = 0;
    uint16 xvariance = 0;
    uint16 yvariance = 0;
    uint16 zvariance = 0;
    uint16 GMI_ADX = 0;
    uint16 GMI_ADY = 0;
    uint32 intensity = 0;
    uint8  i=0;
    int xdrift = 0;
    int ydrift = 0;
    int zdrift = 0;
    
    halLedSet(4);
    __disable_interrupt();


    for(i=0;i<4;i++)
    {
        delay_ms(50);
        SampleChannel(&GMI_ADvalueX,&GMI_ADvalueY);
        
        if((abs(GMI_ADvalueX-MagneticUnit.GMI_XMiddleM)<200)&&(abs(GMI_ADvalueY-MagneticUnit.GMI_YMiddleM)<200))
        {
            count1++;
            GMI_ADX += GMI_ADvalueX;
            GMI_ADY += GMI_ADvalueY;
        }
        Multi_Read_HMC(&ADvalueX,&ADvalueY,&ADvalueZ);
        ADX[i] = ADvalueX;
        ADY[i] = ADvalueY;
        ADZ[i] = ADvalueZ;
        xsum += ADvalueX;
        ysum += ADvalueY;
        zsum += ADvalueZ;
        
    }
    xvariance = getVariance(ADX,4);
    yvariance = getVariance(ADY,4);
    zvariance = getVariance(ADZ,4);
    if((xvariance<5)&&(yvariance<5)&&(zvariance<5))
    {
        xmiddle = xsum >> 2;
        ymiddle = ysum >> 2;
        zmiddle = zsum >> 2;
    }
    else
    {
        return ;
    }
    
    
    if(count1!=0)
    {
        GMI_ADX = GMI_ADX/count1;
        GMI_ADY = GMI_ADY/count1;
    }
    
    xdrift = (int)(xmiddle - MagneticUnit.XMiddleMF);
    ydrift = (int)(ymiddle - MagneticUnit.YMiddleMF);
    zdrift = (int)(zmiddle - MagneticUnit.ZMiddleMF);

    if(abs(xdrift) < 5)
    {
    }
    else if(abs(xdrift) < 10)
    {
        xdrift = (int)(xdrift * 0.8);
    }
    else if(abs(xdrift) < 25)
    {
        xdrift = (int)(xdrift * 0.6);
        Ext_Threshold = EXT_THRESHOLD + 25;
    }
    else if(abs(xdrift) < 50)
    {
        xdrift = (int)(xdrift * 0.3);
        Ext_Threshold = EXT_THRESHOLD + 30;
    }
    if(abs(ydrift) < 5)
    {
    }
    else if(abs(ydrift) < 20)
    {
        ydrift = (int)(ydrift * 0.8);
    }
    else if(abs(ydrift) < 35)
    {
        ydrift = (int)(ydrift * 0.6);
        Var_Threshold = VAR_THRESHOLD + 8;
    }
    else if(abs(ydrift) < 60)
    {
        ydrift = (int)(ydrift * 0.3);
        Var_Threshold = VAR_THRESHOLD + 13;
    }
    if(zdrift<5)
    {
    
    }
    else if(abs(zdrift) < 10)
    {
        zdrift = (int)(zdrift * 0.8);
    }
    else if(abs(zdrift) < 25)
    {
        zdrift = (int)(zdrift * 0.6);
        Int_Threshold = INT_THRESHOLD + 7;
    }
    else if(abs(zdrift) < 50)
    {
        zdrift = (int)(zdrift * 0.3);
        Int_Threshold = INT_THRESHOLD + 5;
    }
    
    MagneticUnit.XMiddle = (uint16)(MagneticUnit.XMiddle + xdrift);
    MagneticUnit.YMiddle = (uint16)(MagneticUnit.YMiddle + ydrift);
    MagneticUnit.ZMiddle = (uint16)(MagneticUnit.ZMiddle + zdrift);
    
    if(abs(GMI_ADX - MagneticUnit.GMI_XMiddleM)<100)
    {
        MagneticUnit.GMI_XMiddle = GMI_ADX;
    }
    else if(abs(GMI_ADX - MagneticUnit.GMI_XMiddleM)<200)
    {
        MagneticUnit.GMI_XMiddle = (MagneticUnit.GMI_XMiddleM + GMI_ADX)>>1;
    }
    else if(abs(GMI_ADX - MagneticUnit.GMI_XMiddleM)<300)
    {
        MagneticUnit.GMI_XMiddle = (MagneticUnit.GMI_XMiddleM << 1 + GMI_ADX)/3;
    }
    if(abs(GMI_ADY - MagneticUnit.GMI_YMiddleM)<100)
    {
        MagneticUnit.GMI_YMiddle = GMI_ADY;
    }
    else if(abs(GMI_ADY - MagneticUnit.GMI_YMiddleM)<150)
    {
        MagneticUnit.GMI_YMiddle = (MagneticUnit.GMI_YMiddleM + GMI_ADY)>>1;
    }
    else if(abs(GMI_ADY - MagneticUnit.GMI_YMiddleM)<200)
    {
        MagneticUnit.GMI_YMiddle = (MagneticUnit.GMI_YMiddleM << 1 + GMI_ADY)/3;
    }

    MagneticUnit.Ext_Middle = abs(MagneticUnit.XMiddle-MagneticUnit.YMiddle);
    MagneticUnit.XMiddleM = MagneticUnit.XMiddle;
    MagneticUnit.YMiddleM = MagneticUnit.YMiddle;
    MagneticUnit.ZMiddleM = MagneticUnit.ZMiddle;
    
    MagneticUnit.GMI_XMiddleM = MagneticUnit.GMI_XMiddle;
    MagneticUnit.GMI_YMiddleM = MagneticUnit.GMI_YMiddle;
    
    intensity = sqrt_16((((uint32)MagneticUnit.XMiddle*(uint32)MagneticUnit.XMiddle)+((uint32)MagneticUnit.YMiddle*(uint32)MagneticUnit.YMiddle))+((uint32)MagneticUnit.ZMiddle*(uint32)MagneticUnit.ZMiddle));
    MagneticUnit.Int_Middle = intensity;
    Sensor_Drift = 0;
    CarCaliFlag = 0;
    MagneticUnit.CarExtremum = 0;
    MagneticUnit.CarIntensity = 0;
    MagneticUnit.CarVariance = 0;
    delay_ms(50);
    halLedClear(4);
    TA0CCTL0 &= ~CCIFG;
    __enable_interrupt();
    
}

void CmdCalibration()
{
    uint16 ADvalueX=0;
    uint16 ADvalueY=0;
    uint16 ADvalueZ=0;
    uint16 GMI_ADvalueX=0;
    uint16 GMI_ADvalueY=0;
    uint16 ADX = 0;
    uint16 ADY = 0;
    uint16 ADZ = 0;
    uint16 GMI_ADX = 0;
    uint16 GMI_ADY = 0;
    uint32 intensity = 0;
    uint8 i=0;
    
    halLedSet(4);
    __disable_interrupt();

    SampleChannel(&GMI_ADX,&GMI_ADY);
    Multi_Read_HMC(&ADX,&ADY,&ADZ);
    for(i=0;i<4;i++)
    {
        delay_ms(100);
        SampleChannel(&GMI_ADvalueX,&GMI_ADvalueY);
        GMI_ADX = (GMI_ADvalueX+GMI_ADX)/2;
        GMI_ADY = (GMI_ADvalueY+GMI_ADY)/2; 
        Multi_Read_HMC(&ADvalueX,&ADvalueY,&ADvalueZ);
        
        ADX = (ADvalueX+ADX)/2;
        ADY = (ADvalueY+ADY)/2;
        ADZ = (ADvalueZ+ADZ)/2;
    }
    
    MagneticUnit.XMiddle = ADX;
    MagneticUnit.YMiddle = ADY;
    MagneticUnit.ZMiddle = ADZ;
    MagneticUnit.XValue = ADX;
    MagneticUnit.YValue = ADY;
    MagneticUnit.ZValue = ADZ;
    MagneticUnit.GMI_XMiddle = GMI_ADX;
    MagneticUnit.GMI_YMiddle = GMI_ADY;

    MagneticUnit.Ext_Middle = abs(MagneticUnit.XMiddle-MagneticUnit.YMiddle);
    MagneticUnit.XMiddleM = MagneticUnit.XMiddle;
    MagneticUnit.YMiddleM = MagneticUnit.YMiddle;
    MagneticUnit.ZMiddleM = MagneticUnit.ZMiddle;
    
    MagneticUnit.XMiddleMF = MagneticUnit.XMiddleM;
    MagneticUnit.YMiddleMF = MagneticUnit.YMiddleM;
    MagneticUnit.ZMiddleMF = MagneticUnit.ZMiddleM;
    
    MagneticUnit.GMI_XMiddleM = MagneticUnit.GMI_XMiddle;
    MagneticUnit.GMI_YMiddleM = MagneticUnit.GMI_YMiddle;
    
    
    
    intensity = sqrt_16((((uint32)MagneticUnit.XMiddle*(uint32)MagneticUnit.XMiddle)+((uint32)MagneticUnit.YMiddle*(uint32)MagneticUnit.YMiddle))+((uint32)MagneticUnit.ZMiddle*(uint32)MagneticUnit.ZMiddle));
    MagneticUnit.Int_Middle = intensity;
    Sensor_Drift = 0;
    CarCaliFlag = 0;
    MagneticUnit.CarExtremum = 0;
    MagneticUnit.CarIntensity = 0;
    MagneticUnit.CarVariance = 0;
    delay_ms(50);
    halLedClear(4);
    TA0CCTL0 &= ~CCIFG;
    __enable_interrupt();
    
}

void GetVariance()
{
    uint32 VarianceY;
    VarianceY = (uint32)abs(MagneticUnit.YValue - MagneticUnit.YMiddle)*(uint32)abs(MagneticUnit.YValue - MagneticUnit.YMiddle);
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
                +((uint32)(MagneticUnit.YValue)*(uint32)(MagneticUnit.YValue))
                +((uint32)(MagneticUnit.ZValue)*(uint32)(MagneticUnit.ZValue));
    MagneticUnit.Intensity = sqrt_16(intensity);
}
void CmdSendHandler()
{
    uint16 i = 0;
    halLedSet(4);
    __disable_interrupt();
    On_Test = 1;
    A7139_StrobeCmd(CMD_STBY);
    delay_ms(10);
    A7139_Init(470.001f);
    delay_ms(10);
    A7139_SetFreq(476.001f);
    delay_us(1);
    A7139_Cal();                    //更改完频率后校准
    delay_us(1);
    A7139_SetPackLen(TEST_LENGTH);
    delay_us(1);
    
    for(i=0;i<1200;i++)
    {
        IdentifyCar();
        TestSend();
        delay_ms(50);
    }
    On_Test = 0;
    halLedClear(4);
    TA0CCTL0 &= ~CCIFG;
    __enable_interrupt();
}
void CmdCarCalibration()
{
    Cmd_CarCal_Flag = 1;
    CmdCalibration();
}
void CmdHandler()
{
    switch(EndPointDevice.cmd)
    {
        case CMD_SEND_TEST:
          CmdSendHandler();
          break;
        case CMD_NOCAR:
          CmdCalibration();
          break;
        case CMD_CAR:
          CmdCarCalibration();
          break;
    }
      
    halLedToggle(1);
}

