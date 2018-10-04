#ifndef TEMP_SENSOR_ADC_H_
#define TEMP_SENSOR_ADC_H_

#include "tm4c123gh6pm.h"
void ADC0_InitSWTrigger(void);

uint32_t ADC0_readChannel(void);

#endif