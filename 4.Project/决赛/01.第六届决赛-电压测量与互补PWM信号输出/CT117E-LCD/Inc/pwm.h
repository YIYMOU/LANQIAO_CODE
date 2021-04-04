#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"

void pwm_init(uint16_t period,uint16_t pulse, FunctionalState NewState);

#endif /*__PWM_H*/
