#include "stm32f10x.h"
#include "lcd.h"
#include "i2c.h"
#include "key.h"
#include "led.h"
#include "tim.h"
#include "rtc.h"
#include "string.h"
#include "stdio.h"
#include "pwm.h"

#define 		OFF					0
#define 		STOP				1
#define 		OPENNING		2
#define 		CLOSING			3
#define 		MOVING			4

#define 		F1			0x0100
#define 		F2			0x0200
#define 		F3			0x0400
#define 		F4			0x0800

#define			UP			1
#define			DOWM		0

_Bool direction = UP;

uint16_t upper_tick_cnt = 1000;
uint16_t tick_cnt = 0;
_Bool time_out_flag = 0;

uint16_t aim_floor = 0x0000;

u32 TimingDelay = 0;

uint8_t state = OFF;

uint8_t key_cnt = 0;


_Bool led_flag = 0;
uint16_t led_cnt = 0;
uint16_t liushui = 0x1000;

uint8_t lcd_str[20];

uint16_t current_platform = F1;

_Bool waitting_flag = 0;
uint16_t waitting_cnt = 0;

_Bool lcd_flag1 = 0;
_Bool lcd_flag2 = 0;
uint16_t lcd_cnt = 0;


//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}

void key_scan(void)
{
	if(state == OFF || state == STOP)
	{
		key_refresh();
		if(key_falling == B1)
		{
			if(current_platform != F1)
			{
				aim_floor ^= F1;
				waitting_flag = 1;
				waitting_cnt = 0;
			}
		}
		else if(key_falling == B2)
		{
			{
				aim_floor ^= F2;
				waitting_flag = 1;
				waitting_cnt = 0;
			}
		}
		else if(key_falling == B3)
		{
			{
				aim_floor ^= F3;
				waitting_flag = 1;
				waitting_cnt = 0;
			}
		}
		else if(key_falling == B4)
		{
			{
				aim_floor ^= F4;
				waitting_flag = 1;
				waitting_cnt = 0;
			}
		}
	}
}

void lcd_proc(void)
{
	
	LCD_DisplayStringLine(Line2 ,(unsigned char *)"  CURRENT-PLATFORM ");
	
	if(!lcd_flag1 || (!lcd_flag2))
	{
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"         %d          ",(current_platform / 256 / 2 + 1) == 5 ? 4: (current_platform / 256 / 2 + 1));
		LCD_DisplayStringLine(Line4 ,lcd_str);
	}
	else
	{
		LCD_DisplayStringLine(Line4 ,(unsigned char *)"                   ");
	}
	memset(lcd_str,0,sizeof(lcd_str));
	sprintf((char*)lcd_str,"      %02d:%02d:%02d",THH,TMM,TSS);
	LCD_DisplayStringLine(Line6 ,lcd_str);
	
//	memset(lcd_str,0,sizeof(lcd_str));
//	sprintf((char*)lcd_str,"%d   ",tick_cnt);
//	LCD_DisplayStringLine(Line8 ,lcd_str);
//	
//	switch(state)
//	{
//		case STOP: 			LCD_DisplayStringLine(Line9 ,(unsigned char *)"  STOP      "); break;
//		case CLOSING: 	LCD_DisplayStringLine(Line9 ,(unsigned char *)"  CLOSING   "); break;
//		case OFF: 			LCD_DisplayStringLine(Line9 ,(unsigned char *)"  OFF       "); break;
//		case MOVING: 		LCD_DisplayStringLine(Line9 ,(unsigned char *)"  MOVING    "); break;
//		case OPENNING: 	LCD_DisplayStringLine(Line9 ,(unsigned char *)"  OPENNING  "); break;
//	}
}

void led_proc(void)
{
	if(MOVING != state)
	{
		led_ctrl(0xF000,DISABLE);
	}
	else if(led_flag)
	{
		led_flag = 0;
		led_ctrl(liushui,ENABLE);
		led_ctrl((~liushui) & 0xF000 ,DISABLE);
		if(direction == UP)
		{
			if(liushui != 0x8000) 
				liushui <<= 1;
			else
				liushui = 0x1000;
		}
		else
		{
			if(liushui != 0x1000) 
				liushui >>= 1;
			else
				liushui = 0x8000;
		}
	}
	led_ctrl(aim_floor,ENABLE);
	led_ctrl((~aim_floor) & 0x0F00,DISABLE);
}

void direction_check(void)
{
	if(aim_floor > current_platform)
	{
		direction = UP;
	}
	else
	{
		direction = DOWM;
	}
}

void update_current_floot(void)
{
	if(direction == UP)
	{
		current_platform <<= 1;
	}
	else
	{
		current_platform >>= 1;
	}
	
	aim_floor &= ~current_platform;
	
	direction_check();
}

void state_change(void)
{
	if(state == STOP)
	{
		state = CLOSING;
		upper_tick_cnt = 1000;		// 关门:1s
	}
	else if(state == CLOSING)
	{
		state = MOVING;
		upper_tick_cnt = 6000;		// 电梯上行、下行时间：6s
	}
	else if(state == MOVING)
	{
		state = OPENNING;
		upper_tick_cnt = 1000;		// 开门:1秒
	}
	else if(state == OPENNING)
	{
		update_current_floot();
		lcd_flag1 = 1;
		if(aim_floor == 0x0000)
			state = OFF;						// 如果没有目标平台，那么电梯停止运行
		else 
			state = STOP;				// 如果还剩有目标楼层，那么电梯继续运行
		upper_tick_cnt = 2000;		// 开门停留两秒
	}
}

void pwm_proc(void)
{
	static uint8_t state_pre = 0;
	if(state_pre != state)
	{
		if(state == MOVING)
		{
			if(direction == UP)
				pwm_init(80,0);
			else
				pwm_init(60,0);
		}
		else if(state == OPENNING)
		{
			pwm_init(0,60);
		}
		else if(state == CLOSING)
		{
			pwm_init(0,50);
		}
		else
		{
			pwm_init(0,0);
		}
		state_pre = state;
	}
}

void gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIOA->ODR &= ~(GPIO_Pin_4 | GPIO_Pin_5);
}

void gpio_proc(void)
{
	if(direction == UP)
	{
		GPIOA->ODR |= GPIO_Pin_4;
	}
	else
	{
		GPIOA->ODR &= ~GPIO_Pin_4;
	}
	if(state == OFF || state == STOP)
	{
		GPIOA->ODR |= GPIO_Pin_5;
	}
	else
	{
		GPIOA->ODR &= ~GPIO_Pin_5;
	}
}

//Main Body
int main(void)
{
	SysTick_Config(SystemCoreClock/1000);

	Delay_Ms(200);
	
	led_init();
	STM3210B_LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
	key_init();
	tim_init();
	i2c_init();
	rtc_init();
	Time_Adjust(12,50,55);
	gpio_init();
	pwm_init(0,0);
	
	while(1)
	{
		Time_Display();
		
		lcd_proc();
		
		led_proc();
		
		pwm_proc();
		
		gpio_proc();
	}
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
		
		if(++key_cnt == 10)
		{
			key_cnt =0;
			key_scan();
		}
		
		if((state == MOVING) && ++led_cnt == 400)
		{
			led_cnt = 0;
			led_flag = 1;
		}
		
		if(waitting_flag == 0 &&  state != OFF && ++tick_cnt == upper_tick_cnt)
		{
			tick_cnt = 0;
			state_change();
		}
		
		if(waitting_flag && ++waitting_cnt == 1000)
		{
			waitting_cnt = 0;
			waitting_flag = 0;
			state = CLOSING;
			direction_check();
		}
		
		if(lcd_flag1 && (++lcd_cnt % 250) == 0)
		{
			if(lcd_cnt == 1000)
			{
				lcd_flag1 = 0;
				lcd_cnt = 0;
				lcd_flag2 = 0;
			}
			else
			{
				lcd_flag2 = !lcd_flag2;
			}
		}
  }
}
