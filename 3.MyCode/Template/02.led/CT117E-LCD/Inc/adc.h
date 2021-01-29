#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

void GPIO_Configuration(void);
void ADC_Channel18_Init(void);
uint16_t Get_Adc_Value(void);

#endif /*__ADC_H*/
