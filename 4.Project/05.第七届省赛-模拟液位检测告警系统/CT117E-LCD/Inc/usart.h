#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"

extern uint8_t RxBuffer[20];
extern uint8_t RxCounter;
extern uint8_t rx_idle_cnt;
extern _Bool rx_flag;

void usart_init(void);

#endif /*__USART_H*/
