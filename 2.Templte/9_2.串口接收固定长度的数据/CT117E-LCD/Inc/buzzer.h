#ifndef __BUZZER_H
#define __BUZZER_H
#include "stm32f10x.h"

#define BEEP_ON()	GPIO_ResetBits(GPIOB,GPIO_Pin_4)
#define BEEP_OFF()  GPIO_SetBits(GPIOB,GPIO_Pin_4)

void Buzzer_Init(void);

#endif
