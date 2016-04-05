/******************** (C) COPYRIGHT 2013 PNI Sensor Corp *****************
* File Name: ThreeD_magIC.c
* Sample Code for 3D MagIC ASIC
*
*   0 : write
*   1 : read
*************************************************************************/
#include "common.h"
#include "PNI.h"

uint8 circle_cfg[6];

int32 xoffset = 0;
int32 yoffset = 0;
int32 zoffset = 0;
uint8  sensor_count = 0;

const digioConfig pni_mosi_out   = {3, 6, BIT6, HAL_DIGIO_OUTPUT, 0};
const digioConfig pni_miso_in   = {3, 5, BIT5, HAL_DIGIO_INPUT, 0};
const digioConfig pni_sck_out   = {3, 4, BIT4, HAL_DIGIO_OUTPUT, 0};
const digioConfig pni_scs_out   = {3, 7, BIT7, HAL_DIGIO_OUTPUT, 1};
const digioConfig pni_drdy_in   = {3, 3, BIT3, HAL_DIGIO_INPUT, 0};

void PNI_GPIO_Init()
{
  halDigioConfig(&pni_mosi_out);
  halDigioConfig(&pni_miso_in);
  halDigioConfig(&pni_sck_out);
  halDigioConfig(&pni_scs_out);
  halDigioConfig(&pni_drdy_in);
  delay_us(10);
  SCS_1;
  SCK_0;
  delay_ms(1);
}

/***** 初始化PNI ********/
void Init_PNI(void)
{
  PNI_GPIO_Init();
  delay_us(10);
  PNI_write_TMRC(TMRC_VALUE);
  delay_us(10);
  PNI_write_circle(PNI_CIRCLR);
  delay_us(10);
  PNI_read_circle();

  if(  (((circle_cfg[0]<<8)|circle_cfg[1])!=PNI_CIRCLR)
     ||(((circle_cfg[2]<<8)|circle_cfg[3])!=PNI_CIRCLR)
     ||(((circle_cfg[4]<<8)|circle_cfg[5])!=PNI_CIRCLR))
  {
    while(1)
    {
      halLedClear(3);
      halLedClear(4);
      halLedSet(1);
      halLedSet(2);
      delay_ms(500);
      halLedSet(3);
      halLedSet(4);
      halLedClear(1);
      halLedClear(2);
      delay_ms(500);
    }
 }
}

/***** 读配置 ********/
void PNI_read_circle(void)
{
  SCS_0;
  delay_us(1);
  // read register 0x04 command
  SPI_read_write(0x84);
  delay_us(1);
  // read register 0x04 data ，initiate a single measurement
  for(unsigned int i=0;i<6;i++)
  {
    circle_cfg[i] = SPI_read_write(0xFF);
    delay_us(1);
    }
  delay_us(1);
  SCS_1;
  delay_us(1);
}

/***** 写配置 ********/
void PNI_write_circle(uint16 circle_num)
{
  SCS_0;
  delay_us(1);
  // write register 0x04 command
  SPI_read_write(0x04);
  delay_us(1);
  // write register 0x04 data ，initiate a single measurement
  for(unsigned int i=0;i<3;i++)
  {
    SPI_read_write((uint8)(circle_num>>8));
    delay_us(1);
    SPI_read_write((uint8)(circle_num & 0xFF));
    delay_us(1);
  }
  SCS_1;
  delay_us(1);
}

void getOffset()
{
    int32 value = 0;
    uint16 time=0;
    SCS_0;
    delay_us(1);
    SPI_read_write(0x00);
    delay_us(1);
    SPI_read_write(0x70);
    delay_us(1);
    SCS_1;
    delay_us(1);
    while ((DataReady() != 1))
    {
        time++;
        delay_1ms();
        if(time>500)
        {
            sensor_count++;
            if(sensor_count>250)
            {
                sensor_count = 0;
            }
            return;
        }
    }
    
    SCS_0;
    delay_us(1);
    SPI_read_write(0xA4);
    delay_us(1);
    value = ((int8)SPI_read_write(0xFF))<<16 |SPI_read_write(0xFF)<<8|SPI_read_write(0xFF);
    xoffset = VALUE_BASIC - value;
    
    value = ((int8)SPI_read_write(0xFF))<<16 |SPI_read_write(0xFF)<<8|SPI_read_write(0xFF);
    yoffset = VALUE_BASIC - value;
    
    value = ((int8)SPI_read_write(0xFF))<<16 |SPI_read_write(0xFF)<<8|SPI_read_write(0xFF);
    zoffset = VALUE_BASIC - value;
    
    SCS_1;
}

void PNI_read_data(uint16* Xn,uint16* Yn,uint16* Zn)
{      
  int32 sensor;
  uint16 time=0;
  //halLedSet(2);
  // 初始化，开始单次测量
  SCS_0;
  delay_us(1);
  // write register 0x00 command
  SPI_read_write(0x00);
  delay_us(1);
  // write register 0x00 data ，initiate a single measurement
  SPI_read_write(0x70);
  delay_us(1);
  //If data is not ready, wait here
  SCS_1;
  delay_us(1);
  
  // 等待数据 ready
  
  while ((DataReady() != 1))
  {
      time++;
      delay_100us();
      if(time>400)
      {
          sensor_count++;
          if(sensor_count>250)
          {
              sensor_count = 250;
          }
          return;
      }
  }
  
  // 读数据
  SCS_0;
  delay_us(1);
  // read(0x80) register 0x24 command 
  SPI_read_write(0xA4);
  delay_us(1);
  // read register 0x24 data to mag_raw
  sensor = ((int8)SPI_read_write(0xFF))<<16 |SPI_read_write(0xFF)<<8|SPI_read_write(0xFF);
  sensor += xoffset;
  if(sensor<0)
  {
      sensor = 0;
  }
  if(sensor>65534)
  {
      sensor = 65534;
  }
  *Xn = (uint16)sensor;
  
  sensor = ((int8)SPI_read_write(0xFF))<<16 |SPI_read_write(0xFF)<<8|SPI_read_write(0xFF);
  sensor += yoffset;
  if(sensor<0)
  {
      sensor = 0;
  }
  if(sensor>65534)
  {
      sensor = 65534;
  }
  *Yn = (uint16)sensor;
  
  sensor = ((int8)SPI_read_write(0xFF))<<16 |SPI_read_write(0xFF)<<8|SPI_read_write(0xFF);
  sensor += zoffset;
  if(sensor<0)
  {
      sensor = 0;
  }
  if(sensor>65534)
  {
      sensor = 65534;
  }
  *Zn = (uint16)sensor;

  SCS_1;
  //halLedClear(2);
}

int DataReady(void)
{
    return DRDY_PIN;
}

unsigned char SPI_read_write(unsigned char data)
{
  uint8 i;
  for(i=0; i<8; i++)
  {
    if(data & 0x80)
      MOSI_1;
    else
      MOSI_0; 
    data = data << 1;
    if(read_MISO)
      data |= 0x01;
    else
      data &= 0xfe;   
    delay_1us();
    SCK_1;
    delay_1us();
    SCK_0;
    delay_1us(); 
  }
  return data;
}

/***** 读TMRC ********/
uint8 PNI_read_TMRC(void)
{
  uint8 TMRC_now=0;
  SCS_0;
  delay_us(1);
  // read register 0x04 command
  SPI_read_write(0x8B);
  delay_us(1);
  // read register 0x0B data ，determination a fre
  TMRC_now = SPI_read_write(0xFF);
  delay_us(1);
  SCS_1;
  delay_us(1);
  return TMRC_now;
}

/***** 写TMRC ********/
void PNI_write_TMRC(uint8 TMRC)
{
  SCS_0;
  delay_us(1);
  // read register 0x04 command
  SPI_read_write(0x0B);
  delay_us(1);
  // read register 0x0B data ，determination a fre
  SPI_read_write(TMRC);
  delay_us(1);
  SCS_1;
  delay_us(1);
}

void PNI_Reset()
{
    SCS_0;
    SCK_0;
    MOSI_0;
    delay_s(2);
    SCS_1;
    SCK_1;
    MOSI_1;
}
