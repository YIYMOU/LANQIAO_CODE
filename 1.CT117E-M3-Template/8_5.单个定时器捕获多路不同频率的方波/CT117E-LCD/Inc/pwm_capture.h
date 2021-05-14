#ifndef __PWM_CAPTURE_H
#define __PWM_CAPTURE_H

#include "stm32f10x.h"
extern __IO uint32_t TIM3_CH2_Freq, TIM3_CH1_Freq;
extern __IO uint16_t TIM3_CH2_ReadValue1, TIM3_CH2_ReadValue2;

void TIM2_Input_Mode_Init(void);
void TIM3_Input_Mode_Init(void);
#endif
