#ifndef _FLASH_h_
#define _FLASH_h_

#define PHY_ADDRESS_SEGMENT  0x01000
#define REBOOT_FLAG_SEGMENT  0x01002
#define XMIDDLE_SEGMENT      0x01004
#define YMIDDLE_SEGMENT      0x01006
#define ZMIDDLE_SEGMENT      0x01008

extern void FLASHInit(void);
extern void FLASH_Writew(unsigned int *Address,unsigned int nValue);
extern void FLASH_Writeb(unsigned int *Address,unsigned char nValue);
extern void FLASH_Erase(unsigned int *Address);
extern unsigned int FLASH_Readw(unsigned int *Address);
extern unsigned char FLASH_Readb(unsigned int *Address);
extern void WritePhyAddress();
extern uint16 GetPhyAddress();
extern void write_SegD(char value);
#endif 
