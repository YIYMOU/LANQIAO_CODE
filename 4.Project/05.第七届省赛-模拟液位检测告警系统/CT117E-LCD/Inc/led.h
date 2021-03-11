#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

#define LD1		0X0100
#define LD2		0X0200
#define LD3		0X0400
#define LD4		0X0800
#define LD5		0X1000
#define LD6		0X2000
#define LD7		0X4000
#define LD8		0X8000

void led_init(void);


void led_ctrl(uint16_t LEDx,FunctionalState NewState);

void led_toggle(uint16_t LEDx);

#endif /*__LED_H*/
