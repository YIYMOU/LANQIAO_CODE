#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"

extern __IO uint16_t CCR1_Val;
extern __IO uint16_t CCR2_Val;
extern float PA6_Duty;
extern float PA7_Duty;

void pwm_init(uint8_t PA6,uint8_t PA7);

#endif /*__PWM_H*/
