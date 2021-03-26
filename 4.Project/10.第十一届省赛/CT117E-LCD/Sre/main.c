#include "stm32f10x.h"
#include "lcd.h"
#include "led.h"
#include "tim.h"
#include "key.h"
#include "adc.h"
#include "string.h"
#include "stdio.h"
#include "pwm.h"

#define AOTO_MODE		0

#define DATA				0
#define PARA				1

u32 TimingDelay = 0;

uint16_t led_cnt = 0;
_Bool led_flag = 0;

uint8_t key_cnt = 0;

_Bool mode = 0;	// 0：自动，1：手动

_Bool interface = 0;	// 0：数据界面，1：参数界面

float adc_value = 0.0;

uint8_t pa6_para = 10;
uint8_t pa7_para = 10;

uint8_t lcd_str[20];

void Delay_Ms(u32 nTime);

void key_scan(void)
{
	key_refresh();
	if(key_falling == B1)
	{
		interface = !interface;
	}
	else if(interface == PARA && key_falling == B2)
	{
		if(pa6_para != 90)
			pa6_para += 10;
		else
			pa6_para = 10;
	}
	else if(interface == PARA && key_falling == B3)
	{
		if(pa7_para != 90)
			pa7_para += 10;
		else
			pa7_para = 10;
	}
	else if(key_falling == B4)
	{
		if(mode == AOTO_MODE)
		{
			pa6_duty = 0.1;
			pa7_duty = 0.1;
		}
		mode = !mode;
	}
}

void pwm_enable(uint16_t PAx,FunctionalState NewState,_Bool state)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = PAx;
	if(NewState == ENABLE)
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	else
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		if(state)
		{
			GPIOA->ODR |= PAx;
		}
		else
		{
			GPIOA->ODR &= ~PAx;
		}
	}
}

//Main Body
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	SysTick_Config(SystemCoreClock/1000);

	Delay_Ms(200);
	
	led_init();
	tim_init();
	key_init();
	adc_init();
	pwm_init();
	
	STM3210B_LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
	
	
	while(1)
	{
		if(interface == DATA)
		{
			LCD_DisplayStringLine(Line0 ,(unsigned char *)"      Data          ");
			adc_value = adc_get();
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char *)lcd_str,"    V:%.2fV         ",adc_value);
			LCD_DisplayStringLine(Line2 ,lcd_str);
			if(mode == AOTO_MODE)
			{
				LCD_DisplayStringLine(Line4 ,(unsigned char *)"    Mode:AUTO       ");
				if(ADC_GetConversionValue(ADC1) > 4090)
				{
					pwm_enable(GPIO_Pin_6 | GPIO_Pin_7,DISABLE,1);
				}
				else if(ADC_GetConversionValue(ADC1) < 10)
				{
					pwm_enable(GPIO_Pin_6 | GPIO_Pin_7,DISABLE,0);
				}
				else
				{
					pwm_enable(GPIO_Pin_6 | GPIO_Pin_7,ENABLE,0);
					pa6_duty = adc_value / 3.3;
					pa7_duty = adc_value / 3.3;
				}
			}
			else
			{
				LCD_DisplayStringLine(Line4 ,(unsigned char *)"    Mode:MANU       ");
				pwm_enable(GPIO_Pin_6 | GPIO_Pin_7,ENABLE,0);
				pa6_duty = pa6_para / 100.0;
				pa7_duty = pa7_para / 100.0;
			}
			
		}
		else
		{
			LCD_DisplayStringLine(Line0 ,(unsigned char *)"      Para          ");
			
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char *)lcd_str,"    PA6:%d%%         ",pa6_para);
			LCD_DisplayStringLine(Line2 ,lcd_str);
			
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char *)lcd_str,"    PA7:%d%%         ",pa7_para);
			LCD_DisplayStringLine(Line4 ,lcd_str);
			
		}
		if(mode == AOTO_MODE)
		{
			led_ctrl(LD1,ENABLE);
		}
		else
		{
			led_ctrl(LD1,DISABLE);
		}
		if(interface == DATA)
		{
			led_ctrl(LD2,ENABLE);
		}
		else
		{
			led_ctrl(LD2,DISABLE);
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
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		if(++key_cnt == 10)
		{
			key_cnt = 0;
			key_scan();
		}
  }
}

