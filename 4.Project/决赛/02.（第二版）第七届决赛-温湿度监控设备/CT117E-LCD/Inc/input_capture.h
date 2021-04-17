#ifndef __INPUT_CAPTURE_H
#define __INPUT_CAPTURE_H

#include "stm32f10x.h"

extern __IO uint16_t IC3ReadValue1, IC3ReadValue2;
extern __IO uint16_t CaptureNumber;
extern __IO uint32_t Capture;
extern __IO uint32_t TIM3Freq;

void input_capture_init(void);

#endif /*__INPUT_CAPTURE_H*/
