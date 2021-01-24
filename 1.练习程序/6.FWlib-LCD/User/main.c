#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_key.h"

void Delay(uint32_t cnt)
{
	while(cnt--);
}

int main(void)
{
	LED_GPIO_Init();
	Key_Init();
	
	while(1)
	{
		if(Key_Scan() == KEY1)
		{
			LED_TOGGLE(LED_1);
		}
		else if(Key_Scan() == KEY2)
		{
			LED_TOGGLE(LED_2);
		}
		else if(Key_Scan() == KEY3)
		{
			LED_TOGGLE(LED_3);
		}
		else if(Key_Scan() == KEY4)
		{
			LED_TOGGLE(LED_4);
		}
	}
}
