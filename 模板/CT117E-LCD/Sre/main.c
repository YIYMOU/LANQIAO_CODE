#include "stm32f10x.h"
#include "lcd.h"
#include "i2c.h"
#include "key.h"
#include "led.h"
#include "tim.h"
#include "string.h"
#include "stdio.h"

u32 TimingDelay = 0;

uint8_t key_cnt = 0;

_Bool led_flag = 0;
uint16_t led_cnt = 0;

uint8_t lcd_str[20];


//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}

void key_scan(void)
{
	key_refresh();
	if(key_falling == B1)
	{
		
	}
	else if(key_falling == B2)
	{
		
	}
	else if(key_falling == B3)
	{
		
	}
	else if(key_falling == B4)
	{
		
	}
}

//Main Body
int main(void)
{
	SysTick_Config(SystemCoreClock/1000);

	Delay_Ms(200);
	
	led_init();
	STM3210B_LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
	key_init();
	tim_init();
	i2c_init();
	
	LCD_DisplayStringLine(Line4 ,(unsigned char *)"    Hello,world.   ");
	
	
	while(1)
	{
		
		if(led_flag)
		{
			led_flag =  0;
			
		}
	}
}

/**
  * @brief  This function handles TIM4 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM4_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		
		if(++key_cnt == 10)
		{
			key_cnt =0;
			key_scan();
		}
		
		if(++led_cnt == 1000)
		{
			led_cnt = 0;
			led_flag = 1;
		}
  }
}
