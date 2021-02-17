#include "stm32f10x.h"
#include "lcd.h"
#include "led.h"
#include "tim.h"
#include "i2c.h"
#include "key.h"
#include "stdio.h"		// 包含sprintf的头文件
#include "string.h"		// 包含memset的头文件
#include "uart.h"

u32 TimingDelay = 0;

uint16_t led_cnt = 0;
_Bool led_flag = 0;

uint8_t key_scan_cnt = 0;
_Bool key_scan_flag = 0;

uint8_t lcd_str[20];

struct EEPROM
{
	uint8_t uint8;
	uint8_t str[20];
} EEPROM_Write,EEPROM_Read;

void Delay_Ms(u32 nTime);

void key_scan(void)
{
	Key_Refresh();
	if(key_falling == B1)
	{
		Led_Ctrl(LD1,ENABLE);
	}
	if(key_falling == B2)
	{
		Led_Ctrl(LD2,ENABLE);
	}
	if(key_falling == B3)
	{
		Led_Ctrl(LD3,ENABLE);
	}
	if(key_falling == B4)
	{
		Led_Ctrl(LD4,ENABLE);
	}
	
	if(key_state == 0)
	{
		Led_Ctrl(0xff00,DISABLE);
	}
}

//Main Body
int main(void)
{
	SysTick_Config(SystemCoreClock/1000);

	Delay_Ms(200);
	
	Led_Init();
	Tim_Init();
	i2c_init();
	Key_Init();
	Uart_Init();
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
//	EEPROM_Write.uint8 = 123;
//	Write_AT24C02(0x00,EEPROM_Write.uint8);
//	Delay_Ms(5);
//	EEPROM_Read.uint8 = Read_AT24C02(0x00);
	
	LCD_DisplayStringLine(Line1 ,(uint8_t *)" Channel(1): 1000Hz");
	
	LCD_DisplayStringLine(Line3 ,(uint8_t *)" N(1): 2           ");
	
	LCD_DisplayStringLine(Line5 ,(uint8_t *)" Channel(2): 3000Hz");
	
	LCD_DisplayStringLine(Line7 ,(uint8_t *)" Channel(2): 3000Hz");
	
	LCD_DisplayStringLine(Line9 ,(uint8_t *)"               1   ");
	
	
	while(1)
	{
//		if(led_flag)
//		{
//			led_flag = 0;
//			Led_Toggle(LD1);
//		}
		if(key_scan_flag)
		{
			key_scan_flag = 0;
			key_scan();
		}
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}

/**
  * @brief  This function handles TIM4 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM4_IRQHandler(void)		// 1ms产生一次中断
{
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

    if(++led_cnt >= 300)
		{
			led_cnt = 0;
			led_flag = 1;
		}
		
		if(++key_scan_cnt >= 10)
		{
			key_scan_cnt = 0;
			key_scan_flag = 1;
		}
  }
}

