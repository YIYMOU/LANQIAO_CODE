#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include "stdio.h"

extern uint8_t RxBuffer[20];
extern uint8_t RxCounter;
extern _Bool UsartFlag;
extern uint8_t usart_ticker;

void usart_init(void);



#endif /*__USART_H*/
