#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"

extern float PA6_Duty;
extern float PA7_Duty;

void pwm_init(void);

#endif /*__PWM_H*/
