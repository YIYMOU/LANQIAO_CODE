#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"

extern __IO uint16_t CCR1_Val;
extern __IO uint16_t CCR2_Val;

void pwm_init(void);

#endif /*__PWM_H*/
