#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

#define K1	0x01
#define K2	0x02
#define K3	0x04
#define K4	0x08

#define key_read (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) << 0) | \
									(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8) << 1) | \
									(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) << 2) | \
									(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2) << 3)

extern uint8_t falling_flag;
extern uint8_t rising_flag;
extern uint8_t key_state;

void Key_Init(void);
void Key_Read(void);

#endif /*__KEY_H*/
