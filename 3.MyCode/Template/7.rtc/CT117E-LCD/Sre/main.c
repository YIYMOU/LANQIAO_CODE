#include "stm32f10x.h"
#include "lcd.h"
#include "LED.h"
#include "tim.h" 
#include "buzzer.h"
#include "key.h"
#include "stdio.h"
#include "i2c.h"
#include "string.h"
#include "adc.h"
#include "rtc.h"

u32 TimingDelay = 0;

_Bool key_read_flag = 0;
_Bool lcd_display_flag = 0;
_Bool led_flag = 0;
_Bool buzzer_flag = 0;
_Bool adc_flag = 0;
_Bool TimeDisplay = 0;

uint16_t key_time = 0;
uint16_t key_pressed_time = 0;
uint16_t lcd_display_cnt = 0;
uint16_t led_cnt = 0;
uint16_t buzzer_cnt = 0;
uint16_t key_cnt = 0;
uint16_t adc_cnt = 0;

uint16_t key1Time = 0;
uint8_t key1PressNumber = 0;

uint8_t lcd_string[20];


union eeprom_dat
{
	uint8_t t1;
	uint16_t t2;
	uint32_t t3;
	int16_t t4;
	float f1;
	double f2;
	unsigned char str[20];
}eeprom_dat_write,eeprom_dat_read;

//利用共用体读写EEPROM
union eeprom_float
{
   float a;  
   u8 b[4];
}float_write,float_read;

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
	
	if(key1PressNumber == 1)	// 检测到一次按键的上升沿，开始计时
	{
		key1Time += 10;			// 由于按键程序是10ms扫描一次，所以这里加10ms
		if(key1Time >= 150)	// 如果超过1500ms，表示按键是单击，初始化双击检测
		{
			// begin
			// 在这里添加按键单击的代码
			LCD_DisplayStringLine(Line8,(unsigned char *)"SINGLE              ");
			// end
			key1Time = 0;
			key1PressNumber = 0;
		}
	}
	
	if(Key_Trg_Rising == 0x01)	// 按键的上升沿，即按键松开
	{
		if(key1PressNumber == 0)	// 表示按键是第一次按键的上升沿
		{
			key1PressNumber = 1;		// 做个标记
		}
	}
	
	if(Key_Trg_Falling == 0x01)	// 检测到按键的下降沿
	{
		LED_Ctrl(0x10,ENABLE);		
		if(key1PressNumber == 1)	// 如果已经捕获到了一次按键的上升沿，并且现在还没有超时，那么这就是一个双击的动作
		{
			// begin
			// （在这里添加双击需要处理的程序）
			// end
			LCD_DisplayStringLine(Line8,(unsigned char *)"DOUBLE              ");	
			key1Time = 0;
			key1PressNumber = 0;		// 初始化按键双击相关的变量
		}		
	}
	else if(Key_Trg_Falling == 0x02)
	{
		LED_Ctrl(0x20,ENABLE);
	}
	else if(Key_Trg_Falling == 0x04)
	{
		LED_Ctrl(0x40,ENABLE);
	}
	else if(Key_Trg_Falling == 0x08)
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
	
	if(key_State == 0 && Key_Trg_Falling == 0)	// 按键松开
	{
		key_pressed_time = 0;
		key_time = 0;
		LED_Ctrl(0xF0,DISABLE);
	}
}

//Main Body
int main(void)
{
//	uint8_t i;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	SysTick_Config(SystemCoreClock/1000);
	Delay_Ms(200);
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	LED_Init();
	KEY_Init();
	Buzzer_Init();
	Tim_Init();
	i2c_init();					//I2C总线初始化
	ADC_Channel18_Init();
	RTC_Configuration();
	Time_Adjust();
	//LCD_Display();
	
//	strcpy((char *)eeprom_dat_write.str,"Hello World!!!");
//	for(i = 0; i < strlen((char *)eeprom_dat_write.str); i++)
//	{
//		Write_eeprom(i,eeprom_dat_write.str[i]);
//		Delay_Ms(5);
//	}
//	for(i = 0; i < strlen((char *)eeprom_dat_write.str); i++)
//	{
//		eeprom_dat_read.str[i] = Read_eeprom(i);
//		Delay_Ms(5);
//	}
//	sprintf((char*)lcd_string,"%s      ",eeprom_dat_read.str);
//	LCD_DisplayStringLine(Line4,(unsigned char *)lcd_string);	
//	
//	eeprom_dat_write.f2 = 3.1415926535;
//	for(i = 0; i < sizeof(eeprom_dat_write.f2); i++)
//	{
//		Write_eeprom(i,eeprom_dat_write.str[i]);
//		Delay_Ms(5);
//	}
//	for(i = 0; i < sizeof(eeprom_dat_write.f2); i++)
//	{
//		eeprom_dat_read.str[i] = Read_eeprom(i);
//		Delay_Ms(5);
//	}
//	sprintf((char*)lcd_string,"%f        ",eeprom_dat_read.f2);
//	LCD_DisplayStringLine(Line5,(unsigned char *)lcd_string);	


	
	while(1)
	{
		
//		LCD_Display();
//		
//		if(key_read_flag)	// 每十毫秒扫描一次按键
//		{
//			key_read_flag = 0;
//			Key_Scan();
//		}	
		Time_Get();
		sprintf((char*)lcd_string,"Time: %0.2d:%0.2d:%0.2d", Tmp_HH, Tmp_MM, Tmp_SS);
		LCD_DisplayStringLine(Line4,(unsigned char *)lcd_string);
		
		if(led_flag)
		{
			led_flag = 0;
			LED_Toggle(0x01);
		}
		
		if(adc_flag)
		{
			adc_flag = 0;
			sprintf((char*)lcd_string,"%.2fV               ",Get_Adc_Value() * 3.3f / 4096);
			LCD_DisplayStringLine(Line5,(unsigned char *)lcd_string);
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
		
		if(++adc_cnt >= 50)
		{
			adc_cnt = 0;
			adc_flag = 1;
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

//void RTC_IRQHandler(void)
//{
//  if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
//  {
//		/* Clear the RTC Second interrupt */
//    RTC_ClearITPendingBit(RTC_IT_SEC);
//  }
//}
