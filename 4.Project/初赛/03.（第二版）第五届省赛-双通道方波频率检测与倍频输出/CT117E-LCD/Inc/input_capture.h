#ifndef __INPUT_CAPTURE_H
#define __INPUT_CAPTURE_H

#include "stm32f10x.h"

extern __IO uint32_t IC3_TIM2Freq;
extern __IO uint32_t IC2_TIM2Freq;

void input_capture_init(void);

#endif /*__INPUT_CAPTURE_H*/
