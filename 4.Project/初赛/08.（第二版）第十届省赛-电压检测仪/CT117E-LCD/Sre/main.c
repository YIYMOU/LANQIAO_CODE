#include "stm32f10x.h"
#include "lcd.h"
#include "i2c.h"
#include "key.h"
#include "led.h"
#include "adc.h"
#include "tim.h"
#include "string.h"
#include "stdio.h"

#define 		SETTING			0
#define 		DATA				1

#define			Upper				0
#define			Lower				1
#define			Normal				2

u32 TimingDelay = 0;

uint8_t key_cnt = 0;

_Bool led_flag = 0;
uint16_t led_cnt = 0;

uint8_t lcd_str[20];

float Volt = 0.0;

uint8_t status = Normal;

float Max_Volt = 2.4;
float Min_Volt = 1.2;

uint8_t Max_Volt_temp = 24;
uint8_t Min_Volt_temp = 12;

uint8_t Upper_LED_temp = 1;
uint8_t Lower_LED_temp = 2;

uint8_t Upper_LED = 1;
uint8_t Lower_LED = 2;

uint8_t select = 0;

_Bool interface = DATA;


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
		if(interface == SETTING && (Upper_LED_temp != Lower_LED_temp) && (Max_Volt_temp > Min_Volt_temp))
		{
			interface = DATA;
			Upper_LED = Upper_LED_temp;
			Lower_LED = Lower_LED_temp;
			Max_Volt = Max_Volt_temp / 10.0;
			Min_Volt = Min_Volt_temp / 10.0;
		}
		else
		{
			interface = SETTING;
			select = 0;
		}
	}
	else if(interface == SETTING && key_falling == B2)
	{
		select = (select + 1) % 4;
	}
	else if(interface == SETTING && key_falling == B3)
	{
		switch(select)
		{
			case 0: 
				if(Max_Volt_temp < 33)
					Max_Volt_temp += 3;
			break;
			case 1: 
				if(Min_Volt_temp < 33)
					Min_Volt_temp += 3;
			break;
			case 2: 
				if(Upper_LED_temp < 8)
					Upper_LED_temp += 1;
			break;
			case 3: 
				if(Lower_LED_temp < 8)
					Lower_LED_temp += 1;
			break;
		}
	}
	else if(interface == SETTING && key_falling == B4)
	{
		switch(select)
		{
			case 0: 
				if(Max_Volt_temp > 0)
					Max_Volt_temp -= 3;
			break;
			case 1: 
				if(Min_Volt_temp > 0)
					Min_Volt_temp -= 3;
			break;
			case 2: 
				if(Upper_LED_temp > 1)
					Upper_LED_temp -= 1;
			break;
			case 3: 
				if(Lower_LED_temp > 1)
					Lower_LED_temp -= 1;
			break;
		}
	}
}

void status_proc(void)
{
	if(Volt > Max_Volt)
	{
		status = Upper;
	}
	else if(Volt < Min_Volt)
	{
		status = Lower;
	}
	else
	{
		status = Normal;
	}
}

void highlight(u8 Line, u8 *ptr)
{
	u32 i = 0;
	u16 refcolumn = 319;//319;
	
	while ((*ptr != 0) && (i < 20))	 //	20
	{
		if((i >= 1 && i <= 17) && ((Line == Line3 && select == 0) || (Line == Line5 && select == 1) || (Line == Line7 && select == 2) || (Line == Line9 && select == 3)))
			LCD_SetBackColor(Green);
		LCD_DisplayChar(Line, refcolumn, *ptr);
		LCD_SetBackColor(Black);
		refcolumn -= 16;
		ptr++;
		i++;
	}
}

void lcd_proc(void)
{
	if(interface == SETTING)
	{
		LCD_DisplayStringLine(Line1 ,(unsigned char *)"      Setting       ");
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"  Max Volt: %.2fV    ",Max_Volt_temp / 10.0);
		highlight(Line3 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"  Min Volt: %.2fV    ",Min_Volt_temp / 10.0);
		highlight(Line5 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"  Upper: LD%d    ",Upper_LED_temp);
		highlight(Line7 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"  Lower: LD%d    ",Lower_LED_temp);
		highlight(Line9 ,lcd_str);
		
	}
	else
	{
		
		LCD_DisplayStringLine(Line1 ,(unsigned char *)"        Main       ");
		
		LCD_DisplayStringLine(Line3 ,(unsigned char *)"                   ");
		LCD_DisplayStringLine(Line9 ,(unsigned char *)"                   ");
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"  Volt: %.2fV      ",Volt);
		LCD_DisplayStringLine(Line5 ,lcd_str);
		
		switch(status)
		{
			case Upper: LCD_DisplayStringLine(Line7 ,(unsigned char *)"  Status: Upper       "); break;
			case Lower: LCD_DisplayStringLine(Line7 ,(unsigned char *)"  Status: Lower       "); break;
			case Normal: LCD_DisplayStringLine(Line7 ,(unsigned char *)"  Status: Normal       "); break;
			default : LCD_DisplayStringLine(Line7 ,(unsigned char *)"  Status: Normal       "); break;
		}
	}
}

void led_proc(void)
{
	if(led_flag)
	{
		led_flag =  0;
		if(status == Upper && interface == DATA)
		{
			led_toggle((0x0100 << (Upper_LED - 1)));
			led_ctrl((~(0x0100 << (Upper_LED - 1))),DISABLE);
		}
		else if(status == Lower && interface == DATA)
		{
			led_toggle((0x0100 << (Lower_LED - 1)));
			led_ctrl((~(0x0100 << (Lower_LED - 1))),DISABLE);
		}
		else
		{
			led_ctrl(LD_ALL,DISABLE);
		}
	}
}

void adc_proc(void)
{
	Volt = ADCConvertedValue / 4095.0 * 3.3;
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
	adc_init();
	
	
	
	while(1)
	{
		
		adc_proc();
		
		status_proc();
		
		lcd_proc();
		
		led_proc();
		
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
		
		if(++led_cnt == 200)
		{
			led_cnt = 0;
			led_flag = 1;
		}
  }
}
