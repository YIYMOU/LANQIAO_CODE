/*
	例程：利用共用体读写EEPROM
	作者：电子设计工坊
	重点： (1) 共用体：几个不同的变量共同占用一段内存的结构；
	      (2) 学会共用体的定义及EEPROM的读写方法
	
    Copyright (c) 2020 电子设计工坊 dianshe.taobao.com
	All rights reserved
*/
#include "stm32f10x.h"
#include "lcd.h"
#include "stdio.h"
#include "led.h"
#include "timer.h"
#include "buzzer.h"
#include "key.h"
#include "i2c.h"
#include "string.h"


u8 size_u8 = sizeof(u8);				//u8取值范围0~255
u8 size_s8 = sizeof(s8);				//s8取值范围-128~127			
u8 size_u16 = sizeof(u16);				//u16取值范围0~65535
u8 size_u32 = sizeof(u32);				//u32取值范围0~0xFFFF FFFF
u8 size_s16 = sizeof(s16);				//s16取值范围-32768~32767
u8 size_float = sizeof(float);
u8 size_double= sizeof(double);

u32 TimingDelay = 0;
void Delay_Ms(u32 nTime);

_Bool key_flag=0;

//利用共用体读写EEPROM
union eeprom_float
{
   float a;  
   u8 b[4];
}float_write,float_read;

union eeprom_s16
{
   s16 a;  
   u8 b[2];
}s16_write,s16_read;

union eeprom_u16
{
   u16 a;  
   u8 b[2];
}u16_write;

union eeprom_u32
{
   u32 a;  
   u8 b[4];
}u32_write;

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

uint8_t lcd_str[20];

//Main Body
int main(void)
{
	u8 i = 0;
	SysTick_Config(SystemCoreClock/1000);
	Delay_Ms(200);
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	LED_Init(); 				//LED初始化
	TIM4_Init();				//定时器4初始化，1ms中断
	Buzzer_Init();				//蜂鸣器初始化
	Key_Init();					//按键初始化
	i2c_init();					//I2C总线初始化
	
	/***************float小数的读写**************/
	float_write.a = 3.1415926;
	for(i=0;i<sizeof(float);i++)
	{
		Write_AT24c02(0x10+i,float_write.b[i]);
		Delay_Ms(5);
	}
	for(i=0;i<sizeof(float);i++)
	{
		float_read.b[i]=Read_AT24c02(0x10+i);
	}
	sprintf((char*)lcd_str,"float_read=%f",float_read.a);
	LCD_DisplayStringLine(Line0,lcd_str);
	
	/***************负数的读写*************/
	s16_write.a = -1234;
	for(i=0;i<sizeof(s16);i++)
	{
		Write_AT24c02(0x20+i,s16_write.b[i]);
		Delay_Ms(5);
	}
	for(i=0;i<sizeof(s16);i++)
	{
		s16_read.b[i]=Read_AT24c02(0x20+i);
	}
	sprintf((char*)lcd_str,"s16_read=%4d",s16_read.a);
	LCD_DisplayStringLine(Line2,lcd_str);
	
	eeprom_dat_write.f2 = 3.1415926535;
	for(i = 0; i < sizeof(eeprom_dat_write.f2); i++)
	{
		Write_AT24c02(i,eeprom_dat_write.str[i]);
		Delay_Ms(5);
	}
	for(i = 0; i < sizeof(eeprom_dat_write.f2); i++)
	{
		eeprom_dat_read.str[i] = Read_AT24c02(i);
		Delay_Ms(5);
	}
	sprintf((char*)lcd_str,"%f",eeprom_dat_read.f2);
	LCD_DisplayStringLine(Line4,(unsigned char *)lcd_str);	
	
	while(1)
	{
		
		if(key_flag)
		{
			key_flag=0;
			Key_Read();
			if(Trg==0x01)						//B1按下
			{
				
			}
			
			if(Trg==0x02)						//B2按下
			{
				
			}
			if(Trg==0x04)						//B3按下
			{
				
			}
			if(Trg==0x08)						//B4按下
			{
				
			}
			
			if(Cont==0x01)						//长按检测
			{
				
			}
			
			if(Trg==0x00 && Cont==0x00)			//松手检测
			{
				
			}
		}
	}
}


void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}

void TIM4_IRQHandler(void)
{
	static u16 key_count=0;		//定义为静态变量
	if (TIM_GetITStatus(TIM4,TIM_IT_Update) != RESET)
	{
		key_count++;
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		if(key_count==10)
		{
			key_count=0;
			key_flag=1;
		}
	}
}
