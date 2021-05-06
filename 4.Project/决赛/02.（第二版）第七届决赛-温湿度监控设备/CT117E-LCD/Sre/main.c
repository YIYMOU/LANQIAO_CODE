#include "stm32f10x.h"
#include "string.h"
#include "stdio.h"
#include "lcd.h"
#include "tim.h"
#include "adc.h"
#include "input_capture.h"
#include "key.h"
#include "led.h"
#include "pwm.h"
#include "rtc.h"
#include "i2c.h"
#include "usart.h"

#define		SETTING			0
#define		DATA				1

int current_temperature = 20;
uint8_t current_humodity = 60;

uint8_t Index = 0;

uint8_t recording_times = 0;

int upper_temperature = 40;
uint8_t upper_humodity = 80;
uint8_t sample_interval = 1;
uint16_t test_signal_fre = 1500;

int upper_temperature_temp = 40;
uint8_t upper_humodity_temp = 80;
uint8_t sample_interval_temp = 1;
uint16_t test_signal_fre_temp = 1000;

uint8_t lcd_str[20];
uint8_t select = 0;

uint16_t sample_tick = 0;
_Bool sample_led_flag = 0;
_Bool sample_flag = 0;

union ID {
	uint32_t id;
	uint8_t str[8];
} My_ID;


_Bool interface = DATA;

u32 TimingDelay = 0;

uint16_t led_tick = 0;
_Bool led_flag = 0;

TIME current_time;

struct DATA_ST {
	int current_temperature_l;
	uint8_t current_humodity_l;
	TIME time_l;
} data_st[60];

uint8_t RxBuffer[20];
uint8_t RxCounter = 0;
_Bool RxIdleFlag = 0;

void Delay_Ms(u32 nTime);

void key_scan(void)
{
	key_refresh();
	if(key_falling == B1)
	{
		if(interface == DATA)
		{
			interface = SETTING;
			select = 0;
		}
		else
		{
			interface = DATA;
			upper_temperature = upper_temperature_temp;
			upper_humodity = upper_humodity_temp;
			sample_interval = sample_interval_temp;
			test_signal_fre = test_signal_fre_temp;
			sample_tick = 0;
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
			case 0: if(upper_temperature_temp < 60) upper_temperature_temp++; break;
			case 1: if(upper_humodity_temp < 90) upper_humodity_temp += 5;  break;
			case 2: if(sample_interval_temp < 5) sample_interval_temp++;  break;
			case 3: if(test_signal_fre_temp < 10000) test_signal_fre_temp += 500;  break;
			default : break;
		}
	}
	else if(interface == SETTING && key_falling == B4)
	{
		switch(select)
		{
			case 0: if(upper_temperature_temp > -20) upper_temperature_temp--; break;
			case 1: if(upper_humodity_temp > 10) upper_humodity_temp -= 5;  break;
			case 2: if(sample_interval_temp > 1) sample_interval_temp--;  break;
			case 3: if(test_signal_fre_temp > 1000) test_signal_fre_temp -= 500;  break;
			default : break;
		}
	}
}

void highlight(u8 Line, u8 *ptr)
{
	if((Line == Line3 && select == 0) || (Line == Line5 && select == 1) || (Line == Line7 && select == 2) || (Line == Line9 && select == 3))
	{
		LCD_SetTextColor(Green);
	}
	LCD_DisplayStringLine(Line,ptr);
	LCD_SetTextColor(White);
}

void lcd_proc(void)
{
	if(interface == DATA)
	{
		LCD_DisplayStringLine(Line1 ,(uint8_t *)"   Real-time Data   ");
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str," Now Temp:%d    ",current_temperature);
		LCD_DisplayStringLine(Line3 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str," Now Humo:%d    ",current_humodity);
		LCD_DisplayStringLine(Line5 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str," Real Time:%02d:%02d:%02d  ",current_time.hour,current_time.min,current_time.sec);
		LCD_DisplayStringLine(Line7 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"           Times:%d",recording_times);
		LCD_DisplayStringLine(Line9 ,lcd_str);
	}
	else
	{
		LCD_DisplayStringLine(Line1 ,(uint8_t *)"   Para Setting     ");
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str," Upeer Temp:%dC  ",upper_temperature_temp);
		highlight(Line3 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str," Upeer Humo:%d    ",upper_humodity_temp);
		highlight(Line5 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str," Interval:%d        ",sample_interval_temp);
		highlight(Line7 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str," Test Fre:%d      ",test_signal_fre_temp);
		highlight(Line9 ,lcd_str);
	}
}

void temperature_conv(void)
{
	current_temperature = -20 + (int)((ADCConvertedValue / 4095.0) * 80.0 + 0.5);
}

void pwm_proc(void)
{
	static uint16_t fre_pre = 0;
	if(fre_pre != test_signal_fre)
	{
		fre_pre = test_signal_fre;
		CCR2_Val = 100000 / test_signal_fre;
	}
	CCR2_Val = 1000000 / test_signal_fre;
//	CCR2_Val = 1000000 / test_signal_fre;
}

void input_capture_proc(void)
{
	current_humodity = (uint8_t)(TIM3Freq * 8.0 / 900.0 + 10.0 / 9.0 + 0.5);
}

void led_proc(void)
{
	if(led_flag)
	{
		led_flag = 0;
		if(current_temperature > upper_temperature)
		{
			led_toggle(LD1);
		}
		else
		{
			led_ctrl(LD1,DISABLE);
		}
		if(current_humodity > upper_humodity)
		{
			led_toggle(LD2);
		}
		else
		{
			led_ctrl(LD2,DISABLE);
		}
	}
	if(sample_led_flag)
	{
		sample_led_flag = 0;
		led_toggle(LD3);
	}
}

void rtc_proc(void)
{
	current_time = Time_Display();
}

void check(void)
{
	uint8_t i = 0;
	for(i = 0; i < 8; i++)
	{
		My_ID.str[i] = eeprom_read(0x50 + i);
	}
	
	if(My_ID.id == 1375132689)
	{
		if(eeprom_read(0) == 1)
		{
			upper_temperature = eeprom_read(1);
		}
		else
		{
			upper_temperature = -1 * eeprom_read(1);
		}
		upper_humodity = eeprom_read(2);
		sample_interval = eeprom_read(3);
		test_signal_fre = eeprom_read(4) * 100;
	}
	else
	{
		My_ID.id = 1375132689;
		for(i = 0; i < 8; i++)
		{
			eeprom_write(0x50 + i, My_ID.str[i]);
		}
		eeprom_write(0,1); 	Delay_Ms(5);		// 表示上限温度为正
		eeprom_write(1,40); Delay_Ms(5);		// 初始化上限温度为40
		eeprom_write(2,80); Delay_Ms(5);		// 初始化上限湿度为80
		eeprom_write(3,1); 	Delay_Ms(5);		// 初始化采样间隔为1s
		eeprom_write(4,15); Delay_Ms(5);		// 初始化测试信号为15000Hz
	}
}

void eeprom_proc(void)
{
	static int upper_temperature_pre = 100;
	static uint8_t upper_humodity_pre = 100;
	static uint8_t sample_interval_pre = 100;
	static uint16_t test_signal_fre_pre = 100;
	if(upper_temperature != upper_temperature_pre)
	{
		if(upper_temperature > 0)
		{
			eeprom_write(0,1); 	Delay_Ms(5);
		}
		else
		{
			eeprom_write(0,0); 	Delay_Ms(5);
		}
		eeprom_write(1,upper_temperature); Delay_Ms(5);
		upper_temperature_pre = upper_temperature;
	}
	if(upper_humodity_pre != upper_humodity)
	{
		eeprom_write(2,upper_humodity); Delay_Ms(5);
		upper_humodity_pre = upper_humodity;
	}
	if(sample_interval_pre != sample_interval)
	{
		eeprom_write(3,sample_interval); Delay_Ms(5);
		sample_interval_pre = sample_interval;
	}
	if(test_signal_fre_pre != test_signal_fre)
	{
		eeprom_write(4,test_signal_fre / 100); Delay_Ms(5);
		test_signal_fre_pre = test_signal_fre;
	}
}

void sample_proc(void)
{
	if(sample_flag)
	{
		sample_flag = 0;
		data_st[Index].current_humodity_l = current_humodity;
		data_st[Index].current_temperature_l = current_temperature;
		data_st[Index].time_l = current_time;
		Index = (Index + 1) % 60;
		if(recording_times < 60) recording_times++;
	}
}

void usart_proc(void)
{
	if(RxIdleFlag)
	{
		uint8_t i = 0;
		RxIdleFlag = 0;
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"%20.20s",RxBuffer);
		highlight(Line0 ,lcd_str);
		if(RxCounter == 1 && RxBuffer[0] == 'T')
		{
			for(i = 0;i < Index; i++)
			{
				printf("%d,%d,%02d:%02d:%02d\r\n",data_st[i].current_humodity_l,data_st[i].current_temperature_l,data_st[i].time_l.hour,data_st[i].time_l.min,data_st[i].time_l.sec);
			}
		}
		else if(RxCounter == 1 && RxBuffer[0] == 'C')
		{
			printf("%d,%d,%02d:%02d:%02d\r\n",upper_temperature,upper_humodity,current_time.hour,current_time.min,current_time.sec);
		}
		memset(RxBuffer,0,sizeof(RxBuffer));
		RxCounter = 0;
	}
}

//Main Body
int main(void)
{
	SysTick_Config(SystemCoreClock/1000);

	led_init();
	STM3210B_LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
	
	key_init();
	tim_init();
	adc_init();
	pwm_init();
	input_capture_init();
	rtc_init();
	Time_Adjust(12,50,00);
	i2c_init();
	usart_init();
	check();
	
	while(1)
	{
		
		pwm_proc();
		
		input_capture_proc();
		
		temperature_conv();
		
		led_proc();
		
		rtc_proc();
		
		eeprom_proc();
		
		lcd_proc();
		
		sample_proc();
		
		usart_proc();
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
	static uint8_t key_tick = 0;
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		
		if(++key_tick == 10)
		{
			key_tick = 0;
			key_scan();
		}
		
		if(++led_tick == 500)
		{
			led_tick = 0;
			led_flag = 1;
		}
		
		if(++sample_tick == (sample_interval * 1000))
		{
			sample_tick = 0;
			sample_flag = 1;
			sample_led_flag = 1;
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
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
    RxBuffer[RxCounter++] = USART_ReceiveData(USART2);
		
    if(RxCounter == 20)
    {
			memset(RxBuffer,0,sizeof(RxBuffer));
			RxCounter = 0;
    }
  }
	else if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET) 
	{
		USART_ReceiveData(USART2);
		RxIdleFlag = 1;
	}
}
