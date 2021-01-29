#ifndef __RTC_H
#define __RTC_H

#include "stm32f10x.h"
extern uint32_t Tmp_HH ;
extern uint32_t Tmp_MM;
extern uint32_t Tmp_SS;
void RTC_NVIC_Configuration(void);
void RTC_Configuration(void);
void Time_Adjust(void);
void Time_Get(void);
#endif /*__RTC_H*/
