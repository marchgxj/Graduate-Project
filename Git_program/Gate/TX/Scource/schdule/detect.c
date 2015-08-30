#include "common.h"
int16 AD_Value;
uint16 AD_middle_valueX = 0; //
uint16 AD_middle_valueY = 0;
uint16 AD_middle_valueXM = 0; //
uint16 AD_middle_valueYM = 0;
uint8 Car_Flag = 0;                             //识别结果
uint8 Car_Flag_Memory = 0;                      //上一次识别结果
int16 Magnet_Value[COLLECT_WIDTH];      //why use CODE??       //store X-Y 传感器采集值
uint16 MagnetValueX[COLLECT_WIDTH];      // store X value       
uint16 MagnetValueY[COLLECT_WIDTH];      // store Y value 
uint8 Start_Collect = 1;       //是否开启采集
uint16 collect_count = 0;
int Ave_Slop = 0;
uint16 Ave_StableX = 0;          //稳定后的值
uint16 Ave_StableY = 0;
uint32 Sum_ValueX = 0;
uint32 Sum_ValueY = 0;
uint16 Draw_DataX = 0;          //绘制曲线数据X轴
uint16 Draw_DataY = 0;          //绘制曲线数据Y轴
uint16 Cal_Time = 0;            //校准周期
uint16 VarianceX = 0;           //X轴方差
uint16 VarianceY = 0;           //Y轴方差
uint16 VarianceAve = 0;         //两轴平均方差
uint16 VarianceM = 0;           //两轴方差差值
uint8  State1_Count = 0;        
uint8  State2_Count = 0;
uint8  State3_Count = 0;
uint8 Parking_State = 0;
uint8 ExtremumCount = 0;        //采集计数
uint16 ExtremumValue = 0;       //1S内极值
uint16 ExtremumValueMiddle = 0; //两轴差标定值

//first calculate the slop of the (X-Y)
//if slop less than thredhold, judge (Xavg1 - Xavg0)  and (Yavg1 - Yavg0)
//input data Xvalue and Y value

void CollectData()
{
    Magnet_Value[collect_count] = SampleChannel(&MagnetValueX[collect_count],&MagnetValueY[collect_count]);
    collect_count++;
    if(collect_count>=COLLECT_WIDTH)
    {
        collect_count = 0;
        Start_Collect = 0;
        //PostTask(EVENT_IDENTIFY_CAR);
    }
    else
    {
        Start_Collect = 1;
    }
   
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
#define STATE2 10                //中间态回无车
#define STATE3 15                //有车回无车

void MultiState(uint16 value,uint16 threshold)
{
    if(value > threshold)
    {
        if(EndPointDevice.parking_state!=CAR)
        {
            State1_Count++;
            if(State1_Count>STATE1)
            {
                EndPointDevice.parking_state = CAR;
                halLedSet(4);
                State1_Count = 0;
            }
            else
            {
                EndPointDevice.parking_state = NOCAR2CAR;
            }
        }
    }
    else
    {
        if(EndPointDevice.parking_state == NOCAR2CAR)
        {
            State2_Count++;
            if(State2_Count>STATE2)
            {
                EndPointDevice.parking_state = NOCAR;
                State2_Count = 0;
                State1_Count = 0;
            }
        }
        else
        {
            if(EndPointDevice.parking_state != NOCAR)
            {
                State3_Count++;
                if(State3_Count>STATE3)
                {
                    EndPointDevice.parking_state = NOCAR;
                    halLedClear(4);
                    State3_Count = 0;
                }
            }
            
        }
        
    }
}
/*方差多状态机识别*/
void VarianceMultiState()
{
    GetVariance();
    MultiState(VarianceM,VAR_THRESHOLD);
}
/*极值多状态机识别*/
void ExtremumMultiState()
{
    GetExtremum();
    MultiState(ExtremumValue,100);
    
    
}
void IdentifyCar()
{
    //VarianceMultiState();
    ExtremumMultiState();
    if(EndPointDevice.parking_state_m!=EndPointDevice.parking_state)
    {
        A7139_Deep_Wake();
        Parking_State = EndPointDevice.parking_state;
        PostTask(EVENT_CSMA_RESEND);
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
    
void Calibration()
{
    uint8 count = 0;
    uint16 ADvalueX=0;
    uint16 ADvalueY=0;
    uint8 i=0;

    AD_middle_valueX = 0;
    AD_middle_valueY = 0;
    halLedToggle(3);
    for(i=0;i<4;i++)
    {
        SampleChannel(&ADvalueX,&ADvalueY);
        if((abs(ADvalueX-AD_middle_valueXM)<80)||(abs(ADvalueY-AD_middle_valueYM)<80))
        {
            count++;
            AD_middle_valueX += ADvalueX;
            AD_middle_valueY += ADvalueY;
        }

        delay_ms(10);
        
    }
    if(count!=0)
    {
        AD_middle_valueX = AD_middle_valueX/count;
        AD_middle_valueY = AD_middle_valueY/count;
        ExtremumValueMiddle = abs(AD_middle_valueX-AD_middle_valueY);
        AD_middle_valueXM = AD_middle_valueX;
        AD_middle_valueYM = AD_middle_valueY;
    }
    
}
void GetVariance()
{
    uint16 ADvalueX=0;
    uint16 ADvalueY=0;
    SampleChannel(&ADvalueX,&ADvalueY);
    
    VarianceX = abs(ADvalueX - AD_middle_valueX)*abs(ADvalueX - AD_middle_valueX);
    VarianceY = abs(ADvalueY - AD_middle_valueY)*abs(ADvalueY - AD_middle_valueY);
    VarianceAve = (VarianceX + VarianceY)>>1;
    VarianceM = abs(VarianceY-VarianceX/100);

}
void GetExtremum()
{
    uint16 ADvalueX=0;
    uint16 ADvalueY=0;
    uint16 minus = 0;

    SampleChannel(&ADvalueX,&ADvalueY);
    minus = abs(ADvalueX - ADvalueY);
    ExtremumValue = abs(minus - ExtremumValueMiddle);
}