#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

extern uint8_t Key_Trg_Falling;
extern uint8_t Key_Trg_Rising;
extern uint8_t key_State;

void KEY_Init(void);
void Key_Read(void);


#endif /*__KEY_H*/
