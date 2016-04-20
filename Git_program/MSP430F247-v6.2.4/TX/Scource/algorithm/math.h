#ifndef _MATH_h_
#define _MATH_h_
extern unsigned int sqrt_16(unsigned long M);
extern uint16 getAverage(uint16 *data,uint8 length);
extern uint16 getVariance(uint16 *data,uint8 length);
extern void bubbledata(DataStruct *a,uint16 n) ;
extern void sorted(uint16 *a,uint16 length);
extern uint32 pow(int32 buf,uint8 type);
extern uint32 max(uint32 data1,uint32 data2);
#endif