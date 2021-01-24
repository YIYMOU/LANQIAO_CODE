#include "stm32f10x.h"
#include "lcd.h"
#include "LED.h"
#include "tim.h" 
#include "buzzer.h"
#include "key.h"
#include "stdio.h"

u32 TimingDelay = 0;

_Bool key_read_flag = 0;
_Bool lcd_display_flag = 0;
_Bool led_flag = 0;
_Bool buzzer_flag = 0;

uint16_t key_time = 0;
uint16_t key_pressed_time = 0;
uint16_t lcd_display_cnt = 0;
uint16_t led_cnt = 0;
uint16_t buzzer_cnt = 0;
uint16_t key_cnt = 0;

uint8_t lcd_string[20];


void Delay_Ms(u32 nTime);

void LCD_Display(void)
{
	sprintf((char*)lcd_string,"key_State = %#04X",key_State);
	LCD_DisplayStringLine(Line4,(unsigned char *)lcd_string);	
	sprintf((char*)lcd_string,"pressed time = %-5d",key_pressed_time);
	LCD_DisplayStringLine(Line5,(unsigned char *)lcd_string);	
	if(key_time == 0)
		LCD_DisplayStringLine(Line6,(unsigned char *)"NO KEY PRESSED");
	else if(key_time < 50)
		LCD_DisplayStringLine(Line6,(unsigned char *)"KEY PRESSED  SHORT");
	else if(key_time >= 50)
		LCD_DisplayStringLine(Line6,(unsigned char *)"NO KEY PRESSED LONG");
}

void Key_Scan(void)
{
	Key_Read();
	if(Key_Trg == 0x01)
	{
		LED_Ctrl(0x10,ENABLE);
	}
	else if(Key_Trg == 0x02)
	{
		LED_Ctrl(0x20,ENABLE);
	}
	else if(Key_Trg == 0x04)
	{
		LED_Ctrl(0x40,ENABLE);
	}
	else if(Key_Trg == 0x08)
	{
		LED_Ctrl(0x80,ENABLE);
	}
	
	if(key_State == 0x01)
	{
		key_pressed_time+=10;
		if(key_time != 0xffff && ++key_time >= 50)
		{
			key_time = 0xffff;	// 防止重复进入次程序，在松开按键的时候，将key_time置零			
		}
	}
	
	if(key_State == 0 && Key_Trg == 0)
	{
		key_pressed_time = 0;
		key_time = 0;
		LED_Ctrl(0xF0,DISABLE);
	}
}

//Main Body
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	SysTick_Config(SystemCoreClock/1000);
	Delay_Ms(200);
	LED_Init();
	KEY_Init();
	Buzzer_Init();
	Tim_Init();
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	//LCD_Display();
	while(1)
	{
		
		LCD_Display();
		
		if(key_read_flag)	// 每十毫秒扫描一次按键
		{
			key_read_flag = 0;
			Key_Scan();
		}
		
		if(led_flag)
		{
			led_flag = 0;
			LED_Toggle(0x01);
		}
		
//		if(lcd_display_flag)
//		{
//			lcd_display_flag = 0;
//			LCD_Display();
//		}
		
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
		
		if(++led_cnt == 500)
		{
			led_cnt = 0;
			led_flag = 1;
		}
		
		if(++key_cnt >= 10)
		{
			key_cnt = 0;
			key_read_flag = 1;
		}
		
//		if(++lcd_display_cnt >= 50)
//		{
//			lcd_display_cnt = 0;
//			lcd_display_flag = 1;
//		}
		
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
