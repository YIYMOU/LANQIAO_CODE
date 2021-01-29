#include "stm32f10x.h"
#include "lcd.h"
#include "LED.h"
#include "pwm_octoggle.h"

u32 TimingDelay = 0;

void Delay_Ms(u32 nTime);

//Main Body
int main(void)
{
//	uint8_t i;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	SysTick_Config(SystemCoreClock/1000);
	Delay_Ms(200);
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	LED_Init();
	PWM_OCTOGGLE_Init();
	// CH1: Freq=1464.8Hz£¬Duty=0.7
	// CH2: Freq=1464.8Hz£¬Duty=0.5
	
	while(1){}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}

