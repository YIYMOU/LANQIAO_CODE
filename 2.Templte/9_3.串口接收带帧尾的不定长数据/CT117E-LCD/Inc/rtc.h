#ifndef __RTC_H
#define __RTC_H
#include "stm32f10x.h"
#include "stdio.h"
extern __IO uint32_t TimeDisplay;
extern uint32_t THH, TMM, TSS;

void RTC_Configuration(void);
void Time_Adjust(u8 Tmp_HH,u8 Tmp_MM,u8 Tmp_SS);
void Time_Display(uint32_t TimeVar);
#endif
