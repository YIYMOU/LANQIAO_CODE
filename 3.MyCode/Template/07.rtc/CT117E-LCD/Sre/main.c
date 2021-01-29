#include "stm32f10x.h"
#include "lcd.h"
#include "LED.h"
#include "stdio.h"
#include "rtc.h"

u32 TimingDelay = 0;

uint8_t lcd_string[20];

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
	RTC_Configuration();
	Time_Adjust();
	while(1)
	{
		Time_Get();
		sprintf((char*)lcd_string,"Time: %0.2d:%0.2d:%0.2d", Tmp_HH, Tmp_MM, Tmp_SS);
		LCD_DisplayStringLine(Line4,(unsigned char *)lcd_string);
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}

//void RTC_IRQHandler(void)
//{
//  if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
//  {
//		/* Clear the RTC Second interrupt */
//    RTC_ClearITPendingBit(RTC_IT_SEC);
//  }
//}
