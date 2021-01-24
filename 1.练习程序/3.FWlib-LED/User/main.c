#include "stm32f10x.h"
#include "bsp_led.h"

void Delay(uint32_t cnt)
{
	while(cnt--);
}

int main(void)
{
// 来到这里的时候，系统的时钟已经被配置成72M。
	LED_GPIO_Init();
	
	while(1)
	{
		LED_TOGGLE(LED_1);
		Delay(0x8FFFFF);
	}
}
