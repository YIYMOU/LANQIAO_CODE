/*
	���̣�������ʱ�������·��ͬƵ�ʣ���ͬռ�ձȵķ���
	���ߣ�������ƹ���	
	�ص㣺(1) �����̵ĵ�����ʱ���Ĳ�ͬͨ����Ƶ������ͬ�ģ�����ռ�ձȿɵ�������TIM2_CH2,TIM2_CH3�����Ƶ����һ�µģ�ռ�ձȲ�ͬ��
	     (2) ��ϤARR��CCR�Ĵ�������ؿ⺯����ʹ��
		 (3) ���ͬһƵ�ʣ���ͬռ�ձȵ�PWM��ֻ��Ҫʹ��TIM_OCMode_PWM1ģʽ���ο�\TIM\PWM_Output�ļ�
		 (4) ѧ������޸�PWM��Ƶ�ʺ�ռ�ձ�
		 
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
	LED_Init(); 				//LED��ʼ��
	TIM4_Init();				//��ʱ��4��ʼ����1ms�ж�
	Buzzer_Init();				//��������ʼ��
	Key_Init();					//������ʼ��
	i2c_init();					//I2C���߳�ʼ��
	ADC_Channel8_Init();		//ADC1ͨ��8��ʼ��
	RTC_Configuration();		//RTC��ʼ��
	TIM2_PWM_Init();			//TIM2 PWM��ʼ������·���Ƶ����ͬ
	TIM3_PWM_Init();			//TIM3 PWM��ʼ������·���Ƶ����ͬ
	while(1)
	{
		TIM_SetAutoreload(TIM2,1000 - 1);	//����TIM2������ֵ����PWMƵ��Ϊ1KHz������TIM2_CH2 TIM2_CH3��Ƶ�ʶ�����ͬ��
		TIM_SetCompare2(TIM2,500);		//����PA1(TIM2_CH2)��ռ�ձȣ�50%
		TIM_SetCompare3(TIM2,100);		//����PA2(TIM2_CH3)��ռ�ձȣ�10%

		TIM_SetAutoreload(TIM3,500 - 1);	//����TIM3������ֵ����PWMƵ��Ϊ2KHz
		TIM_SetCompare1(TIM3,250);		//����PA6(TIM3_CH1)��ռ�ձȣ�50%
		TIM_SetCompare2(TIM3,375);		//����PA7(TIM3_CH2)��ռ�ձȣ�75%
		
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
	static u16 key_count=0;		//����Ϊ��̬����
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
//	static u16 led_cnt=0;		//����Ϊ��̬����
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
