#ifndef __LED_H

#include "stm32f10x.h"

void LED_Init(void);
void LED_Ctrl(uint16_t LEDx,FunctionalState NewState);
void LED_Toggle(uint16_t LEDx);

#define __LED_H
#endif /*__LED_H*/
