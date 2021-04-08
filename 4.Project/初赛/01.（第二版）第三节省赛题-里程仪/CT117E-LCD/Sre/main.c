#include "stm32f10x.h"
#include "lcd.h"
#include "led.h"
#include "tim.h"
#include "key.h"
#include "pwm.h"
#include "i2c.h"
#include "input_capture.h"
#include "stdio.h"
#include "string.h"

u32 TimingDelay = 0;

uint8_t key_cnt = 0;

uint8_t lcd_str[20];
uint8_t lcd_temp[20];

uint16_t led_cnt = 0;
_Bool led_flag = 0;

uint32_t total_distance = 0;	// 总里程，单位：m

uint8_t Vim = 0;		// 单位：km/h
uint8_t Vavg = 0;	// 单位：km/h

uint32_t driving_time = 0;

uint8_t out_fre = 0;

uint16_t sec_cnt = 0;

union DATA {
	uint32_t distance;
	uint8_t str[8];
} eeprom_data;

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
		if(out_fre != 20)
		{
			input_capture_init();
			pwm_init(++out_fre);
		}
	}
	else if(key_falling == B4)
	{
		if(out_fre != 0)
		{
			input_capture_init();
			pwm_init(--out_fre);
			if(out_fre == 0)
			{
				DutyCycle = 0;
				Frequency = 0;
				driving_time = 0;
				current_distance = 0;
				sec_cnt = 0;
			}
		}
	}
}

//Main Body
int main(void)
{
	uint8_t i;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	SysTick_Config(SystemCoreClock/1000);
	
	led_init();
	STM3210B_LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
	key_init();
	tim_init();
	i2c_init();
	pwm_init(0);
	// input_capture_init();
	
	for(i = 0; i < 8;i++)
	{
		eeprom_data.str[i] = eeprom_read(i + 0x10);
	}
	
	if(eeprom_data.distance != 1375132689)
	{
		eeprom_data.distance = 1375132689;
		for(i = 0; i < 8;i++)
		{
			eeprom_write(i + 0x10,eeprom_data.str[i]);
			Delay_Ms(5);
		}
	}
	else
	{
		for(i = 0; i < 8;i++)
		{
			eeprom_data.str[i] = eeprom_read(i);
		}
		total_distance = eeprom_data.distance;
	}
	
	while(1)
	{
		
		Vim = (uint8_t)(Frequency * 2 * 3600 / 1000.0 + 0.5);
		
		eeprom_data.distance = total_distance;
		for(i = 0; i < 8;i++)
		{
			eeprom_write(i,eeprom_data.str[i]);
			Delay_Ms(5);
		}
		
		if(driving_time)
			Vavg = (uint8_t)(current_distance / 1000.0) / driving_time * 3600 / 1000;
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"Vim(km/h): %d     ",Vim);
		LCD_DisplayStringLine(Line1 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"Vavg(km/h): %d    ",Vavg);
		LCD_DisplayStringLine(Line3 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"T(h:m:s): %02d:%02d:%02d   ",driving_time / 3600,driving_time % 3600 / 60,driving_time % 60);
		LCD_DisplayStringLine(Line5 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"S(km) : %.1f      ",(float)(current_distance / 1000.0));
		LCD_DisplayStringLine(Line7 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_temp));			// 这一部分的代码是字符串右对齐显示
		sprintf((char *)lcd_temp,"Total(km):%d",total_distance / 1000);
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"%20s",lcd_temp);
		LCD_DisplayStringLine(Line9 ,lcd_str);
		
		if(led_flag)
		{
			led_flag = 0;
			if(Vim > 90)
			{
				led_toggle(LD1);
			}
		}
		
		if(Vim <= 90)
		{
			led_ctrl(LD1,DISABLE);
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
		
		if(out_fre && ++sec_cnt == 1000)
		{
			sec_cnt = 0;
			driving_time++;
		}
  }
}
