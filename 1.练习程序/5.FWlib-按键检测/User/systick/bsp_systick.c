#include "bsp_systick.h"

uint32_t delay_cnt = 0;
/* SystemFrequency / 1000    1ms中断一次
	 * SystemFrequency / 100000	 10us中断一次
	 * SystemFrequency / 1000000 1us中断一次
	 */
#if 0
void SysTick_Delay_us(uint32_t us)
{
	SysTick_Config(SystemCoreClock / 1000000);
	while(delay_cnt != us);
	delay_cnt = 0;
	SysTick->CTRL  &= ~SysTick_CTRL_ENABLE_Msk;    // 关闭SysTick
}

void SysTick_Delay_ms(uint32_t ms)
{
	SysTick_Config(SystemCoreClock / 1000);
	while(delay_cnt != ms);
	delay_cnt = 0;
	SysTick->CTRL  &= ~SysTick_CTRL_ENABLE_Msk;    // 关闭SysTick
}

#elif 0

void SysTick_Delay_us(uint32_t us)
{
	uint32_t i;
	SysTick->LOAD = (SystemCoreClock  / 1000000);
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;
	for(i = 0; i < us; i++)
	{
		while((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == RESET);
	}
	SysTick->CTRL  &= ~SysTick_CTRL_ENABLE_Msk;    // 关闭SysTick
}

void SysTick_Delay_ms(uint32_t ms)
{
	uint32_t i;
	SysTick->LOAD = (SystemCoreClock  / 1000);
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;
	for(i = 0; i < ms; i++)
	{
		while((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == RESET);
	}
	SysTick->CTRL  &= ~SysTick_CTRL_ENABLE_Msk;    // 关闭SysTick
}

#endif
