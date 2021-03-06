/*
	例程：长按按键 之 单次触发
	作者：电子设计工坊
	功能： 实现长按按键的单次触发，每次长按使屏幕数字增加10
	重点： (1) 用key1_release来实现长按操作的单次触发，在松手时置1，长按操作时置0
           (2) 每次长按前都会触发一次短按操作
           
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

u32 TimingDelay = 0;

void Delay_Ms(u32 nTime);

_Bool key_flag=0,key1_release=1;

u16 lcd_number = 0;
u8 lcd_buf[30];
//Main Body
int main(void)
{
	u16 key1_time=0;
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
	while(1)
	{		
		sprintf(lcd_buf,"Number = %d",lcd_number);
		LCD_DisplayStringLine(Line4,lcd_buf);
		
		if(key_flag)
		{
			key_flag=0;
			Key_Read();
			if(Trg==0x01)						//B1按下
			{
				lcd_number++;
			}
			if(Trg==0x02)						//B2按下
			{
				lcd_number+=2;
			}
			if(Trg==0x04)						//B3按下
			{
				lcd_number+=3;
			}
			if(Trg==0x08)						//B4按下
			{
				lcd_number+=4;
			}
			
			if(Cont==0x01)						//B1长按检测
			{
				key1_time++;
				if(key1_time>=50 && key1_release==1)			//超过500ms，且key1为弹起状态
				{
					key1_time = 0;
					key1_release = 0;							//用key1_release来实现长按操作的单次触发
					lcd_number+=10;
				}
			}
			
			if(Trg==0x00 && Cont==0x00)			//松手检测
			{
				key1_time=0;
				key1_release = 1;
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
