#include "stm32f10x.h"
#include "lcd.h"
#include "LED.h"

u32 TimingDelay = 0;

void Delay_Ms(u32 nTime);

//Main Body
int main(void)
{
	SysTick_Config(SystemCoreClock/1000);
	Delay_Ms(200);
	LED_Init();
	while(1)
	{
		LED_Ctrl(1,ENABLE);
		Delay_Ms(500);
		LED_Ctrl(1,DISABLE);
		Delay_Ms(500);
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
