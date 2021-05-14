#ifndef __UART_H
#define __UART_H
#include "stm32f10x.h"
#include "stdio.h"
extern u8 rx_buf[15];
extern u8 rx_count;
extern u8 rx_ideltime;
extern _Bool rx_flag;

void USART2_Init(void);
void SendString(char *s);
#endif
