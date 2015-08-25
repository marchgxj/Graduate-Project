#include "common.h"
int16 AD_Value;
uint16 AD_middle_valueX = 0; //
uint16 AD_middle_valueY = 0;
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
#define STATE1 5
#define STATE2 5
#define STATE3 5
void IdentifyCar()
{
    if(VarianceM > VAR_THRESHOLD)
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
void Calibration()
{
    uint16 ADvalueX=0;
    uint16 ADvalueY=0;
    
    SampleChannel(&ADvalueX,&ADvalueY);
    
    if((abs(AD_middle_valueX-ADvalueX)<500)&&abs(AD_middle_valueY-ADvalueY))
    {
        AD_middle_valueX = (AD_middle_valueX + ADvalueX)>>1;
        AD_middle_valueY = (AD_middle_valueY + ADvalueY)>>1;
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
    IdentifyCar();
}
void SwitchCamera()
{
    if(Camera_Statue==1)
    {
        SWITCH1_HIGH;
        SWITCH2_HIGH;
        SWITCH3_HIGH;
    }
    if(Camera_Statue==0)
    {
        SWITCH1_LOW;
        SWITCH2_LOW;
        SWITCH3_LOW;
    }
}