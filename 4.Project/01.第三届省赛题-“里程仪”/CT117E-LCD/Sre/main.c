/************************************************
*���ű�Ƕ��ʽ������ʡ����-������ǡ�
*CSDN��https://blog.csdn.net/qq_43715171
*���ƣ�https://gitee.com/yiymou
*GitHub��https://github.com/YIYMOU
*Ӳ��ƽ̨�����ų���CT117E
*���ߣ�yiymou   2021-02-02
************************************************/
#include "stm32f10x.h"
#include "stdio.h"					// ʹ����sprintf����
#include "lcd.h"
#include "led.h"
#include "tim.h"
#include "i2c.h"
#include "key.h"
#include "pwm_octoggle.h"
#include "string.h"					// ʹ����memset����
#include "InputCapture.h"

u32 TimingDelay = 0;		// ��ʱ�����ı���

uint16_t led_cnt = 0;
_Bool led_flag = 0;

uint16_t key_scan_cnt = 0;
uint16_t key_continue_cnt = 0;

uint8_t lcd_str[20];

uint8_t speed = 0;							// �����趨�ٶ�
uint16_t total_time_cnt = 0;
uint32_t total_time = 0;				// ���ڴ����ʱ��

uint32_t total_meter = 0;				// ���ڴ���������

union EEPROM				// �����壬���ڴ�źͶ�ȡ�����
{
	uint8_t uint8;
	uint16_t uint16;
	uint32_t uint32;
	uint8_t str[20];
} Write_EEPROM,Read_EEPROM;

void Delay_Ms(u32 nTime);

void Key_Scan(void)					// ����ɨ�躯����10msɨ��һ��
{
	Key_Read();			//����һ�ΰ���״̬
	if(falling_flag == K1)		// K1���£������١�������Ƶ�ʲ���ֵΪ 1Hz
	{
		if(speed < 20)
		{
			if(speed == 0)
			{
				TIM_Cmd(TIM3, ENABLE);		// �Ѷ�ʱ���򿪣�Ҳ���������������
			}
			speed++;
			CCR1_Val = 1000 / speed;		// ���÷�����Ƶ��
		}
	}
	if(falling_flag == K2)		// K2���£������١������� Ƶ�ʲ���ֵΪ 1Hz
	{
		if(speed > 0)
		{
			speed--;
			if(speed == 0)
			{	
				TIM_Cmd(TIM3, DISABLE);			// �ٶȼ���0����Ѷ�ʱ���رգ�����������
			}
			else
			{
				CCR1_Val = 1000 / speed;		// ���÷�����Ƶ��
			}
		}
	}
	if(falling_flag == K3)		// K3δʹ��
	{
		
	}
	if(falling_flag == K4)		// K4δʹ��
	{
		
	}
	
	if(key_state == K1)				// �����������
	{
		
		if(key_continue_cnt == 50)		// �������£�����500ms����ʾ����
		{
			
		}
		else
		{
			key_continue_cnt++;		// �������µ�ʱ�����ã�ÿ+1�����ʾʱ����10ms
		}
	}
	
	if(key_state == 0x00)			// �����ɿ�
	{
		key_continue_cnt = 0;		// �����ɿ�����Ѱ������µ�ʱ������
	}
}

//Main Body
int main(void)
{
	int i;
	SysTick_Config(SystemCoreClock/1000);			// ����SySTickʱ��Ϊ1ms
	Delay_Ms(200);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		// �����ж����ȼ���
	Led_Init();			// ��ʼ��LED
	Tim4_Init();		// ��ʼ��TIM4
	Key_Init();			// ��ʼ������
	i2c_init();			// ��ʼ��iic
	Pwm_Octoggle_Init();				// ��ʼ��PWM���
	TIM_Cmd(TIM3, DISABLE);			// ��ʼ��PWM�������ʱ�ٶ�Ϊ0����������������Ѷ�ʱ���رռ���
	Time2_InputCapture_Init();	// ��ʼ����ʱ������
	STM3210B_LCD_Init();				// lcd��ʼ��
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	for(i = 0; i < sizeof(uint32_t); i++)			// ��������eeprom�е���ʻ��̶�ȡ����
	{
		Read_EEPROM.str[i] = Read_AT24C02(i);
	}
	total_meter = Read_EEPROM.uint32;
	while(1)
	{
		// ����˲ʱ�ٶȲ���ʾ��LCD
		memset(lcd_str,0,sizeof(lcd_str));	// ʹ��sprintf֮ǰ���Ƚ�lcd_str�ַ������
		sprintf((char*)lcd_str,"Vim(km/h): %-11.1f", speed==0?0:TIM2Freq*2.0*3.6);		// �����ж�speed�Ƿ�Ϊ0��ԭ���ǣ���ÿһ�������ٶ�Ϊ0��ʱ������ֱ�ӰѶ�ʱ���ر��ˣ�TIM2Freq������һ�ε�ֵ������������Ҫ�ж�һ��
		LCD_DisplayStringLine(Line0 ,lcd_str);
		// ����ƽ���ٶȲ���ʾ��LCD 
		memset(lcd_str,0,sizeof(lcd_str));	// ʹ��sprintf֮ǰ���Ƚ�lcd_str�ַ������
		sprintf((char*)lcd_str,"Vavg(km/h): %-8.1f", total_time==0?0:(total_meter_now*7.2/total_time));		// �����ж�total_time��Ϊ�˷�ֹ��0�Ĵ���
		LCD_DisplayStringLine(Line2 ,lcd_str);
		// ��ʾ��ʻʱ�䵽LCD
		memset(lcd_str,0,sizeof(lcd_str));	// ʹ��sprintf֮ǰ���Ƚ�lcd_str�ַ������
		sprintf((char*)lcd_str,"T(h:m:s): %02d:%02d:%02d  ",total_time/3600,total_time%3600/60,total_time%60);
		LCD_DisplayStringLine(Line4 ,lcd_str);
		// ��ʾ��ǰ��ʻ�Ĺ�������LCD
		memset(lcd_str,0,sizeof(lcd_str));	// ʹ��sprintf֮ǰ���Ƚ�lcd_str�ַ������
		sprintf((char*)lcd_str,"S(km): %12.1f",total_meter_now / 1000.0);
		LCD_DisplayStringLine(Line6 ,lcd_str);
		// ��ʾ����ʹ�Ĺ�������LCD
		memset(lcd_str,0,sizeof(lcd_str));	// ʹ��sprintf֮ǰ���Ƚ�lcd_str�ַ������
		sprintf((char*)lcd_str,"       Total(km):%3.0f", (total_meter + total_meter_now) / 1000.0);
		LCD_DisplayStringLine(Line9 ,lcd_str);
		
		if(TIM2Freq*2.0*3.6 < 90.0)		// ����ٶ�С��90km/h��LD1Ϩ��
		{
			Led_Ctrl(0x01,DISABLE);
		}
		
		if(TIM2Freq*2.0*3.6 >= 90.0 && led_flag)		// ����ٶȴ���90km/h��LD1��1sΪ���������˸
		{
			led_flag = 0;
			Led_Toggle(0x01);
		}
		
		Write_EEPROM.uint32 = total_meter + total_meter_now;		// ����̴�ŵ������壬����֮����б��浽eeprom
		for(i = 0; i < sizeof(uint32_t); i++)		// ����ǰ������̴浽eeprom
		{
			Write_AT24C02(i,Write_EEPROM.str[i]);
			Delay_Ms(5);
		}
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
/**
  * @brief  This function handles TIM4 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM4_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)		// 1ms�ж�
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		if(++led_cnt >= 1000)		// Led��1s�ж�
		{
			led_cnt = 0;
			led_flag = 1;
		}
		if(++key_scan_cnt >= 10)	// 10ms����һ�ΰ�����ɨ��
		{
			Key_Scan();
		}
		if(speed && ++total_time_cnt >= 1000)	// 1sΪ��λ��¼��ʻʱ��
		{
			total_time_cnt = 0;
			total_time++;
		}
  }
}
