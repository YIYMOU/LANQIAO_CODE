/*
	���̣�EEPROMд��������ֵ����
	���ߣ�������ƹ���
	�ص㣺 (1) �Լ���ɶ�дEEPROM�ĺ�����I2C���������ṩ
          (2) ��������д����Ϊ��5ms
		  (3) ����ָ����ֵʱ���ڳ����ʼ��ʱԤ��д����ֵ�������ε��ú���
		  (4) ��Ҫ��while(1)Ƶ����дEEPROM��AT24C02д������Ϊ100w�Ρ����5msд��һ�Σ�500���ӾͰ�EEPROMд���ˡ�
		  
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

u32 TimingDelay = 0;
void Delay_Ms(u32 nTime);

_Bool key_flag=0;

u8 write_data_h = 88;
u8 write_data_l = 99;
u8 read_data_h = 0;
u8 read_data_l = 0;

u8 key_data = 0;
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
	Write_AT24c02(0x00,write_data_h);
	Delay_Ms(5);
	Write_AT24c02(0x01,write_data_l);
	Delay_Ms(5);				// EEPROM��������д��ļ����ò�С��5ms
	read_data_h = Read_AT24c02(0x00);
	read_data_l = Read_AT24c02(0x01);
	
	//����ȫ��EEPROMĬ������Ϊ0xff��������ҪԤ��д�����ָ���ĳ�ֵ
	//����Ҫ��key_dataĬ�ϳ�ֵ��100�������ֶ�����̫�������Բ����ڳ����ʼ��ʱԤ��д����ֵ�������ε��ú���
//	Write_AT24c02(0xf0,100);
//	Delay_Ms(5);
	key_data = Read_AT24c02(0xf0);
	while(1)
	{
		sprintf((char*)lcd_buf,"Key Data = %3d",key_data);
		LCD_DisplayStringLine(Line4,lcd_buf);
		
		if(key_flag)
		{
			key_flag=0;
			Key_Read();
			if(Trg==0x01)						//B1����
			{
				key_data++;
				Write_AT24c02(0xf0,key_data);
			}
			
			if(Trg==0x02)						//B2����
			{
				key_data--;
				Write_AT24c02(0xf0,key_data);
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
