#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"

void pwm_init(uint8_t PA1, FunctionalState PA1State,uint8_t PA2, FunctionalState PA2State);

#endif /*__PWM_H*/
