#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

#define 		LD1						0x0100
#define 		LD2						0x0200
#define 		LD3						0x0400
#define 		LD4						0x0800
#define 		LD5						0x1000
#define 		LD6						0x2000
#define 		LD7						0x4000
#define 		LD8						0x8000
#define 		LD_ALL				0xFF00

void led_init(void);
void led_ctrl(uint16_t LEDx, FunctionalState NewState);
void led_toggle(uint16_t LEDx);

#endif /*__LED_H*/
