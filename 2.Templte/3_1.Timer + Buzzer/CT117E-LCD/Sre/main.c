/*
	���̣���ʱ���ж� + ������
	���ߣ�������ƹ���
	�ص㣺	��1�� ʹ��Timer4����1ms���жϣ�
			  ��2�� �ɲο�STM32F10x_StdPeriph_Examples\TIM\TimeBase
			
	Copyright (c) 2020 ������ƹ��� dianshe.taobao.com
	All rights reserved
*/
#include "stm32f10x.h"
#include "lcd.h"
#include "stdio.h"
#include "led.h"
#include "timer.h"
#include "buzzer.h"

u32 TimingDelay = 0;

void Delay_Ms(u32 nTime);

_Bool buzzer_flag=0;
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
	while(1)
	{		
		if(buzzer_flag)			//500msִ��һ�Σ��Ҳ�ռ��CPUʱ��
		{
			buzzer_flag=0;
			GPIO_WriteBit(GPIOB, GPIO_Pin_4, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_4)));		
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
	static u16 buzzer_count=0;		//����Ϊ��̬����
	if (TIM_GetITStatus(TIM4,TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		buzzer_count++;
		if(buzzer_count==500)
		{
			buzzer_count = 0;
			buzzer_flag =1;
		}
	}
}
