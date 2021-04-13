#include "stm32f10x.h"
#include "lcd.h"
#include "i2c.h"
#include "key.h"
#include "led.h"
#include "tim.h"
#include "adc.h"
#include "pwm.h"
#include "string.h"
#include "stdio.h"

#define			DATA				0
#define			PARA				1

#define			AUTO				0
#define			MANU				1

u32 TimingDelay = 0;

uint8_t key_cnt = 0;

_Bool adc_flag = 0;
uint16_t adc_cnt = 0;

uint8_t lcd_str[20];

_Bool interface = DATA;

float Volt = 0.0;
_Bool mode = AUTO;

float PA6_Duty = 0.1;
float PA7_Duty = 0.1;

uint8_t PA6_Duty_temp = 10;
uint8_t PA7_Duty_temp = 10;

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}

void key_scan(void)
{
	key_refresh();
	if(key_falling == B1)
	{
		if(interface == DATA)
		{
			interface = PARA;
		}
		else
		{
			interface = DATA;
		}
	}
	else if(interface == PARA && key_falling == B2)
	{
		if(PA6_Duty_temp == 90)
			PA6_Duty_temp = 10;
		else
			PA6_Duty_temp += 10;
	}
	else if(interface == PARA && key_falling == B3)
	{
		if(PA7_Duty_temp == 90)
			PA7_Duty_temp = 10;
		else
			PA7_Duty_temp += 10;
	}
	else if(key_falling == B4)
	{
		if(mode == MANU)
		{
			mode = AUTO;
		}
		else
		{
			mode = MANU;
		}
	}
}

void lcd_proc(void)
{
	if(interface == DATA)
	{
		LCD_DisplayStringLine(Line0 ,(unsigned char *)"      Data          ");
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"    V:%.2fV         ",Volt);
		LCD_DisplayStringLine(Line3,lcd_str);
		
		if(mode == AUTO)
			LCD_DisplayStringLine(Line5,(unsigned char *)"    Mode:AUTO      ");
		else
			LCD_DisplayStringLine(Line5,(unsigned char *)"    Mode:MANU      ");
	}
	else
	{
		LCD_DisplayStringLine(Line0 ,(unsigned char *)"      Para          ");
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"    PA6:%2d%%      ",PA6_Duty_temp);
		LCD_DisplayStringLine(Line3,lcd_str);
		
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"    PA7:%2d%%      ",PA7_Duty_temp);
		LCD_DisplayStringLine(Line5,lcd_str);
		
	}
	
}

void pwm_ctrl(FunctionalState Newstate)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
  /* GPIOA Configuration:TIM3 Channel1, 2 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  if(Newstate != ENABLE)
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	else
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
}

void pwm_proc()
{
	if(mode == MANU)
	{
		pwm_ctrl(ENABLE);
		PA6_Duty = PA6_Duty_temp / 100.0;
		PA7_Duty = PA7_Duty_temp / 100.0;
	}
	else
	{
		if(Volt > 3.295)
		{
			pwm_ctrl(DISABLE);
			GPIOA->ODR |= GPIO_Pin_6 | GPIO_Pin_7;
		}
		else if(Volt < 0.01)
		{
			pwm_ctrl(DISABLE);
			GPIOA->ODR &= ~(GPIO_Pin_6 | GPIO_Pin_7);
		}
		else
		{
			pwm_ctrl(ENABLE);
			PA6_Duty = ADCConvertedValue / 4095.0;
			PA7_Duty = ADCConvertedValue / 4095.0;
		}
	}
}

void led_proc(void)
{
	if(mode == AUTO)
		led_ctrl(LD1,ENABLE);
	else
		led_ctrl(LD1,DISABLE);
	if(interface == PARA)
		led_ctrl(LD2,ENABLE);
	else
		led_ctrl(LD2,DISABLE);
}

void adc_proc(void)
{
	if(adc_flag)
	{
		Volt = ADCConvertedValue / 4095.0 * 3.3;
		adc_flag = 0;
	}
}

//Main Body
int main(void)
{
	SysTick_Config(SystemCoreClock/1000);

	led_init();
	STM3210B_LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
	key_init();
	tim_init();
	i2c_init();
	adc_init();
	pwm_init();
	
	while(1)
	{
		
		adc_proc();
		
		lcd_proc();
		
		pwm_proc();
		
		led_proc();
		
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
		
		if(++adc_cnt == 80)
		{
			adc_cnt = 0;
			adc_flag = 1;
		}
  }
}
