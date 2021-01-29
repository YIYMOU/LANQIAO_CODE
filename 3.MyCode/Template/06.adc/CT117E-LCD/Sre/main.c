#include "stm32f10x.h"
#include "lcd.h"
#include "LED.h"
#include "tim.h" 
#include "stdio.h"
#include "adc.h"

u32 TimingDelay = 0;

_Bool adc_flag = 0;

uint16_t adc_cnt = 0;

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
	Tim_Init();
	ADC_Channel18_Init();
	while(1)
	{
		if(adc_flag)
		{
			adc_flag = 0;
			sprintf((char*)lcd_string,"%.2fV               ",Get_Adc_Value() * 3.3f / 4096);
			LCD_DisplayStringLine(Line5,(unsigned char *)lcd_string);
		}
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
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		
		if(++adc_cnt >= 50)
		{
			adc_cnt = 0;
			adc_flag = 1;
		}
  }
}
