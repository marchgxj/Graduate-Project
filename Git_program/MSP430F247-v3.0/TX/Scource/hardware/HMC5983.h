#ifndef HMC5983_H
#define HMC5983_H
#include "common.h"

extern void Multi_Read_HMC(uint16* XValue,uint16* YValue);
extern void Init_5983();
extern void HMC5983_cal();
#endif