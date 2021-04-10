#ifndef __INPUT_CAPTURE_H
#define __INPUT_CAPTURE_H

#include "stm32f10x.h"

extern uint32_t Capture;
       
extern uint32_t IC2ReadValue1, IC2ReadValue2;
extern uint32_t TIM2Freq_IC2;
extern uint8_t CaptureNumber_IC2;
       
extern uint32_t IC3ReadValue1, IC3ReadValue2;
extern uint32_t TIM2Freq_IC3;
extern uint8_t CaptureNumber_IC3;
       
void input_capture_init(void);

#endif /*__INPUT_CAPTURE_H*/
