#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"
void TIM2_PWM_Init(void);
void TIM3_PWM_Init(void);
void TIM2_PWM_OCToggle(void);
void TIM3_PWM_OCToggle(void);

extern __IO uint16_t TIM2_CCR2_Val;
extern __IO uint16_t TIM2_CCR3_Val;

extern __IO uint16_t TIM3_CCR1_Val;
extern __IO uint16_t TIM3_CCR2_Val;
#endif
