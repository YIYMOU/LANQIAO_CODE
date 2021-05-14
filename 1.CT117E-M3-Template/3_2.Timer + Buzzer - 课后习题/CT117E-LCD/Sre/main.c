/*
	例程：定时器中断 + 蜂鸣器课后习题
	作者：电子设计工坊
	功能：实现指定蜂鸣器响0.5s
	
	Copyright (c) 2020 电子设计工坊 dianshe.taobao.com
	All rights reserved
*/
#include "stm32f10x.h"
#include "lcd.h"
#include "stdio.h"
#include "led.h"
#include "timer.h"
#include "buzzer.h"

u32 TimingDelay = 0;

void Delay_Ms(u32 nTime);

_Bool buzzer_flag = 1; 		//把初值置为1，比赛中往往通过按键或者其他状态判断置位1
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
	while(1)
	{
		if(buzzer_flag)
		{
			BEEP_ON();
		}
		else
		{
			BEEP_OFF();
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
	static u16 buzzer_count=0;		//定义为静态变量
	if (TIM_GetITStatus(TIM4,TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);	
		if(buzzer_flag)
		{
			buzzer_count++;
			if(buzzer_count==500)
			{
				buzzer_count = 0;
				buzzer_flag =0;
			}
		}
	}
}
