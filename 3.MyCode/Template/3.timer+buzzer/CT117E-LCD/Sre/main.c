#include "stm32f10x.h"
#include "lcd.h"
#include "LED.h"
#include "tim.h" 
#include "buzzer.h"

u32 TimingDelay = 0;

void Delay_Ms(u32 nTime);

//Main Body
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	SysTick_Config(SystemCoreClock/1000);
	Delay_Ms(200);
	LED_Init();
	Buzzer_Init();
	Tim_Init();
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	// LCD_DisplayStringLine(Line9,(unsigned char *)"                    ");	
	
	Buzzer_Ctrl(ENABLE);
	Delay_Ms(500);
	Buzzer_Ctrl(DISABLE);
	Delay_Ms(500);
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

void TIM4_IRQHandler(void)
{
	static uint16_t led_cnt = 0;
//	static uint16_t buzzer_cnt = 0;
	static _Bool led_flag = 0;
//	static _Bool buzzer_flag = 0;
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		if(++led_cnt == 500)
		{
			led_cnt = 0;
			if(!led_flag)
			{
				LED_Ctrl(2,ENABLE);
			}
			else
			{
				LED_Ctrl(2,DISABLE);
			}
			led_flag = !led_flag;
		}
//		if(++buzzer_cnt == 500)
//		{
//			buzzer_cnt = 0;
//			if(!buzzer_flag)
//			{
//				Buzzer_Ctrl(ENABLE);
//			}
//			else
//			{
//				Buzzer_Ctrl(DISABLE);
//			}
//			buzzer_flag = !buzzer_flag;
//		}
  }
}
