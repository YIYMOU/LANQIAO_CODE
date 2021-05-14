/*
	例程：EEPROM写入间隔及初值设置
	作者：电子设计工坊
	重点： (1) 自己完成读写EEPROM的函数，I2C驱动比赛提供
          (2) 两次数据写入间隔为：5ms
		  (3) 赛题指定初值时：在程序初始化时预先写入数值，再屏蔽掉该函数
		  (4) 不要在while(1)频繁读写EEPROM，AT24C02写的寿命为100w次。如果5ms写入一次，500分钟就把EEPROM写坏了。
		  
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

u32 TimingDelay = 0;
void Delay_Ms(u32 nTime);

_Bool key_flag=0;

u8 write_data_h = 88;
u8 write_data_l = 99;
u8 read_data_h = 0;
u8 read_data_l = 0;

u8 key_data = 0;
u8 lcd_buf[30];
//Main Body
int main(void)
{
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
	Write_AT24c02(0x00,write_data_h);
	Delay_Ms(5);
	Write_AT24c02(0x01,write_data_l);
	Delay_Ms(5);				// EEPROM两次数据写入的间隔最好不小于5ms
	read_data_h = Read_AT24c02(0x00);
	read_data_l = Read_AT24c02(0x01);
	
	//由于全新EEPROM默认数据为0xff，所以需要预先写入比赛指定的初值
	//例如要求key_data默认初值是100，但是手动调节太慢，可以采用在程序初始化时预先写入数值，再屏蔽掉该函数
//	Write_AT24c02(0xf0,100);
//	Delay_Ms(5);
	key_data = Read_AT24c02(0xf0);
	while(1)
	{
		sprintf((char*)lcd_buf,"Key Data = %3d",key_data);
		LCD_DisplayStringLine(Line4,lcd_buf);
		
		if(key_flag)
		{
			key_flag=0;
			Key_Read();
			if(Trg==0x01)						//B1按下
			{
				key_data++;
				Write_AT24c02(0xf0,key_data);
			}
			
			if(Trg==0x02)						//B2按下
			{
				key_data--;
				Write_AT24c02(0xf0,key_data);
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
