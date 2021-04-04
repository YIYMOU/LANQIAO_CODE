#include "stm32f10x.h"
#include "lcd.h"
#include "led.h"
#include "tim.h"
#include "key.h"
#include "adc.h"
#include "stdio.h"
#include "string.h"
#include "i2c.h"
#include "pwm.h"

#define 		START				1
#define 		STOP				0
#define 		SETTING			1
#define 		PARA				0


uint8_t output_freq = 1;
uint8_t output_freq_temp = 0;

u32 TimingDelay = 0;

uint8_t key_cnt = 0;

_Bool interface = 0;

_Bool output_state = 0;

uint16_t led_cnt = 0;
_Bool led_flag = 0;

uint8_t lcd_str[20];

_Bool eeprom_write_flag = 0;


float adc_value = 0.0;

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
		if(output_state == START)
		{
			output_state = STOP;
			pwm_init(output_freq * 1000,50,DISABLE);
		}
		else
		{
			output_state = START;
			pwm_init(output_freq * 1000,50,ENABLE);
		}
	}
	else if(key_falling == B2)
	{
		if(interface == SETTING)
		{
			interface = PARA;
			eeprom_write_flag = 1;
		}
		else
		{
			interface = SETTING;
			output_state =  STOP;
			pwm_init(output_freq * 1000,50,DISABLE);
		}
	}
	else if(key_falling == B3)
	{
		if(interface == SETTING)
		{
			output_freq_temp = (output_freq_temp + 1) % 10;
		}
	}
}

//Main Body
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	SysTick_Config(SystemCoreClock/1000);

	led_init();
	
	STM3210B_LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
	tim_init();
	key_init();
	adc_init();
	i2c_init();
	

	if(eeprom_read(0x10) == 13)
	{
		if(eeprom_read(0x11) == 51)
		{			
			if(eeprom_read(0x12) == 32)
			{
				output_freq = eeprom_read(0x0);
			}
		}
	}
	else
	{
		eeprom_write(0x10,13);
		Delay_Ms(5);
		eeprom_write(0x11,51);
		Delay_Ms(5);
		eeprom_write(0x12,32);
		Delay_Ms(5);
		eeprom_write(0x0,1);
		Delay_Ms(5);
	}
	
	while(1)
	{
		
		if(interface == PARA)
		{
			adc_value = adc_get();
			
			LCD_DisplayStringLine(Line1 ,(unsigned char *)"       PARA         ");
			if(output_state)
			{
				LCD_DisplayStringLine(Line4 ,(unsigned char *)"  STATE:START      ");
			}
			else
			{
				LCD_DisplayStringLine(Line4 ,(unsigned char *)"  STATE:STOP       ");
			}
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char *)lcd_str,"  VALUE:%.2fV   ",adc_value);
			LCD_DisplayStringLine(Line3 ,lcd_str);
			
			if(output_state == START)
				TIM_SetCompare2(TIM1, (12000 / output_freq * adc_value / 3.3));
			
			
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char *)lcd_str,"  PARAM: PA9:  %2d%% ",(uint8_t)(adc_value * 100 / 3.3));
			LCD_DisplayStringLine(Line5 ,lcd_str);
			
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char *)lcd_str,"         PB14: %2d%% ",100 - (uint8_t)((adc_value * 100 / 3.3)));
			LCD_DisplayStringLine(Line6 ,lcd_str);
			
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char *)lcd_str,"         %dKHz      ",output_freq);
			LCD_DisplayStringLine(Line7 ,lcd_str);
			
			LCD_DisplayStringLine(Line9 ,(unsigned char *)"                  1");
		}
		else
		{
			LCD_DisplayStringLine(Line1 ,(unsigned char *)"       SETTING   ");
			LCD_DisplayStringLine(Line2 ,(unsigned char *)"                   ");
			LCD_DisplayStringLine(Line3 ,(unsigned char *)"                   ");
			LCD_DisplayStringLine(Line4 ,(unsigned char *)"                   ");
			
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char *)lcd_str,"    Freq:  %dKHz    ",output_freq_temp + 1);
			LCD_DisplayStringLine(Line5 ,lcd_str);
			
			LCD_DisplayStringLine(Line6 ,(unsigned char *)"                   ");
			LCD_DisplayStringLine(Line7 ,(unsigned char *)"                   ");
			
			LCD_DisplayStringLine(Line9 ,(unsigned char *)"                  2");
		}
		if(interface == PARA && output_state == START)
		{
			led_ctrl(LD1,ENABLE);
		}
		else
		{
			led_ctrl(LD1,DISABLE);
		}
		
		if(eeprom_write_flag)
		{
			eeprom_write_flag = 0;
			eeprom_write(0,output_freq_temp + 1);
			Delay_Ms(5);
			output_freq = output_freq_temp + 1;
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
			key_cnt = 0;
			key_scan();
		}
  }
}
