/*
	���̣�ADC�������
	���ߣ�������ƹ���
	�ص㣺 (1) ���ö�ʱ������ʵ��ADC�ļ������
	      (2) ѧ�Ὣ������0~4095����ֵ��ת����0~3.3V
		  (3) ѧ�Ὣ������0~4095����ֵ��ת����N����λ��������״̬����
	
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

u32 TimingDelay = 0;
void Delay_Ms(u32 nTime);

_Bool key_flag=0,adc_flag=0;

u16 adc_val;
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
	while(1)
	{
		if(adc_flag)	// 100ms��ȡһ��adc��ֵ
		{
			adc_flag=0;
			adc_val=ADC_GetConversionValue(ADC1);
		}
		
		sprintf((char*)lcd_buf,"ADC val = %4d",adc_val);
		LCD_DisplayStringLine(Line3,lcd_buf);
		
		sprintf((char*)lcd_buf,"ADC volt = %.2f V",(adc_val/4095.0f)*3.3f);
		LCD_DisplayStringLine(Line4,lcd_buf);
		
		sprintf((char*)lcd_buf,"ADC  = %4d",(adc_val/(4096/4)));	//��Ϊ4��,(0~4095/1024)= 0~3
		LCD_DisplayStringLine(Line5,lcd_buf);
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
	static u16 key_count=0,adc_count=0;		//����Ϊ��̬����
	if (TIM_GetITStatus(TIM4,TIM_IT_Update) != RESET)
	{
		key_count++;adc_count++;
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		if(key_count==10)
		{
			key_count=0;
			key_flag=1;
		}
		if(adc_count==100)
		{
			adc_count=0;
			adc_flag=1;
		}
	}
}
