#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"

extern uint16_t CCR1_Val;
extern uint16_t CCR2_Val;
extern float CH1_Duty;
extern float CH2_Duty;

void pwm_init(void);

#endif /*__PWM_H*/

