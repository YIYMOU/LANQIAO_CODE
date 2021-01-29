#ifndef __INPUTCAPTURE_H

#include "stm32f10x.h"

extern uint32_t IC3ReadValue1, IC3ReadValue2;
extern uint32_t CaptureNumber;
extern uint32_t Capture;
extern uint32_t TIM3Freq;
extern uint32_t TIM3Duty;

void InputCapture_RCC_Configuration(void);
void InputCapture_GPIO_Configuration(void);
void InputCapture_NVIC_Configuration(void);
void InputCapture_Init(void);

#define __INPUTCAPTURE_H
#endif /*__INPUTCAPTURE_H*/
