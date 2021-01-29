#include "stm32f10x.h"
#include "lcd.h"
#include "LED.h"
#include "tim.h" 
#include "buzzer.h"
#include "key.h"
#include "stdio.h"
#include "i2c.h"
#include "string.h"
#include "adc.h"
#include "rtc.h"
#include "tim.h"
#include "pwm.h"
#include "pwm_octoggle.h"
#include "InputCapture.h"

u32 TimingDelay = 0;

_Bool key_read_flag = 0;
_Bool led_flag = 0;

uint16_t key_time = 0;
uint16_t key_pressed_time = 0;
uint16_t led_cnt = 0;
uint16_t key_cnt = 0;

uint16_t key1Time = 0;
uint8_t key1PressNumber = 0;

uint8_t lcd_buf[20];

void Delay_Ms(u32 nTime);

void Key_Scan(void)
{
	Key_Read();
	
	if(key1PressNumber == 1)	// ��⵽һ�ΰ����������أ���ʼ��ʱ
	{
		key1Time += 10;			// ���ڰ���������10msɨ��һ�Σ����������10ms
		if(key1Time >= 150)	// �������1500ms����ʾ�����ǵ�������ʼ��˫�����
		{
			// begin
			// ��������Ӱ��������Ĵ���
			
			// end
			key1Time = 0;
			key1PressNumber = 0;
		}
	}
	
	if(Key_Trg_Rising == 0x01)	// �����������أ��������ɿ�
	{
		if(key1PressNumber == 0)	// ��ʾ�����ǵ�һ�ΰ�����������
		{
			key1PressNumber = 1;		// �������
		}
	}
	
	if(Key_Trg_Falling == 0x01)	// ��⵽�������½���
	{
		LED_Ctrl(0x10,ENABLE);		
		if(key1PressNumber == 1)	// ����Ѿ�������һ�ΰ����������أ��������ڻ�û�г�ʱ����ô�����һ��˫���Ķ���
		{
			// begin
			// �����������˫����Ҫ����ĳ���
			
			// end
		}		
	}
	else if(Key_Trg_Falling == 0x02)
	{
	}
	else if(Key_Trg_Falling == 0x04)
	{
	}
	else if(Key_Trg_Falling == 0x08)
	{
	}
	
	if(key_State == 0x01)
	{
		key_pressed_time+=10;
		if(key_time != 0xffff && ++key_time >= 50)
		{
			key_time = 0xffff;	// ��ֹ�ظ�����γ������ɿ�������ʱ�򣬽�key_time����			
		}
	}
	
	if(key_State == 0 && Key_Trg_Falling == 0)	// �����ɿ�
	{
		key_pressed_time = 0;
		key_time = 0;
		LED_Ctrl(0xF0,DISABLE);
	}
}

//Main Body
int main(void)
{
//	uint8_t i;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	SysTick_Config(SystemCoreClock/1000);
	Delay_Ms(200);
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	LED_Init();
	KEY_Init();
	Tim_Init();
	//PWM_Init();
	PWM_OCTOGGLE_Init();
	InputCapture_Init();
	CCR2_Val = 24000;
	CCR3_Val = 24000;
	while(1)
	{
		
		if(led_flag)
		{
			led_flag = 0;
			LED_Toggle(0x01);
		}
		
		sprintf((char*)lcd_buf,"T3CH2_Freq= %8d",TIM3Freq);
		LCD_DisplayStringLine(Line3,lcd_buf);
		
		sprintf((char*)lcd_buf,"T3CH2_Duty= %8d",TIM3Duty);
		LCD_DisplayStringLine(Line4,lcd_buf);
		
		sprintf((char*)lcd_buf,"ReadValue1= %8d",IC3ReadValue1);
		LCD_DisplayStringLine(Line5,lcd_buf);
		
		sprintf((char*)lcd_buf,"ReadValue2= %8d",IC3ReadValue2);
		LCD_DisplayStringLine(Line6,lcd_buf);
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}

void TIM4_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		
		if(++led_cnt == 500)
		{
			led_cnt = 0;
			led_flag = 1;
		}
		
  }
}
