#include "stm32f10x.h"
#include "lcd.h"
#include "LED.h"
#include "pwm.h"

u32 TimingDelay = 0;

void Delay_Ms(u32 nTime);

//Main Body
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	SysTick_Config(SystemCoreClock/1000);
	Delay_Ms(200);
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	LED_Init();
	PWM_Init();

	TIM_SetAutoreload(TIM2,1000 - 1);		// Fre:1KHz
	TIM_SetCompare2(TIM2,500);				// Duty:50%
	TIM_SetCompare3(TIM2,800);				// Duty:80%
	
	while(1){}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
