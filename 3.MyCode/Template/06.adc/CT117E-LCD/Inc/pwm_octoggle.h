#ifndef __PWM_OCTOGGLE_H

#include "stm32f10x.h"

extern uint16_t CCR2_Val;
extern uint16_t CCR3_Val;

void PWM_OCTOGGLE_RCC_Configuration(void);
void PWM_OCTOGGLE_GPIO_Configuration(void);
void PWM_OCTOGGLE_Init(void);

#define __PWM_OCTOGGLE_H
#endif /*__PWM_OCTOGGLE_H*/
