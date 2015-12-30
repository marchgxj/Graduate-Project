#ifndef HMC5983_H
#define HMC5983_H
#include "common.h"

extern void Multi_Read_HMC(uint16* XValue,uint16* YValue,uint16* ZValue);
extern void Init_5983();
extern void HMC5983_cal();
extern void Single_Read_HMC(uint16* XValue,uint16* YValue,uint16* ZValue);
#endif