#ifndef __PWM_OCTOGGLE_H
#define __PWM_OCTOGGLE_H

#include "stm32f10x.h"

extern uint16_t CCR2_Val;
extern uint16_t CCR3_Val;
extern uint8_t CH3Duty;
extern uint8_t CH2Duty;

void TIM2_PWM_OCTOGGLE_Init(void);
void TIM2_PWM_OCTOGGLE_GPIO_Configuration(void);

#endif /*__PWM_OCTOGGLE_H*/
