#ifndef __KEY_H

#include "stm32f10x.h"

extern uint8_t Key_Trg;
extern uint8_t key_State;

void KEY_Init(void);
void Key_Read(void);


#define __KEY_H
#endif /*__KEY_H*/
