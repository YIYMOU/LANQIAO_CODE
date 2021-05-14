#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f10x.h"

#define KB1	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)
#define KB2	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)
#define KB3 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)
#define KB4 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2)

#define KEYPORT  KB1 | (KB2<<1) | (KB3<<2) | (KB4<<3) | 0xf0

extern unsigned char Trg;
extern unsigned char Cont;

void Key_Init(void);
void Key_Read( void );
#endif
