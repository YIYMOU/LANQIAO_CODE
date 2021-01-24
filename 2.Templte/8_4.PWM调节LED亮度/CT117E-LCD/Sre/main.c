/*
	例程：PWM调节LED亮度
	作者：电子设计工坊	
	重点： 学会利用IO模拟PWM来调节LED灯的亮度
	
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
u8 led_duty=0;					//LED占空比，范围0~10
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
	while(1)
	{
		if(key_flag)
		{
			key_flag=0;
			Key_Read();
			if(Trg==0x01)						//B1按下
			{
				if(led_duty <10)
					led_duty++;
			}
			
			if(Trg==0x02)						//B2按下
			{
				if(led_duty >0)
					led_duty--;
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

//Led亮度控制函数，在中断里运行，PWM输出频率是100Hz，占空比调节范围为0~100%，步进10%
void Led_Light_PWM(u8 led_duty)
{
	static u16 led_pwm_count=0;
	led_pwm_count++;
	if(led_pwm_count == 11)		//设置PWM的周期，为10ms
			led_pwm_count=1;
		
	if(led_pwm_count<=led_duty)
	{
		LED_Control(0x01,1);
	}
	else if(led_pwm_count<=10)
	{
		LED_Control(0x01,0);
	}
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
		
		Led_Light_PWM(led_duty);
	}
}
