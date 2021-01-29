#include "stm32f10x.h"
#include "lcd.h"
#include "LED.h"
#include "stdio.h"
#include "pwm_octoggle.h"
#include "InputCapture.h"

u32 TimingDelay = 0;

uint8_t lcd_buf[20];

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
	PWM_OCTOGGLE_Init();
	InputCapture_Init();
	CCR2_Val = 24000;
	// CH2: Freq=1000Hz£¬Duty=70%
	CCR3_Val = 24000;
	// CH3: Freq=500Hz£¬Duty=50%
	while(1)
	{
		
		sprintf((char*)lcd_buf,"T3CH2_Freq= %8d",TIM3Freq);
		LCD_DisplayStringLine(Line3,lcd_buf);
		
		sprintf((char*)lcd_buf,"T3CH2_Duty= %7d%%",TIM3Duty);
		LCD_DisplayStringLine(Line4,lcd_buf);
		
		sprintf((char*)lcd_buf,"ReadValue1= %8d",IC3ReadValue1);
		LCD_DisplayStringLine(Line5,lcd_buf);
		
		sprintf((char*)lcd_buf,"ReadValue2= %7d%%",IC3ReadValue2);
		LCD_DisplayStringLine(Line6,lcd_buf);
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
