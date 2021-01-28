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

//���ù������дEEPROM
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
	
	if(key1PressNumber == 1)	// ��⵽һ�ΰ����������أ���ʼ��ʱ
	{
		key1Time += 10;			// ���ڰ���������10msɨ��һ�Σ����������10ms
		if(key1Time >= 150)	// �������1500ms����ʾ�����ǵ�������ʼ��˫�����
		{
			// begin
			// ��������Ӱ��������Ĵ���
			LCD_DisplayStringLine(Line8,(unsigned char *)"SINGLE              ");
			// end
			key1Time = 0;
			key1PressNumber = 0;
		}
	}
	
	if(Key_Trg_Rising == 0x01)	// �����������أ��������ɿ�
	{
		if(key1PressNumber == 0)	// ��ʾ�����ǵ�һ�ΰ�����������
		{
			key1PressNumber = 1;		// �������
		}
	}
	
	if(Key_Trg_Falling == 0x01)	// ��⵽�������½���
	{
		LED_Ctrl(0x10,ENABLE);		
		if(key1PressNumber == 1)	// ����Ѿ�������һ�ΰ����������أ��������ڻ�û�г�ʱ����ô�����һ��˫���Ķ���
		{
			// begin
			// �����������˫����Ҫ����ĳ���
			// end
			LCD_DisplayStringLine(Line8,(unsigned char *)"DOUBLE              ");	
			key1Time = 0;
			key1PressNumber = 0;		// ��ʼ������˫����صı���
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
			key_time = 0xffff;	// ��ֹ�ظ�����γ������ɿ�������ʱ�򣬽�key_time����			
		}
	}
	
	if(key_State == 0 && Key_Trg_Falling == 0)	// �����ɿ�
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
	i2c_init();					//I2C���߳�ʼ��
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
//		if(key_read_flag)	// ÿʮ����ɨ��һ�ΰ���
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
