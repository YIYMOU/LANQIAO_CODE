/*
	例程：单个定时器输出多路相同频率，不同占空比的方波
	作者：电子设计工坊	
	重点：(1) 本例程的单个定时器的不同通道的频率是相同的，但是占空比可调。例如TIM2_CH2,TIM2_CH3的输出频率是一致的，占空比不同。
	     (2) 熟悉ARR、CCR寄存器及相关库函数的使用
		 (3) 输出同一频率，不同占空比的PWM，只需要使用TIM_OCMode_PWM1模式，参考\TIM\PWM_Output文件
		 (4) 学会如何修改PWM的频率和占空比
		 
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
_Bool led1_flag=0;

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
	TIM2_PWM_Init();			//TIM2 PWM初始化，多路输出频率相同
	TIM3_PWM_Init();			//TIM3 PWM初始化，多路输出频率相同
	while(1)
	{
		TIM_SetAutoreload(TIM2,1000 - 1);	//设置TIM2的重载值，即PWM频率为1KHz，但是TIM2_CH2 TIM2_CH3的频率都是相同的
		TIM_SetCompare2(TIM2,500);		//设置PA1(TIM2_CH2)的占空比，50%
		TIM_SetCompare3(TIM2,100);		//设置PA2(TIM2_CH3)的占空比，10%

		TIM_SetAutoreload(TIM3,500 - 1);	//设置TIM3的重载值，即PWM频率为2KHz
		TIM_SetCompare1(TIM3,250);		//设置PA6(TIM3_CH1)的占空比，50%
		TIM_SetCompare2(TIM3,375);		//设置PA7(TIM3_CH2)的占空比，75%
		
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

//void TIM2_IRQHandler(void)
//{
//	static u16 led_cnt=0;		//定义为静态变量
//	if (TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET)
//	{
//		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
//		if(++led_cnt==500)
//		{
//			led_cnt=0;
//			LED_Control(0x01,led1_flag);
//			led1_flag = !led1_flag;
//		}
//	}
//}
