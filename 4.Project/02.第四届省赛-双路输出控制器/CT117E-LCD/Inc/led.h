#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

#define LD1 0x01
#define LD2 0x02
#define LD3 0x04
#define LD4 0x08
#define LD5 0x10
#define LD6 0x20
#define LD7 0x40
#define LD8 0x80

void Led_Init(void);
void Led_Ctrl(uint16_t Ledx, FunctionalState NewState);
void Led_Toggle(uint16_t Ledx);

#endif /*__LED_H*/
