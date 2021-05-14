/*
	���̣����ֶ̰��볤��
	���ߣ�������ƹ���
	���ܣ� ʵ�ֶ̰��볤�����������֣�����500ms�İ���Ϊ������С��500ms�İ���Ϊ�̰�
	�ص㣺 (1) �������ּ���У��ж�key1_time�Ƿ�Ϊ1~500ms֮��
          (2) ����̰��볤������󣬶̰���������ʱ��Ϊ ����ʱ
		  
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

u32 TimingDelay = 0;
void Delay_Ms(u32 nTime);

_Bool key_flag=0,key1_release=1;

u16 lcd_number = 0;
u8 lcd_buf[30];
//Main Body
int main(void)
{
	u16 key1_time=0;
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
	while(1)
	{		
		sprintf(lcd_buf,"Number = %d",lcd_number);
		LCD_DisplayStringLine(Line4,lcd_buf);
		
		if(key_flag)
		{
			key_flag=0;
			Key_Read();
			if(Trg==0x01)						//B1����
			{
				
			}
			
			if(Trg==0x02)						//B2����
			{
				lcd_number+=2;
			}
			if(Trg==0x04)						//B3����
			{
				lcd_number+=3;
			}
			if(Trg==0x08)						//B4����
			{
				lcd_number+=4;
			}
			
			if(Cont==0x01)						//�������
			{
				key1_time++;
				if(key1_time>=50 && key1_release==1)			//����500ms����key1Ϊ����״̬
				{
					key1_time = 0;
					key1_release = 0;							//��key1_release��ʵ�ֳ��������ĵ��δ���������key1Ϊ����״̬
					lcd_number+=10;
				}
			}
			
			if(Trg==0x00 && Cont==0x00)			//���ּ��
			{
				if(key1_time>0 && key1_time<50)	//B1�̰�
				{
					lcd_number++;
				}
				key1_time = 0;
				key1_release = 1;
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
