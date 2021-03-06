/*
	例程：单个定时器输出多路可调频率、占空比的方波
	作者：电子设计工坊	
	重点：(1)  设置一个标志位TIMx_CHx_flag，在高、低电平不同时，设置不同的CCRx值，
			 从而设置不同的高、低电平时间，达到设置占空比的目的。
		 (2) 此设置方法，定时器的主频是：TIMCLK = SystemCoreClock；
	
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
#include "pwm.h"

u32 TimingDelay = 0;
void Delay_Ms(u32 nTime);

_Bool key_flag=0;
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
	TIM2_PWM_OCToggle();		//TIM2的多路不同频率PWM的初始化
	TIM3_PWM_OCToggle();		//TIM3的多路不同频率PWM的初始化
	while(1)
	{

		TIM2_CH2_duty=0.3;		//设置TIM2_CH2的占空比为30%
		TIM2_CH3_duty=0.7;		//设置TIM2_CH3的占空比为70%

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
