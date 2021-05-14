/*
	例程：单个定时器捕获多路方波的频率、占空比
	作者：电子设计工坊	
	重点：(1) 编程思路一定要清晰，根据PPT讲的思路来编程代码；
		 (2) 设置capture_flag，防止两个通道的CNT计数器相关干扰；本例程是每个通道扫描100ms，可根据捕获频率不同更改。
		 
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
#include "pwm_capture.h"

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
	TIM2_PWM_OCToggle();		//TIM2的多路不同频率PWM的初始化
//	TIM3_PWM_OCToggle();		//TIM3的多路不同频率PWM的初始化
	TIM3_Input_Mode_Init();
	while(1)
	{
		TIM2_CCR2_Val = 200;			//TIMCLK = 1MHz，设置TIM2_CH2频率为5KHz
		TIM2_CH2_duty = 0.2;			//占空比为20%
		
		TIM2_CCR3_Val = 500;			//TIMCLK = 1MHz，设置TIM2_CH2频率为2KHz
		TIM2_CH3_duty = 0.8;			//占空比为80%

		sprintf((char*)lcd_buf,"T3CH1_duty:%d      ",TIM3_CH1_Duty);
		LCD_DisplayStringLine(Line2, lcd_buf);
	
		sprintf((char*)lcd_buf,"T3CH1_Freq= %d     ",TIM3_CH1_Freq);
		LCD_DisplayStringLine(Line3,lcd_buf);
				
		
		sprintf((char*)lcd_buf,"T3CH2_duty:%d      ",TIM3_CH2_Duty);
		LCD_DisplayStringLine(Line5, lcd_buf);
		
		sprintf((char*)lcd_buf,"T3CH2_Freq= %d     ",TIM3_CH2_Freq);
		LCD_DisplayStringLine(Line6,lcd_buf);
		
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
	static u16 key_count=0,pwm_capture_count=0;		//定义为静态变量
	if (TIM_GetITStatus(TIM4,TIM_IT_Update) != RESET)
	{
		key_count++;pwm_capture_count++;
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		if(key_count==10)
		{
			key_count=0;
			key_flag=1;
		}
		if(pwm_capture_count==100)
		{
			pwm_capture_count=0;
			capture_flag = capture_flag % 2 + 1;
			TIM3_CH1_CaptureNumber=0;
			TIM3_CH2_CaptureNumber=0;
		}
	}
}
