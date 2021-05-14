/*
	例程：按键之短按、长按
	作者：电子设计工坊
	功能： 实现长按按键，使得屏幕上数值每500ms增加10；
	重点： (1) 通过key_time变量结合Cont来判断是否为长按操作
           (2) 单击操作Trg是单次触发的，但是Cont是多次触发的，每500ms触发一次
           
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

_Bool key_flag=0;

u16 lcd_number = 0;
u8 lcd_buf[30];
//Main Body
int main(void)
{
	u16 key1_time=0;			//用来统计B1按键长按的时间
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
		
		
		// 如果要显示一行的时候，最好先清除
		sprintf((char*)lcd_buf,"Number = %d     ",lcd_number);
		LCD_DisplayStringLine(Line4,lcd_buf);
		sprintf((char*)lcd_buf,"Trg = %d",Trg);
		LCD_DisplayStringLine(Line5,lcd_buf);
		sprintf((char*)lcd_buf,"Cont = 0X%02o",Cont);
		LCD_DisplayStringLine(Line6,lcd_buf);
		
		if(key_flag)							//10ms调用一次
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
			
			if(Cont==0x01 || Cont==0x02 || Cont==0x04 || Cont==0x08)						//长按检测
			{
				LED_Control(0x01,SET);
				key1_time++;
				if(key1_time>=50)				//超过500ms
				{
					key1_time=0;
					lcd_number+=10;
				}
			}
			
			if(Trg==0x00 && Cont==0x00)			//松手检测
			{
				LED_Control(0x01,RESET);
				key1_time = 0;
				lcd_number = 0;
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
		if(key_count == 10)			// 每十毫秒检测一下按键状态
		{
			key_count=0; 
			key_flag=1;
		}
	}
}
