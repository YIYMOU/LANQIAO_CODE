#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

#define 					B1								0x01
#define 					B2								0x02
#define 					B3								0x04
#define 					B4								0x08

#define						KEY_STATE_GET			GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) << 0 | \
																		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8) << 1 | \
																		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) << 2 | \
																		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2) << 3;
extern uint8_t key_state;
extern uint8_t key_falling;
extern uint8_t key_rising;

void key_init(void);
void key_refresh(void);

#endif /*__KEY_H*/
