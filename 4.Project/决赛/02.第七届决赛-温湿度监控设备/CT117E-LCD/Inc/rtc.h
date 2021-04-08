#ifndef __RTC_H
#define __RTC_H

#include "stm32f10x.h"


extern uint8_t THH, TMM, TSS;

void rtc_init(void);
void Time_Adjust(uint32_t Tmp_HH,uint32_t Tmp_MM,uint32_t Tmp_SS);
void time_refresh(void);

#endif /*__RTC_H*/
