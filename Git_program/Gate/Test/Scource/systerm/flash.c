#include "common.h"
uint16 address_segment = PHY_ADDRESS_SEGMENT;
/****************************************************************************************

*NAME:       void FLASHInit(void)
*FUNCTION:   FLASH��ʼ��
*Parameter:  �޲���
*RETURN:     �޷���ֵ
*****************************************************************************************/
void FLASHInit(void)
{
    FCTL2=FWKEY+FSSEL_1+FN3+FN4;//ʱ��ԴΪMCLK,25��Ƶ
}

/****************************************************************************************
*NAME:       void FLASH_Writew(unsigned int *Address,unsigned int nValue)
*FUNCTION:   ��һ��WORD���͵�����д�뵽ָ����FLASHż����ʼ��ַ
*Parameter:  Address��д����ʼ��ַ��nValue��д��ֵ
*RETURN:     �޷���ֵ
*****************************************************************************************/
void FLASH_Writew(unsigned int *Address,unsigned int nValue)
{
    while((FCTL3&BUSY)==BUSY);    //�ȴ�FLASH�洢����ɲ���
    FCTL3=FWKEY;             // LOCK = 0
    FCTL1=FWKEY+WRT;             // WRT = 1
    
    *Address=nValue;
    
    FCTL1=FWKEY;        // WRT = 0
    while((FCTL3&BUSY)==BUSY);    //�ȴ�FLASH�洢����ɲ���
    FCTL3=FWKEY+LOCK;   // LOCK = 1
    
}

/****************************************************************************************
*NAME:       void FLASH_Writeb(unsigned int *Address,unsigned char nValue)
*FUNCTION:   ��һ��BYTE���͵�����д�뵽ָ����FLASH��ַ
*Parameter:  Address��д���ַ��nValue��д��ֵ
*RETURN:     �޷���ֵ
*****************************************************************************************/
void FLASH_Writeb(unsigned int *Address,unsigned char nValue)
{
    FCTL1=FWKEY+WRT;             // WRT = 1
    FCTL3=FWKEY;             // LOCK = 0
    while((FCTL3&BUSY)==BUSY);    //�ȴ�FLASH�洢����ɲ���
    *Address=nValue;
    FCTL1=FWKEY;        // WRT = 0
    FCTL3=FWKEY+LOCK;   // LOCK = 1
    while((FCTL3&BUSY)==BUSY);    //�ȴ�FLASH�洢����ɲ���
}

/****************************************************************************************
*NAME:       void FLASH_Erase(unsigned int *Address)
*FUNCTION:   ��FLASH��������ݲ�����
*Parameter:  Address��д���ַ���������⣩
*RETURN:     �޷���ֵ
*****************************************************************************************/
void FLASH_Erase(unsigned int *Address)
{
    
    while((FCTL3&BUSY)==BUSY);    //�ȴ�FLASH�洢����ɲ���
    FCTL3=FWKEY;                 // LOCK = 0
    FCTL1=FWKEY+ERASE;             //ERASE=1
    *Address=0;            //����ڵ�ַ����д��������������
    FCTL1=FWKEY;
    while((FCTL3&BUSY)==BUSY);    //�ȴ�FLASH�洢����ɲ���
    FCTL3=FWKEY+LOCK;
}

/****************************************************************************************
*NAME:       unsigned int FLASH_Readw(unsigned int *Address)
*FUNCTION:   ��ָ��FLASHż����ַ��ʼ��������
*Parameter:  Address��������ַ
*RETURN:     ���ض���ֵ
*****************************************************************************************/
unsigned int FLASH_Readw(unsigned int *Address)
{
    unsigned int Rdata;
    Rdata=*Address;
    return Rdata;
}

/****************************************************************************************
*NAME:       unsigned char FLASH_Readb(unsigned int *Address)
*FUNCTION:   ��ָ��FLASH��ַ���ֽ�����
*Parameter:  Address��������ַ
*RETURN:     ���ض���ֵ
*****************************************************************************************/
unsigned char FLASH_Readb(unsigned int *Address)
{
    unsigned char Rdata;
    Rdata=*Address;
    return Rdata;
    
}



void WritePhyAddress()
{
#if (WRITE_PHY_ADDRESS)
    FLASH_Erase((uint16 *)PHY_ADDRESS_SEGMENT);
    FLASH_Writew((uint16 *)PHY_ADDRESS_SEGMENT,PHY_ADDRESS);
#endif
}
unsigned int GetPhyAddress()
{
    return FLASH_Readw((uint16 *)PHY_ADDRESS_SEGMENT);
}





