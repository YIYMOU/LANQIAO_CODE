/*
	���̣����ù������дEEPROM
	���ߣ�������ƹ���
	�ص㣺 (1) �����壺������ͬ�ı�����ͬռ��һ���ڴ�Ľṹ��
	      (2) ѧ�Ṳ����Ķ��弰EEPROM�Ķ�д����
	
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
#include "string.h"


u8 size_u8 = sizeof(u8);				//u8ȡֵ��Χ0~255
u8 size_s8 = sizeof(s8);				//s8ȡֵ��Χ-128~127			
u8 size_u16 = sizeof(u16);				//u16ȡֵ��Χ0~65535
u8 size_u32 = sizeof(u32);				//u32ȡֵ��Χ0~0xFFFF FFFF
u8 size_s16 = sizeof(s16);				//s16ȡֵ��Χ-32768~32767
u8 size_float = sizeof(float);
u8 size_double= sizeof(double);

u32 TimingDelay = 0;
void Delay_Ms(u32 nTime);

_Bool key_flag=0;

//���ù������дEEPROM
union eeprom_float
{
   float a;  
   u8 b[4];
}float_write,float_read;

union eeprom_s16
{
   s16 a;  
   u8 b[2];
}s16_write,s16_read;

union eeprom_u16
{
   u16 a;  
   u8 b[2];
}u16_write;

union eeprom_u32
{
   u32 a;  
   u8 b[4];
}u32_write;

union eeprom_dat
{
	uint8_t t1;
	uint16_t t2;
	uint32_t t3;
	int16_t t4;
	float f1;
	double f2;
	unsigned char str[20];
}eeprom_dat_write,eeprom_dat_read;

uint8_t lcd_str[20];

//Main Body
int main(void)
{
	u8 i = 0;
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
	
	/***************floatС���Ķ�д**************/
	float_write.a = 3.1415926;
	for(i=0;i<sizeof(float);i++)
	{
		Write_AT24c02(0x10+i,float_write.b[i]);
		Delay_Ms(5);
	}
	for(i=0;i<sizeof(float);i++)
	{
		float_read.b[i]=Read_AT24c02(0x10+i);
	}
	sprintf((char*)lcd_str,"float_read=%f",float_read.a);
	LCD_DisplayStringLine(Line0,lcd_str);
	
	/***************�����Ķ�д*************/
	s16_write.a = -1234;
	for(i=0;i<sizeof(s16);i++)
	{
		Write_AT24c02(0x20+i,s16_write.b[i]);
		Delay_Ms(5);
	}
	for(i=0;i<sizeof(s16);i++)
	{
		s16_read.b[i]=Read_AT24c02(0x20+i);
	}
	sprintf((char*)lcd_str,"s16_read=%4d",s16_read.a);
	LCD_DisplayStringLine(Line2,lcd_str);
	
	eeprom_dat_write.f2 = 3.1415926535;
	for(i = 0; i < sizeof(eeprom_dat_write.f2); i++)
	{
		Write_AT24c02(i,eeprom_dat_write.str[i]);
		Delay_Ms(5);
	}
	for(i = 0; i < sizeof(eeprom_dat_write.f2); i++)
	{
		eeprom_dat_read.str[i] = Read_AT24c02(i);
		Delay_Ms(5);
	}
	sprintf((char*)lcd_str,"%f",eeprom_dat_read.f2);
	LCD_DisplayStringLine(Line4,(unsigned char *)lcd_str);	
	
	while(1)
	{
		
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
