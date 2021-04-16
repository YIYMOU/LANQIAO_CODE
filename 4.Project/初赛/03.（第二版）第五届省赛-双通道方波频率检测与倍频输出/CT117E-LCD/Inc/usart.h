#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"

extern uint8_t Buffer[20];
extern __IO uint8_t RxNumOfReceived;
extern uint8_t RxCnt;
extern _Bool RxFlag;
extern _Bool RxIdleFlag;
extern uint8_t ch[4];

void usart_init(void);

#endif /*__USART_H*/
