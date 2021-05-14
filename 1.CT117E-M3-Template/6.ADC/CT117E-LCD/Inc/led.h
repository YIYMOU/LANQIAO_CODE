#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"
void LED_Init(void);
void LED_Control(u16 led_ctrl,u8 led_status);
#endif
