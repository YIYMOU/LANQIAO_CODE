#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_systick.h"

/*
 * t : 定时时间 
 * Ticks : 多少个时钟周期产生一次中断 
 * f : 时钟频率 72000000
 * t = Ticks * 1/f = (72000000/100000) * (1/72000000) = 10us 
 */ 

int main(void)
{
// 来到这里的时候，系统的时钟已经被配置成72M。
	LED_GPIO_Init();
	
	while(1)
	{
		LED_TOGGLE(LED_1);
		SysTick_Delay_ms(500);
	}
}
