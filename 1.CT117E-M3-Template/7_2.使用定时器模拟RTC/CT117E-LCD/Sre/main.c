/*
	例程：使用定时器模拟RTC
	作者：电子设计工坊	
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
#include "adc.h"
#include "rtc.h"

//定时器模拟RTC变量
u8 shi,fen,miao;

u32 TimingDelay = 0;
void Delay_Ms(u32 nTime);

_Bool key_flag=0;

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
	ADC_Channel8_Init();		//ADC1通道8初始化
	RTC_Configuration();		//RTC初始化
	
	shi=23;fen=59;miao=55;
	while(1)
	{
		sprintf(lcd_buf,"Time: %0.2d:%0.2d:%0.2d", shi, fen, miao);
		LCD_DisplayStringLine(Line3,lcd_buf);
		
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
	static u16 key_count=0,one_sec_count=0;		//定义为静态变量
	if (TIM_GetITStatus(TIM4,TIM_IT_Update) != RESET)
	{
		key_count++;one_sec_count++;
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		if(key_count==10)
		{
			key_count=0;
			key_flag=1;
		}
		
		if(one_sec_count==1000)
		{
			one_sec_count=0;
			miao++;
			if(miao==60)
			{   
				miao=0;
				fen++;
			}
			if(fen==60)
			{
				fen=0;
				shi++;
			}
			if(shi==24)
			{
				shi=0;
			}
		}
	}
}
