/*
	例程：EEPROM读写不同类型数据
	作者：电子设计工坊
	重点： EEPROM一个地址，存入一个字节的数据，所以针对不同类型的数据，需要分离成单个字节的数据。
				
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

u32 TimingDelay = 0;
void Delay_Ms(u32 nTime);

_Bool key_flag=0;

uint16_t u16_write = 1234;
uint16_t u16_read = 0;

float float_write = 3.1415;
float float_read = 0;

int8_t minus_write = -23;
int8_t minus_read = 0;

int16_t minus_s16_write = -1234;
int16_t minus_s16_read = -1234;

uint8_t str_write[11]={"hello world"};
uint8_t str_read[11];
uint8_t lcd_str[20];

//Main Body
int main(void)
{
	uint8_t i = 0;
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
	
	/***************写入u16类型的数据**************/
	Write_AT24c02(0x00,u16_write/256);
	Delay_Ms(5);
	Write_AT24c02(0x01,u16_write%256);
	Delay_Ms(5);
	u16_read = Read_AT24c02(0x00)*256+Read_AT24c02(0x01);
	
	/***************写入float类型的数据，保留小数点后四位**************/
	Write_AT24c02(0x03,(u16)(float_write*1000)/256);
	Delay_Ms(5);
	Write_AT24c02(0x04,(u16)(float_write*1000)%256);
	Delay_Ms(5);
	float_read = (Read_AT24c02(0x03)*256+Read_AT24c02(0x04))/1000.0f;
	
	/***************写入负数，在数字电路中的存储形式是补码**************/
	/***************例如-23，其最高位为符号位1（表示负数），23的二进制10111，反码1110 1000，补码1110 1001 -> 0xE9**************/
	
	//	if(minus_write<0)
	//	{
	//		Write_AT24c02(0x06,'-');
	//		Delay_Ms(5);
	//		Write_AT24c02(0x07,-minus_write);
	//		Delay_Ms(5);
	//	}
	//	else
	//	{
	//		Write_AT24c02(0x06,'+');
	//	}
	//	if(Read_AT24c02(0x06)=='-')
	//	{
	//		minus_read = 0 - Read_AT24c02(0x07);
	//	}
	
	Write_AT24c02(0x06,minus_write);
	minus_read = Read_AT24c02(0x06);
	
	
	Write_AT24c02(0x08,minus_s16_write>>8);						//取出高8位
	Delay_Ms(5);
	Write_AT24c02(0x09,minus_s16_write&0x00ff);					//取出低8位
	Delay_Ms(5);
	minus_s16_read = (Read_AT24c02(0x08)<<8)+Read_AT24c02(0x09);
		
	/***************写入字符串，字符以ASCII码存储**************/
	for(i=0;i<sizeof(str_write);i++)
	{
		Write_AT24c02(0x10+i,str_write[i]);
		Delay_Ms(5);
	}
	for(i=0;i<sizeof(str_write);i++)
	{
		str_read[i]=Read_AT24c02(0x10+i);
	}
	sprintf((char*)lcd_str,"u16_read=%d",u16_read);
	LCD_DisplayStringLine(Line0, lcd_str);
	sprintf((char*)lcd_str,"float_read=%.6f",float_read);
	LCD_DisplayStringLine(Line2, lcd_str);
	sprintf((char*)lcd_str,"minus_read=%d",minus_read);
	LCD_DisplayStringLine(Line4, lcd_str);
	sprintf((char*)lcd_str,"minus_s16_read=%d",minus_s16_read);
	LCD_DisplayStringLine(Line6, lcd_str);
	sprintf((char*)lcd_str,"%-20.11s",str_read);
	LCD_DisplayStringLine(Line8, lcd_str);
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
