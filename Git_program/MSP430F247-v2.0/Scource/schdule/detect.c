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
uint8 Car_Status = 0;
uint8 car_status_memory = 0;



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
        PostTask(EVENT_IDENTIFY_CAR);
    }
    else
    {
        Start_Collect = 1;
    }
//    AD_Value=SampleChannel(0x02);
//    if(AD_middle_value-AD_Value>50)
//    {
//        halLedSet(1);//NKJFK-GPHP7-G8C3J-P6JXR-HQRJR
//        Car_Flag = 1;
//    }
//    else if(AD_Value-AD_middle_value>50)
//    {
//        halLedSet(1);//NKJFK-GPHP7-G8C3J-P6JXR-HQRJR
//        Car_Flag = 1;
//    }
//    else
//    {
//        halLedClear(1);
//        Car_Flag = 0;
//    }
//    if(Car_Flag_Memory!=Car_Flag)
//    {
//        Data_Change_Flag = 1;
//        PostTask(EVENT_DATA_SEND);
//    }
//    else
//    {
//        Data_Change_Flag = 0;
//    }
//    Car_Flag_Memory = Car_Flag;
    

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
void IdentifyCar()
{

    //当前长度为5，长度变化需要修改此处。

    Ave_Slop = (int)((Magnet_Value[1]-Magnet_Value[0]))+
               (int)((Magnet_Value[2]-Magnet_Value[0]))/2+
               (int)((Magnet_Value[3]-Magnet_Value[0]))/3+
               (int)((Magnet_Value[4]-Magnet_Value[0]))/4;
    Ave_Slop = Ave_Slop/COLLECT_WIDTH;
    Start_Collect = 1;
  /*  if(Ave_Slop>SLOP_THRESHLOD)
    {
        Car_Flag = 1;
        halLedSet(1);
    }
    else if(Ave_Slop<-SLOP_THRESHLOD)
    {
        Car_Flag = 1;
        halLedSet(1);
    }
    else
   {
        halLedClear(1);
       // halLedClear(2);
        if(Car_Flag == 1)
        {
            Car_Flag = 0;
      */       // why resample magnet sensor value here? we can use the old data.
if(1){  
  if(1){
            for(int jj=0;jj<5;jj++)
            {
              MagnetValueX[jj]=MagnetValueY[jj]=0;
            }
            delay_ms(100);
            SampleChannel(&MagnetValueX[0],&MagnetValueY[0]);
            //Magnet_Value[0] = SampleChannel(0x02);
            delay_ms(100);
            SampleChannel(&MagnetValueX[1],&MagnetValueY[1]);
            //Magnet_Value[1] = SampleChannel(0x02);
            delay_ms(100);
            SampleChannel(&MagnetValueX[2],&MagnetValueY[2]);
            //Magnet_Value[2] = SampleChannel(0x02);
            delay_ms(100);
            SampleChannel(&MagnetValueX[3],&MagnetValueY[3]);
            //Magnet_Value[3] = SampleChannel(0x02);
            delay_ms(100);
            SampleChannel(&MagnetValueX[4],&MagnetValueY[4]);
            //Magnet_Value[4] = SampleChannel(0x02);
            
            Ave_StableX = (MagnetValueX[0]+MagnetValueX[1]+MagnetValueX[2]+MagnetValueX[3]+MagnetValueX[4])/5;
            Ave_StableY = (MagnetValueY[0]+MagnetValueY[1]+MagnetValueY[2]+MagnetValueY[3]+MagnetValueY[4])/5;
            //AD_middle_value = AD_middle_value;
            if(  (abs(Ave_StableX - AD_middle_valueX)>80)
               ||(abs(Ave_StableY - AD_middle_valueY)>80) )
            {
                Car_Status = 1;
               //halLedClear(2);
            }
            else
            {
                Car_Status = 0;
                //halLedSet(2);
            }
            if(Car_Status != car_status_memory)
            {
                Data_Change_Flag = 1;
            }
            car_status_memory = Car_Status;
        }
        else if (Keep_Alive_Detect == 1)
        {
            Keep_Alive_Detect = 0;
            /*Magnet_Value[0] = SampleChannel(0x02);
            delay_ms(100);
            Magnet_Value[1] = SampleChannel(0x02);
            delay_ms(100);
            Magnet_Value[2] = SampleChannel(0x02);
            delay_ms(100);
            Magnet_Value[3] = SampleChannel(0x02);
            delay_ms(100);
            Magnet_Value[4] = SampleChannel(0x02);
            
            Ave_Stable = (Magnet_Value[0]+Magnet_Value[1]+Magnet_Value[2]+Magnet_Value[3]+Magnet_Value[4])/5;
            */
            SampleChannel(&Ave_StableX,&Ave_StableY); //sample X and Y once
            //AD_middle_value = AD_middle_value;
            if(  (abs(Ave_StableX - AD_middle_valueX)>20)
               ||(abs(Ave_StableY - AD_middle_valueY)>20) )
            {
                Car_Status = 1;
            }
            else
            {
                Car_Status = 0;
            }
        }
    }
    //非低功耗模式测试先注释掉
    if(Data_Change_Flag == 1)
    {
        Data_Change_Flag = 0;
        A7139_Deep_Wake();
        EN_INT;
    }
        
}