#ifndef __RTC_H
#define __RTC_H

#include "stm32f10x.h"

typedef struct
{
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
} TIME;

void rtc_init(void);
TIME Time_Display(void);
void Time_Adjust(uint32_t Tmp_HH,uint32_t Tmp_MM ,uint32_t Tmp_SS);

#endif /*__RTC_H*/
