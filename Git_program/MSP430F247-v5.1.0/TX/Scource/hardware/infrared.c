#include "common.h"
uint16 infraredData[INFRA_LENGTH];
uint8 infrared_count = 0;

uint16 getInfrared()
{
    infraredData[infrared_count++] = sampleInfrared();
    if(infrared_count==INFRA_LENGTH)
    {
        infrared_count = 0;
    }
    return averageFilter(infraredData,INFRA_LENGTH);
}
