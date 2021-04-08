#ifndef __INPUT_CAPTURE_H
#define __INPUT_CAPTURE_H

#include "stm32f10x.h"

extern __IO uint16_t IC2Value;
extern __IO uint16_t DutyCycle;
extern __IO uint32_t Frequency;

void input_capture_init(void);

#endif /*__INPUT_CAPTURE_H*/
