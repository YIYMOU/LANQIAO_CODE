/*
	例程：LED
	作者：电子设计工坊
	重点：	（1） 掌握LED点亮、熄灭、流水灯、闪烁操作；
			（2） 掌握LCD和LED冲突解决方法；
			
	Copyright (c) 2020 电子设计工坊 dianshe.taobao.com
	All rights reserved
*/
#include "stm32f10x.h"
#include "lcd.h"
#include "stdio.h" 		//调用sprintf需要包含此头文件
#include "led.h"

u32 TimingDelay = 0;

void Delay_Ms(u32 nTime);

void LCD_Display(void)
{
	u8 display_buf[20];
	
	u8 string[20];//显示字符串
	
	//【问题】长数据对短数据覆盖问题
	sprintf((char*) display_buf,"%dHz",5000);
	LCD_DisplayStringLine(Line0,display_buf);
	sprintf((char*) display_buf,"%dHz",10);
	LCD_DisplayStringLine(Line0,display_buf);
	
	//--> 解决方案：加空格,针对字符串
	sprintf((char*) string,"hello");
	LCD_DisplayStringLine(Line2,string);
	sprintf((char*) string,"hi   ");
	LCD_DisplayStringLine(Line2,string);
	//--> 解决方案：格式化输出，针对数据
	sprintf((char*) display_buf,"%5dHz",5000);			//显示5位
	LCD_DisplayStringLine(Line3,display_buf);
	sprintf((char*) display_buf,"%5dHz",10);
	LCD_DisplayStringLine(Line3,display_buf);
	
	//格式化输出例子
	sprintf((char*) display_buf,"%-5dHz",10);			//左对齐
	LCD_DisplayStringLine(Line4,display_buf);
	
	sprintf((char*) display_buf,"%05dHz",500);			//前面补0
	LCD_DisplayStringLine(Line5,display_buf);
	
	sprintf((char*) display_buf,"%.fHz",3.1415926);		//显示小数，默认6位
	LCD_DisplayStringLine(Line6,display_buf);
	
	sprintf((char*) display_buf,"%x",15);				//%x显示16进制,%o显示8进制
	LCD_DisplayStringLine(Line7,display_buf);
	
	sprintf((char*) display_buf,"%c",'a');				//%s字符串，%c字符
	LCD_DisplayStringLine(Line8,display_buf);
	
	sprintf((char*) display_buf,"%d %% ",10);			//输出百分号：%
	LCD_DisplayStringLine(Line9,display_buf);
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
	LCD_Display();
	while(1)
	{		
		LED_Control(0x01,1);
	}
}


void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
