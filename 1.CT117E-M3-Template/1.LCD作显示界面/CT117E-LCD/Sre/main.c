/*
	���̣�LCD����ʾ����
	���ߣ�������ƹ���
	�ص㣺	��1�� �����ṩlcd.c lcd.h��
			  ��2�� LCD��ʾ��ֵ������sprintf���ַ�����ʽ���������<stdio.h>
			  ��3�� Ҫ���ⳤ���ݶԶ����ݵĸ��ǣ�Ӧ�����ݺ���ӿո���ø�ʽ�����
			
	Copyright (c) 2020 ������ƹ��� dianshe.taobao.com
	All rights reserved
*/
#include "stm32f10x.h"
#include "lcd.h"
#include "stdio.h" 		//����sprintf��Ҫ������ͷ�ļ�

u32 TimingDelay = 0;

void Delay_Ms(u32 nTime);

void Display(void)
{
	u8 display_buf[20];
	
	u8 string[20];//��ʾ�ַ���
	/*
	//1
	LCD_SetTextColor(Red);//���ú�ɫ����
	LCD_DisplayStringLine(Line2, string);//�ڵ�������ʾ�ַ�������
	LCD_ClearLine(Line2);//�������������
	LCD_Clear(Blue);//��ɫˢ��LCDҳ��
	//2
	LCD_SetBackColor(Black);//����LCD���ı���ɫΪ��ɫ
	//3
	LCD_DisplayChar(Line2,319-16*2,'A');//�ڵڶ��д����ҵ�2*16�д���ʼ��ʾ�ַ�A
	*/
	
	//�����⡿�����ݶԶ����ݸ�������
	sprintf((char*) display_buf,"%dHz",5000);
	LCD_DisplayStringLine(Line0,display_buf);
	sprintf((char*) display_buf,"%dHz",10);
	LCD_DisplayStringLine(Line0,display_buf);
	
	//--> �������1���ӿո�,����ַ���
	sprintf((char*) string,"hello");
	LCD_DisplayStringLine(Line2,string);
	sprintf((char*) string,"hi   ");
	LCD_DisplayStringLine(Line2,string);
	//--> �������2����ʽ��������������
	sprintf((char*) display_buf,"%5dHz",5000);			//��ʾ5λ
	LCD_DisplayStringLine(Line3,display_buf);
	sprintf((char*) display_buf,"%5dHz",10);
	LCD_DisplayStringLine(Line3,display_buf);
	
	//��ʽ���������
	sprintf((char*) display_buf,"%-5dHz",10);			//�����
	LCD_DisplayStringLine(Line4,display_buf);
	
	sprintf((char*) display_buf,"%05dHz",500);			//ǰ�油0
	LCD_DisplayStringLine(Line5,display_buf);
	
	sprintf((char*) display_buf,"%.fHz",3.1415926);	//��ʾС����Ĭ��6λ
	LCD_DisplayStringLine(Line6,display_buf);
	
	sprintf((char*) display_buf,"%x",15);				//%x��ʾ16����,%o��ʾ8����
	LCD_DisplayStringLine(Line7,display_buf);
	
	sprintf((char*) display_buf,"%c",'a');				//%s�ַ�����%c�ַ�
	LCD_DisplayStringLine(Line8,display_buf);
	
	sprintf((char*) display_buf,"%d %% ",10);			//����ٷֺţ�%
	LCD_DisplayStringLine(Line9,display_buf);
}
//Main Body
int main(void)
{
	SysTick_Config(SystemCoreClock/1000);

	Delay_Ms(200);
	
	STM3210B_LCD_Init();
	LCD_Clear(White);
	LCD_SetBackColor(White);
	LCD_SetTextColor(Black);

	Display();
	while(1);
}


void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
