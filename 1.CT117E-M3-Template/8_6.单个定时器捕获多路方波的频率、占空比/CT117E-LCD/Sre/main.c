/*
	���̣�������ʱ�������·������Ƶ�ʡ�ռ�ձ�
	���ߣ�������ƹ���	
	�ص㣺(1) ���˼·һ��Ҫ����������PPT����˼·����̴��룻
		 (2) ����capture_flag����ֹ����ͨ����CNT��������ظ��ţ���������ÿ��ͨ��ɨ��100ms���ɸ��ݲ���Ƶ�ʲ�ͬ���ġ�
		 
    Copyright (c) 2020 ������ƹ��� dianshe.taobao.com
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
	LED_Init(); 				//LED��ʼ��
	TIM4_Init();				//��ʱ��4��ʼ����1ms�ж�
	Buzzer_Init();				//��������ʼ��
	Key_Init();					//������ʼ��
	i2c_init();					//I2C���߳�ʼ��
	ADC_Channel8_Init();		//ADC1ͨ��8��ʼ��
	RTC_Configuration();		//RTC��ʼ��
	TIM2_PWM_OCToggle();		//TIM2�Ķ�·��ͬƵ��PWM�ĳ�ʼ��
//	TIM3_PWM_OCToggle();		//TIM3�Ķ�·��ͬƵ��PWM�ĳ�ʼ��
	TIM3_Input_Mode_Init();
	while(1)
	{
		TIM2_CCR2_Val = 200;			//TIMCLK = 1MHz������TIM2_CH2Ƶ��Ϊ5KHz
		TIM2_CH2_duty = 0.2;			//ռ�ձ�Ϊ20%
		
		TIM2_CCR3_Val = 500;			//TIMCLK = 1MHz������TIM2_CH2Ƶ��Ϊ2KHz
		TIM2_CH3_duty = 0.8;			//ռ�ձ�Ϊ80%

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
			if(Trg==0x01)						//B1����
			{
				
			}
			
			if(Trg==0x02)						//B2����
			{
				
			}
			if(Trg==0x04)						//B3����
			{
				
			}
			if(Trg==0x08)						//B4����
			{
				
			}
			
			if(Cont==0x01)						//�������
			{
				
			}
			
			if(Trg==0x00 && Cont==0x00)			//���ּ��
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
	static u16 key_count=0,pwm_capture_count=0;		//����Ϊ��̬����
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
