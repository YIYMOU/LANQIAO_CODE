#ifndef __UART_H

#include "stm32f10x.h"

extern uint8_t RxBuffer[20];
extern uint8_t RxCounter; 
extern uint16_t uart_cnt;
extern _Bool Rx_flag;
void UART_INIT(void);

#define __UART_H
#endif /*__UART_H*/
