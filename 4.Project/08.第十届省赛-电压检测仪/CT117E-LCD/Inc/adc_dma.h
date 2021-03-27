#ifndef __ADC_DMA_H
#define __ADC_DMA_H

#include "stm32f10x.h"

extern uint16_t ADCConvertedValue;

void adc_dma_init(void);
float adc_dma_get(void);

#endif /*__ADC_DMA_H*/
