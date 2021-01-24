/*
	���̣�PWM����LED����
	���ߣ�������ƹ���	
	�ص㣺 ѧ������IOģ��PWM������LED�Ƶ�����
	
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
u8 led_duty=0;					//LEDռ�ձȣ���Χ0~10
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
	while(1)
	{
		if(key_flag)
		{
			key_flag=0;
			Key_Read();
			if(Trg==0x01)						//B1����
			{
				if(led_duty <10)
					led_duty++;
			}
			
			if(Trg==0x02)						//B2����
			{
				if(led_duty >0)
					led_duty--;
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

//Led���ȿ��ƺ��������ж������У�PWM���Ƶ����100Hz��ռ�ձȵ��ڷ�ΧΪ0~100%������10%
void Led_Light_PWM(u8 led_duty)
{
	static u16 led_pwm_count=0;
	led_pwm_count++;
	if(led_pwm_count == 11)		//����PWM�����ڣ�Ϊ10ms
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
		
		Led_Light_PWM(led_duty);
	}
}
