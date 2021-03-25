#include "stm32f10x.h"
#include "lcd.h"
#include "led.h"
#include "tim.h"
#include "key.h"
#include "rtc.h"
#include "stdio.h"
#include "string.h"
#include "pwm.h"

u32 TimingDelay = 0;

uint8_t key_cnt = 0;
_Bool key_flag = 0;

uint16_t lcd_cnt = 0;
_Bool lcd_flag = 0;

uint16_t liushui_cnt = 0;
uint8_t liushui_x = 1;
_Bool liushui_flag = 0;

uint8_t lcd_str[20];

uint8_t aim_floor = 0;				// ������Ż�ʣ��Ŀ���������ʾ����Ϊ(0x01 << x)��ʾ��(x+1)�㡣
															// ���ֵΪ0x3��Ҳ����Ŀ���ǵ�һ��͵ڶ���
														
uint8_t now_floor = 1;

_Bool key_pressed = 0;				// 1��ʾ�а��������£�0��ʾû�а���������

_Bool open_flag = 0;					// 1��ʾ�������У�0��ʾ����ֹͣ����

_Bool delay_1ms_flag = 0;			
uint16_t delay_1ms_cnt = 0;

_Bool delay_6ms_flag = 0;
uint16_t delay_6ms_cnt = 0;

_Bool delay_4ms_flag = 0;
uint16_t delay_4ms_cnt = 0;

_Bool arrived_flag = 1;				// 1��ʾ���ݵ�������һ��

_Bool up_flag = 1;						// 1��ʾ���ݵ�ǰ�����з���Ϊ���ϣ���������

_Bool lcd_on_flag = 0;				// 1��ʾ����lcd��ǰ¥����1s��˸����

_Bool opening_flag = 0;				// 1��ʾ��ǰ���������ڱ���
uint16_t opening_cnt = 0;			

_Bool closing_flag = 0;				// 1��ʾ��ǰ���������ڱ��ر�
uint16_t closing_cnt = 0;

void Delay_Ms(u32 nTime);

void gpio_init(void)	// ��ʼ��PA4��PA5
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
	
void pwm_enable(uint16_t GPIO_Pin_x, FunctionalState NewState)		// ����pwmͨ���Ƿ��������
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_x ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if(NewState == ENABLE)	// ����򿪣�ʹ�ܣ�PWM�������
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	else	// ����رգ�����͵�ƽ��ʾ����ر�
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIOA->ODR &= ~GPIO_Pin_x;
	}
}

void key_scan(void)
{
	key_refresh();		// ����һ�µ�ǰ�İ���״̬
	
	// ��Ϊ������ת�ڼ䰴����Ч������������Ҫ�жϵ����Ƿ��������У�Ҳ���Ǵ������������½������Ż��߹��ŵ�״̬�У�
	if(arrived_flag && closing_flag == 0 && opening_flag == 0 && falling_flag != 0x00)	
	{
		key_pressed = 1;
		delay_1ms_cnt = 0;
		aim_floor |= falling_flag;		// ��ǰ�����Ͷ�Ӧ��¥�㣬������ʡȥ�˺ܶ���ж�
	}
}

// Main Body
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	SysTick_Config(SystemCoreClock/1000);

	Delay_Ms(200);
	
	STM3210B_LCD_Init();	
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	led_init();												// ��ʼ��led
	key_init();												// ��ʼ������
	tim_init();												// ��ʼ����ʱ��
	rtc_init();												// ��ʼ��rtc
	Time_Adjust(12,50,55);						// ����ʼ��ʱ���趨Ϊ12��50��55
	pwm_init();												// ��ʼ��pwm
	gpio_init();											// ��ʼ��PA4��PA5
	pwm_enable(GPIO_Pin_7 | GPIO_Pin_6,DISABLE);	// ��ʼ��ģ����Ϊ�رգ���pwm�����
	GPIOA->ODR |= GPIO_Pin_5;											// ��Ϊ�ϵ��ʱ��Ĭ���ǿ��ţ�PA5����ߵ�ƽ
	GPIOA->ODR |= GPIO_Pin_4;											// ��Ϊ�ϵ��ʱ��Ĭ�������ϣ�PA4����ߵ�ƽ
	
	while(1)
	{
		
		Time_Refresh();		// ˢ��ʱ��
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"      %02d:%02d:%02d      ",THH,TMM,TSS);
		LCD_DisplayStringLine(Line6 ,lcd_str);		// ����ǰʱ����ʾ����Ļ��
		
		if(up_flag)		// ������з�������
		{
			GPIOA->ODR |= GPIO_Pin_4;		// PA4����ߵ�ƽ
		}
		else
		{
			GPIOA->ODR &= ~GPIO_Pin_4;		// PA4����͵�ƽ
		}
		
		if(closing_flag || opening_flag || (open_flag && arrived_flag == 0))		// ���������ڹ���״̬�����������У����ߴ��ڿ����ŵ�״̬
		{
			GPIOA->ODR &= ~GPIO_Pin_5;
		}
		else		// ���������ڿ���״̬������û�������У�û�д��ڿ����ŵ�״̬
		{
			GPIOA->ODR |= GPIO_Pin_5;
		}
		
		if(closing_flag)		// �������ڹ���
		{
			pwm_enable(GPIO_Pin_7,ENABLE);		// ��PA7 pwm���
			CH2_Duty = 0.5;
		}
		else if(opening_flag)		// �������ڹ���
		{
			pwm_enable(GPIO_Pin_7,ENABLE);		// ��PA7 pwm���
			CH2_Duty = 0.6;		// ռ�ձ�����Ϊ0.6
		}
		else
		{
			pwm_enable(GPIO_Pin_7,DISABLE);		// ���ݲ��ڿ����ŵ�״̬��ʧ��pwm������͵�ƽ
		}
		
		if(open_flag && arrived_flag == 0 && opening_flag == 0 && closing_flag == 0)
		{		// ���ݴ������������½���״̬�������������У�����û�е��û�д��ڿ����ŵ�״̬
			if(up_flag)
				CH1_Duty = 0.8;		// �������ϣ�ռ�ձ�����Ϊ0.8
			else
				CH1_Duty = 0.6;		// �����½���ռ�ձ�����Ϊ0.6
			pwm_enable(GPIO_Pin_6,ENABLE);	// ��PA6 pwm���
		}
		else
		{
			pwm_enable(GPIO_Pin_6,DISABLE);	// �ر�PA6 pwm���������͵�ƽ
		}
		
		if(!lcd_flag)		// lcd��ʾ¥�㣬��lcd_flag==1ʱ��ʾ������ʵ��¥��ŵ���˸
		{
			switch(now_floor)
			{
				case 0x01: LCD_DisplayStringLine(Line4 ,(uint8_t *)"         1         "); break;
				case 0x02: LCD_DisplayStringLine(Line4 ,(uint8_t *)"         2         "); break;
				case 0x04: LCD_DisplayStringLine(Line4 ,(uint8_t *)"         3         "); break;
				case 0x08: LCD_DisplayStringLine(Line4 ,(uint8_t *)"         4         "); break;
				default : break;
			}
		}
		else			// lcd_flag==0ʱ����ʾ������ʵ��¥��ŵ���˸
		{
			LCD_DisplayStringLine(Line4 ,(uint8_t *)"                   ");
		}
		
		
		if(key_flag)		// ����ɨ��
		{
			key_flag = 0;
			key_scan();
		}
		
		if(now_floor & aim_floor)		// ��ʾ��ǰ¥�㵽��Ŀ��¥��
		{
			led_ctrl(now_floor << 8,DISABLE);		// ����ʾ��ǰ¥���ledϨ��
			aim_floor &= ~now_floor;	// ��Ŀ��¥����ȥ����ǰ¥��
			if(aim_floor == 0x00)			// ���û��ʣ���Ŀ��¥���ˣ��򽫵��ݹر�
			{
				open_flag = 0;
			}
		}
		
		if(aim_floor)		// ��ʾ��ǰ��ʾ����ʣ���¥���led
		{
			led_ctrl(aim_floor << 8,ENABLE);
		}
		
		if(!arrived_flag && liushui_flag)		// ��ˮ�ƴ򿪣���ʾ��ǰ������������
		{
			liushui_flag = 0;
			led_ctrl((uint16_t)(liushui_x << 12),DISABLE);
			if(!up_flag)
			{
				liushui_x = liushui_x >> 1;
				if(liushui_x == 0x00)
					liushui_x = 0x08;
			}
			else
			{
				liushui_x = liushui_x << 1;
				if(liushui_x == 0x10)
					liushui_x = 0x01;
			}
			led_ctrl((uint16_t)(liushui_x << 12),ENABLE);
		}
		
		
		if(arrived_flag == 1 || closing_flag || opening_flag)		// �����ǰ����û�������������½���״̬����ر���ˮ��
		{
			led_ctrl(LD5 | LD6 | LD7 | LD8,DISABLE);
			liushui_x = 1;
			liushui_flag = 0;
			liushui_cnt = 0;
		}
	}
}


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
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		if(++key_cnt >= 10)		// 10ms����һ���ж�
		{
			key_cnt = 0;
			key_flag = 1;
		}
		if(++liushui_cnt >= 500)	// ��ˮ��500ms��һ��
		{
			liushui_cnt = 0;
			liushui_flag = 1;
		}
		
		if(key_pressed && ++delay_1ms_cnt >= 1000)	// ������Ч����������1s�󣬿�ʼ����
		{
			delay_1ms_cnt = 0;
			key_pressed = 0;
			arrived_flag = 0;
			delay_4ms_cnt = 0;
			closing_flag = 1;
			
			if(aim_floor > now_floor)		// �����ǰ���õ�Ŀ��¥����ǵ�ǰ��¥�㣬��ô������Ч
			{
				up_flag = 1;
				open_flag = 1;
			}
			else // ������Ч�����ݿ�ʼ����
			{
				up_flag = 0;
				open_flag = 1;
			}
		}
		
		if(open_flag && arrived_flag == 0 && opening_flag == 0 && closing_flag == 0 &&  ++delay_6ms_cnt >= 6000)
		{		// ���ݴ�������״̬
			delay_6ms_cnt = 0;
			arrived_flag = 1;
			opening_flag = 1;
			lcd_on_flag = 1;
			
			if(up_flag)
				now_floor <<= 1;
			else
				now_floor >>= 1;
			
			if(up_flag && now_floor == 0x08)		// �߽��ж�
			{
				up_flag = 0;
			}
			else if(up_flag == 0 && now_floor == 0x01)		// �߽��ж�
			{
				up_flag = 1;
			}
			
		}
		
		if(lcd_on_flag && ++lcd_cnt == 250)		// ���������е�Ŀ��ƽ̨ʱ��ƽ̨������1������˸2��
		{
			lcd_flag = 1;
		}
		else if(lcd_on_flag && lcd_cnt == 500)
		{
			lcd_flag = 0;
		}
		else if(lcd_on_flag && lcd_cnt == 750)
		{
			lcd_flag = 1;
		}
		else if(lcd_on_flag && lcd_cnt == 1000)
		{
			lcd_flag = 0;
			lcd_on_flag = 0;
			lcd_cnt = 0;
		}
		
		if(opening_flag && ++opening_cnt >= 1000)	// ���ݴ��ڿ���״̬��1s
		{
			opening_flag = 0;
			opening_cnt = 0;
		}
		
		if(closing_flag && ++closing_cnt >= 1000)	// ���ݴ��ڹ���״̬��1s
		{
			closing_flag = 0;
			closing_cnt = 0;
		}
		
		if(open_flag && arrived_flag && opening_flag == 0 && key_pressed == 0 && ++delay_4ms_cnt >= 2000)
		{	// ���ݴ��ڵȴ�״̬
			delay_4ms_cnt = 0;
			arrived_flag = 0;
			closing_flag = 1;
		}
  }
}
