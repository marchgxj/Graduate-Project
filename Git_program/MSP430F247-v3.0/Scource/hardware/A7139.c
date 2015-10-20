#include "common.h"
int A7139_init_err_count = 0;
static void A7139_WriteReg(uint8 regAddr, uint16 regVal)
{
    SCS_CLEAR;
    regAddr |= CMD_Reg_W;
    SPIx_WriteByte(regAddr);
    delay_10us();
    SPIx_WriteWord(regVal);
    delay_10us();
    SCS_SET;
}

static uint16 A7139_ReadReg(uint8 regAddr)
{
    uint16 regVal;
    SCS_CLEAR;
    regAddr |= CMD_Reg_R;
    SPIx_WriteByte(regAddr);
    delay_10us();
    regVal=SPIx_ReadWord();
    delay_10us();
    SCS_SET;
    return regVal;
}

static void A7139_WritePageA(uint8 address, uint16 dataWord)
{
    uint16 tmp;
    tmp = address;
    tmp = ((tmp << 12) | A7139Config[CRYSTAL_REG]);
    A7139_WriteReg(CRYSTAL_REG, tmp);
    A7139_WriteReg(PAGEA_REG, dataWord);
}

static void A7139_WritePageB(uint8 address, uint16 dataWord)
{
    uint16 tmp;
    tmp = address;
    tmp = ((tmp << 7) | A7139Config[CRYSTAL_REG]);
    A7139_WriteReg(CRYSTAL_REG, tmp);
    A7139_WriteReg(PAGEB_REG, dataWord);
}

static uint16 A7139_ReadPageA(uint8 address)
{
    uint16 tmp;
    tmp = address;
    tmp = ((tmp << 12) | A7139Config[CRYSTAL_REG]);
    A7139_WriteReg(CRYSTAL_REG, tmp);
    tmp = A7139_ReadReg(PAGEA_REG);
    return tmp;
}

uint16 look;
static void  A7139_Config(void)
{
    uint8 i;
    for(i=0; i<8; i++)
        A7139_WriteReg(i, A7139Config[i]);
    for(i=10; i<16; i++)
        A7139_WriteReg(i, A7139Config[i]);
    for(i=0; i<16; i++)
        A7139_WritePageA(i, A7139Config_PageA[i]);
    for(i=0; i<5; i++)
        A7139_WritePageB(i, A7139Config_PageB[i]);
    A7139_SetPackLen(MAX_PACK_LENGTH);
    look = A7139_ReadReg(SYSTEMCLOCK_REG);
    if(look != A7139Config[SYSTEMCLOCK_REG])
    {
        Err_State();
        
    }
}
void Err_State(void)
{
    
    while(1){
        halLedToggleAll();
        delay_ms(500);
        A7139_init_err_count++;
        if(A7139_init_err_count == 10)
        {
            REBOOT;
        }
    }
}
uint8 A7139_Cal(void)
{
    uint8  fbcf;	//IF Filter
    uint8  vbcf;	//VCO Current
    uint8  vccf;	//VCO Band
    uint16 tmp;
    //IF calibration procedure @STB state
    A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x0802);			//IF Filter & VCO Current Calibration
    do{
        tmp = A7139_ReadReg(MODE_REG);
    }while(tmp & 0x0802);
    //for check(IF Filter)
    tmp = A7139_ReadReg(CALIBRATION_REG);
    //fb = tmp & 0x0F;
    //fcd = (tmp>>11) & 0x1F;
    fbcf = (tmp>>4) & 0x01;
    if(fbcf)
    {
        Err_State();
    }
    //for check(VCO Current)
    tmp = A7139_ReadPageA(VCB_PAGEA);
    //vcb = tmp & 0x0F;
    vccf = (tmp>>4) & 0x01;
    if(vccf)
    {
        Err_State();
    }
    //RSSI Calibration procedure @STB state
    A7139_WriteReg(ADC_REG, 0x4C00);									//set ADC average=64
    A7139_WritePageA(WOR2_PAGEA, 0xF800);								//set RSSC_D=40us and RS_DLY=80us
    A7139_WritePageA(TX1_PAGEA, A7139Config_PageA[TX1_PAGEA] | 0xE000);	//set RC_DLY=1.5ms
    A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x1000);			//RSSI Calibration
    do{
        tmp = A7139_ReadReg(MODE_REG);
    }while(tmp & 0x1000);
    A7139_WriteReg(ADC_REG, A7139Config[ADC_REG]);
    A7139_WritePageA(WOR2_PAGEA, A7139Config_PageA[WOR2_PAGEA]);
    A7139_WritePageA(TX1_PAGEA, A7139Config_PageA[TX1_PAGEA]);
    //VCO calibration procedure @STB state
    A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x0004);		//VCO Band Calibration
    do{
        tmp = A7139_ReadReg(MODE_REG);
    }while(tmp & 0x0004);
    //for check(VCO Band)
    tmp = A7139_ReadReg(CALIBRATION_REG);
    //vb = (tmp >>5) & 0x07;
    vbcf = (tmp >>8) & 0x01;
    if(vbcf)
    {
        Err_State();
    }
    return 0;
}

void A7139_SetFreq(float rfFreq)
{
    float  divFreq = rfFreq / 12.800f;  
    uint8  intFreq = (uint8)(divFreq); //integer part
    float  fltFreq = divFreq - intFreq * 1.000f; //fraction part
    uint16 fpFreg	= (uint16)(fltFreq * 65536);  //FP register val
    uint16 orgVal;
    A7139_StrobeCmd(CMD_STBY); //enter stand-by mode
    //AFC[15:15] = 0
    orgVal = A7139Config[PLL3_REG] & 0x7FFF;
    A7139_WriteReg(PLL3_REG,orgVal);
    //RFC[15:12] = 0000
    orgVal = A7139Config[PLL6_REG] & 0x0FFF;
    A7139_WriteReg(PLL6_REG,orgVal);
    //MD1[12:12]=0,1
    if(rfFreq < 860)	//433-510
        orgVal = A7139Config[PLL4_REG] & 0xEFFF;
    else	 //868-915
        orgVal = A7139Config[PLL4_REG] | 0x1000;
    A7139_WriteReg(PLL4_REG,orgVal);
    //IP[8:0] = intg
    orgVal = A7139Config[PLL1_REG] & 0xFF00;
    A7139_WriteReg(PLL1_REG,orgVal|intFreq);
    //FP[15:0] =  fpFreg
    A7139_WriteReg(PLL2_REG,fpFreg); 
    //FPA[15:0] = 0x0000
    A7139_WritePageB(IF2_PAGEB,0x0000);	
}

uint8 A7139_RCOSC_Cal(void)
{
    uint8  retry = 0xFF;
    uint16 calbrtVal,t_retry=0xFFFF;
    //RCOSC_E[4:4] = 1,enable internal RC Oscillator
    A7139_WritePageA(WOR2_PAGEA, A7139Config_PageA[WOR2_PAGEA] | 0x0010);
    do{
        //ENCAL[0:0] = 1,then start RC OSC Calbrt
        A7139_WritePageA(WCAL_PAGEA, A7139Config_PageA[WCAL_PAGEA] | 0x0001);
        do{
            calbrtVal = A7139_ReadPageA(WCAL_PAGEA) & 0x0001;
        }while(calbrtVal && t_retry--);
        //read NUMLH[9:1]
        calbrtVal = (A7139_ReadPageA(WCAL_PAGEA) & 0x03FF) >> 1;
        if(calbrtVal>186 && calbrtVal<198)
            return OK_RCOSC_CAL;
    }while(retry--);
    return ERR_RCOSC_CAL;
}

void A7139_StrobeCmd(uint8 cmd)
{
    SCS_CLEAR;
    SPIx_WriteByte(cmd);
    SCS_SET;
}

uint8 A7139_Init(float rfFreq)
{
    SCS_SET;
    SCK_CLEAR;
    MOSI_SET;
    
    A7139_StrobeCmd(CMD_RF_RST);	  //reset A7139 chip
    delay_ms(10);
    
    A7139_Config();		  //config A7139 chip
    delay_ms(10);			  //for crystal stabilized
    A7139_SetCID(0x3475C58C);  //set CID code
    delay_ms(1);
    A7139_SetFreq(rfFreq);	  //set Freq
    delay_ms(10);
    A7139_Cal();
    delay_ms(10);
    Correct_State();	
    A7139_StrobeCmd(CMD_STBY);
    
    return 1;		  //IF and VCO calibration
}   

uint8 A7139_SetCID(uint32 id)
{    
    SCS_CLEAR;
    SPIx_WriteByte(CMD_CID_W);
    SPIx_WriteByte((uint8)(id>>24));
    SPIx_WriteByte((uint8)(id>>16));
    SPIx_WriteByte((uint8)(id>>8));
    SPIx_WriteByte((uint8)id);
    SCS_SET;
    return 0;
}

uint16 A7139_ReadPID(void)
{
    uint16 pid;
    uint16 pagAddr = TX2_PAGEB << 7;
    pagAddr|=A7139Config[CRYSTAL_REG] & 0xF7CF;
    A7139_WriteReg(CRYSTAL_REG, pagAddr);
    pid = A7139_ReadReg(PAGEB_REG);
    return pid;
}

uint8 A7139_SetDataRate(uint8 datRate)
{
    //enter stand by mode
    A7139_StrobeCmd(CMD_STBY);
    delay_ms(20);
    //set xs[0:0] = 0 to disable XTAL
    A7139_WriteReg(CRYSTAL_REG,A7139Config[CRYSTAL_REG] & 0xFFFE );
    switch(datRate)
    {
      case 2:
        {	
            //CSC[2:0]=[001],Fcsck=3.2MHz
            //IFBW[3:2]=[01],100kHz
            //SDR[15:9]=0x18,DCK=2Kps,Fdev = 7.8125kHz
            A7139_WriteReg(SYSTEMCLOCK_REG,0x6621);	
            A7139_WriteReg(RX1_REG,0x18D4); 
            A7139_WritePageA(TX1_PAGEA,0xF505);
        }
        break;
      case 10:
        {
            //CSC[2:0]=[001],Fcsck=6.4MHz
            //IFBW[3:2]=[01],100kHz
            //SDR[15:9]=0x04,DCK=10Kps,Fdev = 37.5kHz
            A7139_WriteReg(SYSTEMCLOCK_REG,0x01221);			 		
            A7139_WriteReg(RX1_REG,0x18D4);
            A7139_WritePageA(TX1_PAGEA,0xF706);
        }
        break;
      case 50:
        {
            //CSC[2:0]=[011],Fcsck=3.2MHz
            //IFBW[3:2]=[00],50Kps 
            //SDR[15:9]=0x00,DCK=50kHz,Fdev = 18.75kHz
            A7139_WriteReg(SYSTEMCLOCK_REG,0x0023);
            A7139_WriteReg(RX1_REG,0x18D0);
            A7139_WritePageA(TX1_PAGEA,0xF606);
        }
        break;
      case 100:												
        {
            //CSC[2:0]=[001] ,Fcsck=6.4MHz
            //IFBW[3:2]=[01],100kHz
            //SDR[15:9]=0x00,DCK=100Kps
            A7139_WriteReg(SYSTEMCLOCK_REG,0x0021);
            A7139_WriteReg(RX1_REG,0x18D4);
            A7139_WritePageA(TX1_PAGEA,0xF706);
        }
        break;
      case 150:
        {
            //Cause LO is 12.8MHz
            //must use Pll clk gen,complement in detail later
            //also can be implemented by the users themselves
            //IFBW=[10] <=> 150kHz
            //DCK=150K
            //CSC=000,fcsck=9.6MHz
            //SDR=0x00
            //DMOS=1,IFBW=150KHz
        }
        break;
      default:
        // set xs[0:0] = 1 to open XTAL
        A7139_WriteReg(CRYSTAL_REG,A7139Config[CRYSTAL_REG] | 0x0001 );
        delay_ms(20);
        return ERR_PARAM;
    }
    // set xs[0:0] = 1 to enable XTAL
    A7139_WriteReg(CRYSTAL_REG,A7139Config[CRYSTAL_REG] | 0x0001 );
    delay_ms(20);
    return 0;
}

uint8 A7139_SetPackLen(uint8 len)
{
    uint16 pagVal;
    A7139_StrobeCmd(CMD_STBY);
    //FEP[7:0]
    pagVal = A7139Config_PageA[FIFO_PAGEA] & 0xFF00;
    A7139_WritePageA(FIFO_PAGEA,pagVal|(len-1));
    //FEP[13:8]
    pagVal = A7139Config_PageA[VCB_PAGEA] & 0xC0FF;
    A7139_WritePageA(VCB_PAGEA,pagVal);
    return 0;			
}

uint8 A7139_SetCIDLen(uint8 len)
{
    switch(len)
    {
      case 2:
        {
            A7139_WritePageA(CODE_PAGEA,A7139Config[CODE_PAGEA] & 0xBFFB);
        }
        break;
      case 4:
        {
            A7139_WritePageA(CODE_PAGEA,A7139Config[CODE_PAGEA] & 0xBFFB);
            A7139_WritePageA(CODE_PAGEA,A7139Config[CODE_PAGEA] | 0x0004);
        }
        break;
      default :
        return ERR_PARAM;
    }
    return 0;
}

void A7139_WriteFIFO(uint8 *buf,uint8 bufSize)
{
    A7139_StrobeCmd(CMD_STBY);
    delay_us(1);
    A7139_StrobeCmd(CMD_TFR);	
    delay_us(1);
    SCS_CLEAR;
    SPIx_WriteByte(CMD_FIFO_W);
    while(bufSize--)
        SPIx_WriteByte(*buf++);
    SCS_SET;	
    A7139_StrobeCmd(CMD_TX);
}

void A7139_ReadFIFO(uint8 *buf,uint8 bufSize)
{
    A7139_StrobeCmd(CMD_RFR);
    delay_us(1);
    SCS_CLEAR;
    SPIx_WriteByte(CMD_FIFO_R);
    while(bufSize--)
        *buf++ = SPIx_ReadByte(); 
    SCS_SET;							 
}

uint8 A7139_IsBatteryLow(uint8 low2_x)
{
    uint16 pagVal;
    if(low2_x<0x02 || low2_x>0x07)
        return ERR_PARAM;
    A7139_StrobeCmd(CMD_STBY);
    //BVT[3:1] BDS[0:0]
    pagVal= A7139Config[PM_PAGEA] & 0xFFF0;
    A7139_WritePageA(PM_PAGEA,pagVal | (low2_x << 1) | 0x01);
    delay_10us(); //delay 5us at least 
    //read VBD[7:7]
    return !((A7139_ReadPageA(WOR1_PAGEA) & 0x0080) >> 7);
}


uint8 A7139_GetRSSI()
{	
    uint8  rssi;
    uint16 t_retry = 0xFFFF;
    //entry RX mode
    A7139_StrobeCmd(CMD_RX);
    //CDM[8:8] = 0
    A7139_WriteReg(ADC_REG,A7139Config[ADC_REG] & 0xFEFF);
    //ADCM[0:0] = 1
    A7139_WriteReg(MODE_REG,A7139_ReadReg(MODE_REG) | 0x0001);
    do
    {
        rssi = A7139_ReadReg(MODE_REG) & 0x0001; //ADCM auto clear when measurement done
    }while(t_retry-- && rssi);
    if(t_retry>0)
        rssi=(A7139_ReadReg(ADC_REG) & 0x00FF);  //ADC[7:0] is the value of RSSI
    else
        rssi = ERR_GET_RSSI;
    A7139_StrobeCmd(CMD_STBY);
    delay_us(1);
    A7139_StrobeCmd(CMD_PLL);
    delay_us(1);
    A7139_StrobeCmd(CMD_RX);
    return rssi;		
}

uint8 A7139_WOT(void)
{
    if(A7139_RCOSC_Cal()==ERR_RCOSC_CAL)
        return ERR_RCOSC_CAL;
    A7139_StrobeCmd(CMD_STBY);
    //GIO1=FSYNC, GIO2=WTR	
    A7139_WritePageA(GIO_PAGEA, 0x0045);
    //setup WOT Sleep time
    A7139_WritePageA(WOR1_PAGEA, 0x027f);
    //WMODE=1 select WOT function
    A7139_WriteReg(PIN_REG, A7139Config[PIN_REG] | 0x0400);
    //WORE=1 to enable WOT function		
    A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x0200);
    //while(1); //ע��˴�Ϊ��ѭ��������ֻΪ��ʾ֮�ã��û����밴ҵ��ʵ���߼��������
    return 0;
}
uint8 A7139_WOR_BySync(void)
{
    A7139_StrobeCmd(CMD_STBY);
    //GIO1=FSYNC, GIO2=WTR	
    A7139_WritePageA(GIO_PAGEA, 0x0045);
    //setup WOR Sleep time and Rx time
    A7139_WritePageA(WOR1_PAGEA, 0xFC05);
    if(A7139_RCOSC_Cal()==ERR_RCOSC_CAL)
        return ERR_RCOSC_CAL;
    //enable RC OSC & WOR by sync
    A7139_WritePageA(WOR2_PAGEA, A7139Config_PageA[WOR2_PAGEA] | 0x0010);
    //WORE=1 to enable WOR function
    A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x0200);
    
    //while(GIO1==0);		//Stay in WOR mode until receiving preamble(preamble ok)
    return 0;
}
uint8 A7139_WOR_ByPreamble(void)
{
    A7139_StrobeCmd(CMD_STBY);
    A7139_WritePageA(GIO_PAGEA, 0x004D);	//GIO1=PMDO, GIO2=WTR
    
    //Real WOR Active Period = (WOR_AC[5:0]+1) x 244us,XTAL and Regulator Settling Time
    //Note : Be aware that Xtal settling time requirement includes initial tolerance, 
    //       temperature drift, aging and crystal loading.
    A7139_WritePageA(WOR1_PAGEA, 0xFC05);	//setup WOR Sleep time and Rx time
    //RC Oscillator Calibration
    if(A7139_RCOSC_Cal()==ERR_RCOSC_CAL)
        return ERR_RCOSC_CAL;
    A7139_WritePageA(WOR2_PAGEA, A7139Config_PageA[WOR2_PAGEA] | 0x0030);	//enable RC OSC & WOR by preamble
    
    A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x0200);				//WORE=1 to enable WOR function
    
    //while(GIO1==0);		//Stay in WOR mode until receiving preamble(preamble ok)
    return 0;
}
void Correct_State(void)
{
    /*halLedToggleAll();
    delay_ms(100);
    halLedToggleAll();
    delay_ms(100);
    halLedToggleAll();*/
}
void A7139_SetChannel(uint8 ch)
{
    uint16 unnDataTemp;
    unnDataTemp = ch*16;// 200k
    A7139_StrobeCmd(CMD_STBY);          //�������
    A7139_WritePageB(IF2_PAGEB, unnDataTemp);//setting PLL1     LSD_RF_WritePageB   
}
uint8 A7139_SetPowerLevel(uint8 pwrLev)
{
    uint16 pagVal= A7139Config_PageB[TX2_PAGEB] & 0xFF80;
    A7139_StrobeCmd(CMD_STBY);
    delay_ms(5);
    switch(pwrLev)
    {
      case 0:
        A7139_WritePageB(TX2_PAGEB, pagVal| 0x01);	//-1.1dBm
        break;
      case 1:
        A7139_WritePageB(TX2_PAGEB, pagVal| 0x09);	//1.2dBm
        break;
      case 2:
        A7139_WritePageB(TX2_PAGEB, pagVal| 0x0A);	//4.8dBm
        break;
      case 3:
        A7139_WritePageB(TX2_PAGEB, pagVal| 0x61);	//6.7dBm
        break;
      case 4:
        A7139_WritePageB(TX2_PAGEB, pagVal| 0x2A);	//10.9dBm
        break;
      case 5:
        A7139_WritePageB(TX2_PAGEB, pagVal| 0x1C);	//13.2dBm
        break;
      case 6:
        A7139_WritePageB(TX2_PAGEB, pagVal| 0x26);	//18dBm
        break;
      case 7:
        A7139_WritePageB(TX2_PAGEB, pagVal| 0x3E);	//20dBm
        break;
      case 8:
        A7139_WritePageB(TX2_PAGEB, pagVal| 0x5F);  //21dBm
        break;
      default:
        delay_ms(5);
        return ERR_PARAM;
    }
    delay_ms(5);
    return 0;
}
#if	RF_WOR_RECV == 1
//-------------------------------------------//
//����A7139Ӳ��WOR��WOT���ܣ�
//-------------------------------------------//
//---------OSCУ׼--------
static uint8 A7139_RCOSC_Cal(void)
{
    uint8  retry = 0xFF;
    uint16 calbrtVal,t_retry=0xFFFF;
    A7139_WritePageA(WOR2_PAGEA, A7139Config_PageA[WOR2_PAGEA] | 0x0010);		//RCOSC_E[4:4] = 1,enable internal RC Oscillator	
    do{	
        A7139_WritePageA(WCAL_PAGEA, A7139Config_PageA[WCAL_PAGEA] | 0x0001);	//ENCAL[0:0] = 1,then start RC OSC Calbrt
        do{
            calbrtVal = A7139_ReadPageA(WCAL_PAGEA) & 0x0001;
        }while(calbrtVal && t_retry--);	
        calbrtVal = (A7139_ReadPageA(WCAL_PAGEA) & 0x03FF) >> 1;	//read NUMLH[9:1])
        if(calbrtVal>186 && calbrtVal<198)
        {
            A7139_WritePageA(WCAL_PAGEA, A7139Config_PageA[WCAL_PAGEA]);//�ر�RCУ׼
            return OK_RCOSC_CAL;
        }	   
    }while(retry--);
    A7139_WritePageA(WCAL_PAGEA, A7139Config_PageA[WCAL_PAGEA]);//�ر�RCУ׼
    return ERR_RCOSC_CAL;
}
//---------����WOR��˯�ߺͻ���ʱ��--------
uint8 A7139_WOT_init(float actie_period_ms,float sleep_period_ms)
{
    uint16 wor_ac,wor_sl,wor1_pagea_value;
    if((actie_period_ms<0.245f)|(actie_period_ms>15.625f)|(sleep_period_ms<7.800f)|(sleep_period_ms>7990.000f))
        return	ERR_PARAM;
    wor_ac = (uint16)(actie_period_ms*4.096f-1.000f);
    wor_sl = (uint16)(sleep_period_ms*4.096f/32.000f-1.000f);
    wor1_pagea_value = (wor_ac<<10)|(wor_sl<<0);
    if(A7139_RCOSC_Cal()==ERR_RCOSC_CAL)
        return ERR_RCOSC_CAL;
    A7139_StrobeCmd(CMD_STBY);//GIO1=FSYNC, GIO2=WTR
    A7139_WritePageA(GIO_PAGEA, 0x0045);//setup WOT Sleep time
    A7139_WritePageA(WOR1_PAGEA, wor1_pagea_value);//WMODE=1 select WOT function	0x027f
    A7139_WriteReg(PIN_REG, A7139Config[PIN_REG] | 0x0400);//WORE=1 to enable WOT function			
    A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x0200);
    //while(1); //ע��˴�Ϊ��ѭ��������ֻΪ��ʾ֮�ã��û����밴ҵ��ʵ���߼��������
    return 0;
}
//---------------------------------------------------//
//-uint8 A7139_WOR_BySync1(int actie_period,int sleep_period)
//-actie_period ��Ծʱ�� ��Чֵ 0.245f-15.625f  	ms��
//-sleep_period ˯��ʱ�� ��Чֵ 7.800f-7990.000f  	ms	
//-ע����Խ��Ϊ��Ծʱ���˯��ʱ��Ҫ�̣����齫��Ծʱ���ֵ�ԼӴ󣬿���ʾ��������GIO2��
//--------------------------------------------------//
uint8 A7139_WOR_BySync_init(float actie_period_ms,float sleep_period_ms)
{
    uint16 wor_ac,wor_sl,wor1_pagea_value;
    if((actie_period_ms<0.245f)|(actie_period_ms>15.625f)|(sleep_period_ms<7.800f)|(sleep_period_ms>7990.000f))
        return	ERR_PARAM;
    wor_ac = (uint16)(actie_period_ms*4.096f-1.000f);
    wor_sl = (uint16)(sleep_period_ms*4.096f/32.000f-1.000f);
    wor1_pagea_value = (wor_ac<<10)|wor_sl;
    A7139_StrobeCmd(CMD_STBY);			
    A7139_WritePageA(GIO_PAGEA, 0x0045);	//GIO1=FSYNC, GIO2=WTR		
    A7139_WritePageA(WOR1_PAGEA, wor1_pagea_value);	//setup WOR Sleep time and Rx time//0xFC05 1111 11 00 0000 0101 ����15.6ms��˯��47ms		
    if(A7139_RCOSC_Cal()==ERR_RCOSC_CAL)
        return ERR_RCOSC_CAL;
    A7139_WritePageA(WOR2_PAGEA, A7139Config_PageA[WOR2_PAGEA] | 0x0010);//0x0000enable RC OSC & WOR by sync	
    A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x0200);//WORE=1 to enable WOR function
    return 0;
}
//---------------------------------------------------//
//-uint8 A7139_WOR_ByPreamble1(int actie_period,int sleep_period)
//-actie_period ��Ծʱ�� ��Чֵ 0.245f-15.625f  	ms��
//-sleep_period ˯��ʱ�� ��Чֵ 7.800f-7990.000f  	ms	
//-ע����Խ��Ϊ��Ծʱ���˯��ʱ��Ҫ�̣����齫��Ծʱ���ֵ�ԼӴ󣬿���ʾ��������GIO2��
//--------------------------------------------------//
uint8 A7139_WOR_ByPreamble_init(float actie_period_ms,float sleep_period_ms)
{
    uint16 wor_ac,wor_sl,wor1_pagea_value;
    if((actie_period_ms<0.245f)|(actie_period_ms>15.625f)|(sleep_period_ms<7.800f)|(sleep_period_ms>7990.000f))
        return	ERR_PARAM;
    wor_ac =(uint16)(actie_period_ms*4.096f-1.000f);
    wor_sl = (uint16)(sleep_period_ms*4.096f/32.000f-1.000f);
    wor1_pagea_value = (wor_ac<<10)|wor_sl;
    
    A7139_StrobeCmd(CMD_STBY);
    A7139_WritePageA(GIO_PAGEA, 0x004D);	//GIO1=PMDO, GIO2=WTR
    A7139_WritePageA(WOR1_PAGEA, wor1_pagea_value);	//setup WOR Sleep time and Rx time //0xFC05 1111 11 00 0000 0101 ����15.6ms��˯��47ms
    if(A7139_RCOSC_Cal()==ERR_RCOSC_CAL)
        return ERR_RCOSC_CAL;
    A7139_WritePageA(WOR2_PAGEA, A7139Config_PageA[WOR2_PAGEA] | 0x0030);	//0x0030enable RC OSC & WOR by preamble
    A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x0200);				//WORE=1 to enable WOR function
    return 0;
}
//---------------------------------------------------//
//-void A7139_WOR_rece_init(void)
//status 1,ʹ��WOR���չ��ܣ�0��ֹ
//--------------------------------------------------//
void A7139_WOR_rece_init(uint8 status)
{
    gpio_isr_disable();				//д����ǰ���жϣ����������ж�
    A7139_StrobeCmd(CMD_STBY);		//�������
    if(status)		//��
    {
        rf_wor_start = RF_WOR_OPEN;
        A7139_SetPackLen(2);		//������Ϊ2
        A7139_WritePageA(CODE_PAGEA,0x1503);    //ǰ��4+2ID(��CRC��FEC)
        A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x0200);//WORE=1 to enable WOR function
        gpio_isr_enable_init();			//���ж�
    }
    else
    {
        rf_wor_start = RF_WOR_CLOSE;
        A7139_SetPackLen(CONFIG_PACK_LEN);
        A7139_WritePageA(CODE_PAGEA,A7139Config_PageA[CODE_PAGEA]);  
        A7139_WriteReg(MODE_REG, A7139Config[MODE_REG]);
        A7139_StrobeCmd(CMD_RX);		//�������
        gpio_isr_enable_init(); 			//���ж�
    }
}
//-------------------------------------------//
//����A7139Ӳ��WOR��WOT����
//-------------------------------------------//
#endif	//RF_WOR_RECV == 1


#if RF_WOR_SEND == 1
//---------------------------------------------------//
//-void A7139_WOR_send_init(void)
//��ʼWOR���Ѱ����͹���
//--------------------------------------------------//
void A7139_WOR_send_init(uint8 status)
{	
    A7139_StrobeCmd(CMD_STBY);		//�������
    Delay_us(10); 
    if(status)
    {
        A7139_SetPackLen(2);
        A7139_WritePageA(CODE_PAGEA,0x1503);    //ǰ��4+2ID(��CRC��FEC)	 
        A7139_StrobeCmd(CMD_TFR);	 	//TX FIFO address pointer reset
    }
    else
    {
        A7139_SetPackLen(CONFIG_PACK_LEN);
        A7139_WritePageA(CODE_PAGEA,A7139Config_PageA[CODE_PAGEA]);
        A7139_StrobeCmd(CMD_TFR);	 	//TX FIFO address pointer reset
    }
}
//---------------------------------------------------//
//-void rf_wor_send(uint8 *buf,uint8 bufSize)
//WOR���Ѱ�����
//--------------------------------------------------//
void rf_wor_send(uint8 *buf,uint8 bufSize)
{
    uint16 j,i = 0;
    uint8 wor_data[2];
    wor_data[0] = buf[2]+buf[3]+buf[4]+buf[5];
    wor_data[1] = buf[12]+buf[13]+buf[14]+buf[15];
    A7139_WOR_send_init(1);
    A7139_StrobeCmd(CMD_STBY);		//�������
    Delay_us(10);
    A7139_WriteFIFO(wor_data,2); 	//������д��A7139������
    Delay_us(10);
    for(j=0;j<=RF_SEND_WOR_TIMES;j++)
    {
        A7139_StrobeCmd(CMD_TX); 		//������������
        i =0; 
        while(!GIO1_IN())
        {
            Delay_us(10);
            if(i++ >= 2000)
                break;
        }
        CLR_WDT();
        i =0; 
        while(GIO1_IN())
        {
            if(i++ >= 5000)
            {
                CLR_WDT();				//50ms��ʱ�˳�������ģ���쳣��������ѭ��
                break;
            }
            Delay_us(10); 
        }
        CLR_WDT();	
    } 
    A7139_WOR_send_init(0); 
}
#endif //RF_WOR_SEND
void RXMode()
{
    A7139_StrobeCmd(CMD_PLL);
    delay_us(1);
    A7139_StrobeCmd(CMD_RX);
    delay_us(1);
    EndPointDevice.state = CMD_RX;
}
void SendPack()
{
    uint8 timeout = 0;
    EndPointDevice.state = CMD_TX;
    A7139_StrobeCmd(CMD_PLL);
    delay_us(1);
    A7139_WriteFIFO(DataSendBuffer,MAX_PACK_LENGTH);
    delay_us(1);
    A7139_StrobeCmd(CMD_TX);
    while(GIO1)
    {
        if(timeout++>10)
        {
            break;
        }
        delay_1ms();
    }
    halLedClear(1);
    Send_Error_Flag = 0;
    

}

void A7139_Sleep(void)
{
#if (SLEEP_EN)
    A7139_StrobeCmd(CMD_SLEEP);
#endif
}
void A7139_DeepSleep(void)
{
#if (SLEEP_EN)    
    A7139_StrobeCmd(CMD_DEEP_SLEEP);
#endif
}
void A7139_Wake(void)
{
#if (SLEEP_EN)   
    A7139_StrobeCmd(CMD_STBY);
    delay_us(1);
    A7139_StrobeCmd(CMD_PLL);
    delay_ms(4);
#endif    
}
void A7139_WakeToRecv(void)
{
#if (SLEEP_EN)    
    A7139_Wake();
    RXMode();
#endif
}
void A7139_Deep_Wake(void)
{
#if (SLEEP_EN)   
    DIS_INT;
    DIS_TIMER1;
    DIS_TIMER0;
    A7139_StrobeCmd(CMD_STBY);
    delay_ms(10);
    A7139_StrobeCmd(CMD_STBY);
    delay_ms(10);
    A7139_Init(ChannelList[EndPointDevice.channel]);
    delay_ms(10);
    A7139_Init(ChannelList[EndPointDevice.channel]);
    delay_ms(10);
    RXMode();
    delay_ms(10);
    RXMode();
    delay_ms(10);
    EN_INT;
    EN_TIMER1;
    EN_TIMER0;

#endif   
}
void A7139_Reset()
{
    A7139_Deep_Wake();
}


