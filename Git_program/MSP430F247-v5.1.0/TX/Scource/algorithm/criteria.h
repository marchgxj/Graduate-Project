#ifndef _CRITERIA_h_
#define _CRITERIA_h_

extern uint32 getAbsoluteValue();
extern uint16 getExtremum();
extern uint16 getIntensity();
extern uint16 getEuclideanMetric(uint16 x1,uint16 y1,uint16 z1,uint16 x2,uint16 y2,uint16 z2);
extern uint32 getCompatness(uint16 *x,uint16 *y,uint8 length);

#endif