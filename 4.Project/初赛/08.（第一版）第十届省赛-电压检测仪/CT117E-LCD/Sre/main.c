#include "stm32f10x.h"
#include "lcd.h"
#include "string.h"
#include "stdio.h"
#include "led.h"
#include "tim.h"
#include "key.h"
#include "adc.h"
#include "adc_dma.h"

#define		MAIN			0
#define		SETTING		1

#define		Upper			0
#define		Lower			1
#define		Normal		2

#define column(x)		(320-(16 * x))

u32 TimingDelay = 0;

_Bool interface = 0;

uint8_t status = 0;

uint16_t lower_led_select = LD2;
uint16_t upper_led_select = LD1;

uint16_t lower_led_select_temp = 2;
uint16_t upper_led_select_temp = 1;

uint16_t led_cnt = 0;
_Bool upper_led_flag = 0;
_Bool lower_led_flag = 0;

uint8_t key_cnt = 0;
_Bool key_flag = 0;

float max_volt = 2.4;
float min_volt = 1.2;

uint8_t max_volt_temp = 24;
uint8_t min_volt_temp = 12;

float adc_value = 0.0;

uint8_t lcd_str[20];

uint8_t select = Line3;

void Delay_Ms(u32 nTime);

void highlight(u8 Line,uint8_t* str)
{
	uint8_t i = 2;
	if(select == Line)
	{
		LCD_DisplayChar(Line,column(0),' ');
		LCD_DisplayChar(Line,column(1),' ');
		LCD_SetBackColor(Green);
		for(i = 2; i < 18	; i++)
		{
			LCD_DisplayChar(Line,column(i),str[i]);;
		}
		LCD_SetBackColor(White);
		LCD_DisplayChar(Line,column(18),' ');
		LCD_DisplayChar(Line,column(19),' ');
	}
	else
	{
		LCD_DisplayStringLine(Line ,str);
	}
}

void key_scan(void)
{
	key_refersh();
	if(key_falling == B1)
	{
		if(interface == SETTING)
		{
			if(max_volt_temp > min_volt_temp && upper_led_select_temp != lower_led_select_temp)
			{
				interface = MAIN;
				max_volt = max_volt_temp / 10.0;
				min_volt = min_volt_temp / 10.0;
				upper_led_select = 0x0100 << (upper_led_select_temp - 1);
				lower_led_select = 0x0100 << (lower_led_select_temp - 1);
			}
		}
		else
		{
			interface = SETTING;
			select = Line3;
		}
	}
	else if(interface == SETTING && key_falling == B2)
	{
		if(select == Line9)
		{
			select = Line3;
		}
		else
		{
			select += 48;
		}
	}
	else if(interface == SETTING && key_falling == B3)
	{
		switch(select)
		{
			case Line3:	
				if(max_volt_temp != 33)
					max_volt_temp += 3;
			
			break;
			case Line5:	
				if(min_volt_temp != 33)
					min_volt_temp += 3;
			break;
			case Line7:	
				if(upper_led_select_temp != 8)
					upper_led_select_temp += 1;
			break;
			case Line9:	
				if(lower_led_select_temp != 8)
					lower_led_select_temp += 1;
			break;
			default : break;
		}
	}
	else if(interface == SETTING && key_falling == B4)
	{
		switch(select)
		{
			case Line3:	
				if(max_volt_temp != 0)
					max_volt_temp -= 3;
			
			break;
			case Line5:	
				if(min_volt_temp != 0)
					min_volt_temp -= 3;
			break;
			case Line7:	
				if(upper_led_select_temp != 1)
					upper_led_select_temp -= 1;
			break;
			case Line9:	
				if(lower_led_select_temp != 1)
					lower_led_select_temp -= 1;
			break;
			default : break;
		}
	}
}

//Main Body
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	SysTick_Config(SystemCoreClock/1000);

	Delay_Ms(200);
	
	led_init();
	STM3210B_LCD_Init();
	LCD_Clear(White);
	LCD_SetBackColor(White);
	LCD_SetTextColor(Blue2);
	
	tim_init();
	key_init();
	//adc_init();
	adc_dma_init();
	
	while(1)
	{
		if(interface == SETTING)
		{
			led_ctrl(lower_led_select | upper_led_select,DISABLE);
			
			LCD_DisplayStringLine(Line1 ,(unsigned char *)"       Setting      ");
			
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char *)lcd_str,"   Max Volt:%.1fV    ",max_volt_temp / 10.0);
			highlight(Line3 ,lcd_str);
			
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char *)lcd_str,"   Min Volt:%.1fV    ",min_volt_temp / 10.0);
			highlight(Line5 ,lcd_str);
			
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char *)lcd_str,"   Upper:LD%d        ",upper_led_select_temp);
			highlight(Line7 ,lcd_str);
			
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char *)lcd_str,"   Lower:LD%d        ",lower_led_select_temp);
			highlight(Line9 ,lcd_str);
		}
		else
		{
			adc_value = ADCConvertedValue / 4096.0 * 3.3;
		
			if(adc_value > max_volt)
			{
				status = Upper;
				if(upper_led_flag)
				{
					upper_led_flag = 0;
					led_toggle(upper_led_select);
				}
				led_ctrl(lower_led_select,DISABLE);
			}
			else if(adc_value < min_volt)
			{
				status = Lower;
				if(lower_led_flag)
				{
					lower_led_flag = 0;
					led_toggle(lower_led_select);
				}
				led_ctrl(upper_led_select,DISABLE);
			}
			else
			{
				status = Normal;
				led_ctrl(lower_led_select | upper_led_select,DISABLE);
			}
			
			LCD_DisplayStringLine(Line1 ,(unsigned char *)"       Main         ");
			LCD_DisplayStringLine(Line3 ,(unsigned char *)"                    ");
			LCD_DisplayStringLine(Line9 ,(unsigned char *)"                    ");
			
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char *)lcd_str,"   Volt:%.2fV       ",adc_value);
			LCD_DisplayStringLine(Line5 ,lcd_str);
			
			switch(status)
			{
				case Upper: 
					LCD_DisplayStringLine(Line7 ,(unsigned char *)"   Status:Upper     ");
				break;
				case Lower: 
					LCD_DisplayStringLine(Line7 ,(unsigned char *)"   Status:Lower     ");
				break;
				case Normal: 
					LCD_DisplayStringLine(Line7 ,(unsigned char *)"   Status:Normal    ");
				break;
				default: break;
			}
		}
		if(key_flag)
		{
			key_flag = 0;
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
void TIM4_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		if(++led_cnt == 200)
		{
			led_cnt = 0;
			upper_led_flag = 1;
			lower_led_flag = 1;
		}
		if(++key_cnt == 10)
		{
			key_cnt = 0;
			key_flag = 1;
		}
  }
}

