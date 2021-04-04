#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

#define B1		0X01
#define B2		0X02
#define B3		0X04
#define B4		0X08

extern uint8_t key_state;
extern uint8_t key_falling;
extern uint8_t key_rising;

#define key_read	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) << 0 | \
									GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8) << 1 | \
									GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) << 2 | \
									GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2) << 3;

void key_init(void);
void key_refresh(void);


#endif /*__KEY_H*/
