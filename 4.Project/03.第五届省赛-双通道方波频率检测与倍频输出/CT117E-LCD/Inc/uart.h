#ifndef __UART_H
#define __UART_H

#include "stm32f10x.h"

extern uint8_t RxBuffer[20];
extern uint8_t RxCnt;
extern _Bool RxFlag;

void uart_init(void);

#endif /*__UART_H*/
