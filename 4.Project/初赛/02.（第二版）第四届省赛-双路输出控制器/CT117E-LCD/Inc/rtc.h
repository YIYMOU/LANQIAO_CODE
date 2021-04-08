#ifndef __RTC_H
#define __RTC_H

#include "stm32f10x.h"

extern uint32_t THH, TMM, TSS;

void rtc_init(void);
void Time_Adjust(uint8_t Tmp_HH,uint8_t Tmp_MM,uint8_t Tmp_SS);
void Time_Get(void);

#endif /*__RTC_H*/
