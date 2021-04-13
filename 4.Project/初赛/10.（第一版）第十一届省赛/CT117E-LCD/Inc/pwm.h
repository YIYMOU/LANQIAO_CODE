#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"

extern float pa6_duty;
extern float pa7_duty;

void pwm_init(void);

#endif /*__PWM_H*/
