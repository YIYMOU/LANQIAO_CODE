#ifndef __UART_IDLE_RXNE_H

#include "stm32f10x.h"

extern uint8_t RxBuffer[20];
extern _Bool uart2_idle_flag;
extern uint8_t RxCnt;

void UART_IDLE_RXNE_INIT(void);

#define __UART_IDLE_RXNE_H
#endif /*__UART_IDLE_RXNE_H*/
