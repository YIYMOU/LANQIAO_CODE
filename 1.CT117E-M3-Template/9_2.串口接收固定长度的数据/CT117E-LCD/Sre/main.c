/*
	例程：串口接收固定长度的数据
	作者：电子设计工坊	
	重点：如何实现加入闲时判断，清空串口数据缓存
	
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
#include "uart.h"
#include "string.h"

u32 TimingDelay = 0;
void Delay_Ms(u32 nTime);

_Bool key_flag=0;

u8 lcd_buf[30];


#define PWM_ENABLE 	1
#define UART_ENABLE 2
void PWM_UART_Enable(u8 flag)
{
	if(flag==PWM_ENABLE)
	{
		USART_Cmd(USART2, DISABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	
		TIM_Cmd(TIM2, ENABLE);
	}
	
	if(flag==UART_ENABLE)
	{
	    TIM_Cmd(TIM2, DISABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);		
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		USART_Cmd(USART2, ENABLE);
	}
}

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
//	TIM2_PWM_OCToggle();		//TIM2的多路不同频率PWM的初始化
//	TIM3_PWM_OCToggle();		//TIM3的多路不同频率PWM的初始化
//	TIM3_Input_Mode_Init();
	USART2_Init();
	while(1)
	{
		if(rx_flag)
		{
			rx_flag=0;
			LCD_DisplayStringLine(Line6,rx_buf);
		}
		
		if(key_flag)
		{
			key_flag=0;
			Key_Read();
			if(Trg==0x01)						//B1按下
			{
				PWM_UART_Enable(PWM_ENABLE);
			}
			
			if(Trg==0x02)						//B2按下
			{
				PWM_UART_Enable(UART_ENABLE);
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
	static u16 key_count=0;			//定义为静态变量
	if (TIM_GetITStatus(TIM4,TIM_IT_Update) != RESET)
	{
		key_count++;
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		if(key_count==10)
		{
			key_count=0;
			key_flag=1;
		}
		
		rx_ideltime++; 
		if(rx_ideltime>=50)
		{
			rx_ideltime=0;
			rx_count=0;
			memset(rx_buf,0,3);
		}
	}
}
