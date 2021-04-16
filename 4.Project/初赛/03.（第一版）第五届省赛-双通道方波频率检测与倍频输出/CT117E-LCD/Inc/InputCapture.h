#ifndef __INPUT_CAPTURE_H
#define __INPUT_CAPTURE_H

#include "stm32f10x.h"

extern uint32_t IC3ReadValue1, IC3ReadValue2;
extern uint32_t IC2ReadValue1, IC2ReadValue2;
extern uint32_t IC3_CaptureNumber;
extern uint32_t IC2_CaptureNumber;
extern uint32_t IC3_Capture;
extern uint32_t IC2_Capture;
extern uint32_t TIM2_IC3_Freq;
extern uint32_t TIM2_IC2_Freq;
extern uint32_t IC3_Update_Cnt;
extern uint32_t IC2_Update_Cnt;


void InputCapture_init(void);


#endif /*__INPUT_CAPTURE_H*/
