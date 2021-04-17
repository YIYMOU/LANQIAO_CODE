#include "stm32f10x.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "tim.h"
#include "adc.h"
#include "rtc.h"
#include "pwm.h"
#include "usart.h"
#include "stdio.h"
#include "i2c.h"
#include "string.h"
#include "input_capture.h"

// inputcapture×¢Òâ£¬Òç³ö

#define			DATA							0
#define			SETTING						1

u32 TimingDelay = 0;

uint16_t led_cnt = 0;
_Bool led_flag = 0;

uint8_t key_cnt = 0;
_Bool key_flag = 0;

_Bool interface = 0;

int temperature = 0;
uint8_t humidity = 0;

uint8_t lcd_str[20];

struct TIME {
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
}	time_now = {12,50,0},time_temp = {0,0,0};

struct STORAGE {
	int temp;
	uint8_t humi;
	struct TIME tt;
} data[60];

uint8_t pointer =0 ;

uint8_t recording_time = 0;

uint16_t sampling_interval_cnt = 0;
_Bool sampling_interval_flag = 0;

uint8_t sampling_interval = 1;

uint8_t test_signal = 15;

int temperature_upper_limit = 40;
uint8_t humidity_upper_limit = 40;

uint8_t select = 0;

_Bool write_flag = 0;

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
		if(interface == SETTING)
		{
			interface = DATA;
			pwm_init(test_signal * 100);
			write_flag = 1;
		}
		else
		{
			interface = SETTING;
			select = 0;
		}
	}
	else if(key_falling == B2)
	{
		if(interface == SETTING)
		{
			select = (select + 1) % 4;
		}
	
	}
	else if(key_falling == B3)
	{
		if(interface == SETTING)
		{
			if(select == 0)
			{
				if(temperature_upper_limit < 60)
					temperature_upper_limit++;
			}
			else if(select == 1)
			{
				if(humidity_upper_limit < 90)
					humidity_upper_limit += 5;
			}
			else if(select == 2)
			{
				if(sampling_interval < 5)
					sampling_interval += 1;
			}
			else if(select == 3)
			{
				if(test_signal < 100)
					test_signal += 5;
			}
		}
	}
	else if(key_falling == B4)
	{
		if(interface == SETTING)
		{
			if(select == 0)
			{
				if(temperature_upper_limit > -20)
					temperature_upper_limit--;
			}
			else if(select == 1)
			{
				if(humidity_upper_limit > 10)
					humidity_upper_limit -= 5;
			}
			else if(select == 2)
			{
				if(sampling_interval > 1)
					sampling_interval -= 1;
			}
			else if(select == 3)
			{
				if(test_signal > 10)
					test_signal -= 5;
			}
		}
	}
}

void hilight(u8 Line, u8 *ptr)
{
	u32 i = 0;
	u16 refcolumn = 319;//319;
	if((select == 0 && Line == Line2) || (select == 1 && Line == Line4) || (select == 2 && Line == Line6) || (select == 3 && Line == Line8))
		LCD_SetTextColor(Green);
	while ((*ptr != 0) && (i < 20))	 //	20
	{
		LCD_DisplayChar(Line, refcolumn, *ptr);
		refcolumn -= 16;
		ptr++;
		i++;
	}
	LCD_SetTextColor(White);
}

//Main Body
int main(void)
{
	uint8_t temp;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	SysTick_Config(SystemCoreClock/1000);
	
	led_init();
	STM3210B_LCD_Init();
	
	tim_init();
	key_init();
	adc_init();
	rtc_init();
	usart_init();
	input_capture_init();
	i2c_init();
	
	Time_Adjust(time_now.hour,time_now.min,time_now.sec);
	
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
	if(eeprom_read(0x11) == 12 && eeprom_read(0x12) == 12 && eeprom_read(0x13) == 12)
	{
		temp = eeprom_read(0);
		
		if(temp > 0)
		{
			temperature_upper_limit = eeprom_read(1);
		}
		else
		{
			temperature_upper_limit = -1 * eeprom_read(1);
		}
		humidity_upper_limit = eeprom_read(2);
		sampling_interval = eeprom_read(3);
		test_signal = eeprom_read(4);
	}
	else
	{
		eeprom_write(0,1);
		Delay_Ms(5);
		eeprom_write(1,40);
		Delay_Ms(5);
		eeprom_write(2,40);
		Delay_Ms(5);
		eeprom_write(3,1);
		Delay_Ms(5);
		eeprom_write(4,15);
		Delay_Ms(5);
		eeprom_write(0x11,12);
		Delay_Ms(5);
		eeprom_write(0x12,12);
		Delay_Ms(5);
		eeprom_write(0x13,12);
		Delay_Ms(5);
	}
	
	
	
	pwm_init(test_signal * 100);
	
	while(1)
	{
		
		temperature = -20 + (int)(adc_get() * 80 / 3.3 + 0.5);
		time_refresh();
		time_now.hour = THH;
		time_now.min = TMM;
		time_now.sec = TSS;
		
		humidity = (uint8_t)(((Frequency + 100) / 500 * 500) * 8.0 / 900.0 + 10.0 / 9.0);
		

		if(interface == SETTING)
		{
			LCD_DisplayStringLine(Line0 ,(unsigned char *)" PARAMETER-SETTING ");
			
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char*)lcd_str,"TEMP LIMIT: %dC  ",temperature_upper_limit);
			hilight(Line2 ,lcd_str);
			
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char*)lcd_str,"HUMI LIMIT: %d%%",humidity_upper_limit);
			hilight(Line4 ,lcd_str);
			
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char*)lcd_str,"SAMP INTER: %dS      ",sampling_interval);
			hilight(Line6 ,lcd_str);
			
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char*)lcd_str,"TEST SIGNA: %d.%dKHz  ",test_signal / 10,test_signal % 10);
			hilight(Line8 ,lcd_str);
		}
		else
		{
			LCD_DisplayStringLine(Line0 ,(unsigned char *)"  REAL-TIME DATA   ");
			
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char*)lcd_str,"CURR TEMPE: %dC  ",temperature);
			LCD_DisplayStringLine(Line2 ,lcd_str);
			
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char*)lcd_str,"CURR HUDIT: %d%%",humidity);
			LCD_DisplayStringLine(Line4 ,lcd_str);
			
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char*)lcd_str,"REAL CLOCK: %02d:%02d:%02d",time_now.hour,time_now.min,time_now.sec);
			LCD_DisplayStringLine(Line6 ,lcd_str);
			
			
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char*)lcd_str,"RECORDING TIMES: %d",recording_time);
			LCD_DisplayStringLine(Line8 ,lcd_str);
			
			if(sampling_interval_flag)
			{
				sampling_interval_flag = 0;
				data[pointer].humi = humidity;
				data[pointer].temp = temperature;
				data[pointer].tt = time_now;
				pointer = (pointer + 1) % 60;
				if(recording_time < 60)
					recording_time++;
				led_toggle(LD3);
			}
		}
		
		if(write_flag)
		{
			write_flag = 0;
			if(temperature_upper_limit > 0)
			{
				eeprom_write(0,1);
				Delay_Ms(5);
				eeprom_write(1,(uint8_t)temperature_upper_limit);
				Delay_Ms(5);
			}
			else
			{
				eeprom_write(0,0);
				Delay_Ms(5);
				eeprom_write(1,(uint8_t)(-1 *temperature_upper_limit));
				Delay_Ms(5);
			}
			eeprom_write(2,humidity_upper_limit);
			Delay_Ms(5);
			eeprom_write(3,sampling_interval);
			Delay_Ms(5);
			eeprom_write(4,test_signal);
			Delay_Ms(5);
		}
		
		if(led_flag)
		{
			led_flag = 0;
			if(temperature > temperature_upper_limit)
			{
				led_toggle(LD1);
			}
			else
			{
				led_ctrl(LD1,DISABLE);
			}
			if(humidity > humidity_upper_limit)
			{
				led_toggle(LD2);
			}
			else
			{
				led_ctrl(LD2,DISABLE);
			}
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
		if(++led_cnt == 500)
		{
			led_cnt = 0;
			led_flag = 1;
		}
		if(++key_cnt == 10)
		{
			key_cnt = 0;
			key_scan();
		}
		if(interface == DATA && ++sampling_interval_cnt == (uint16_t)(sampling_interval * 1000))
		{
			sampling_interval_cnt = 0;
			sampling_interval_flag = 1;
		}
		
  }
}

/**
  * @brief  This function handles USART2 global interrupt request.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{
	uint8_t ch;
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
    ch = USART_ReceiveData(USART2);
		if(ch == 'T')
		{
			uint8_t i;
			for(i  = 0; i < recording_time; i++)
			{
				printf("%d:\r\n",i);
				printf("Temperature: %d\r\n",data[i].temp);
				printf("Humidity: %d\r\n",data[i].humi);
				printf("Time: %02d:%02d:%02d\r\n",data[i].tt.hour,data[i].tt.min,data[i].tt.sec);
			}
		}
		else if(ch == 'C')
		{
			printf("Temperature Upper Limit: %d:\r\n",temperature_upper_limit);
			printf("Humidity Upper Limit: %d:\r\n",humidity_upper_limit);
		}
  }
}
