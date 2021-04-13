#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

extern __IO uint16_t ADCConvertedValue;

void adc_init(void);

#endif /*__KEY_H*/
