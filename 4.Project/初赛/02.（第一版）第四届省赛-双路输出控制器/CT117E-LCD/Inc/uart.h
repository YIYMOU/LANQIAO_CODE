#ifndef __UART_H
#define __UART_H

#include "stm32f10x.h"

extern uint8_t RxBuffer[20];
extern uint8_t rx_ms_cnt;
extern uint8_t RxCounter; 

void Uart_Init(void);

#endif /*__UART_H*/
