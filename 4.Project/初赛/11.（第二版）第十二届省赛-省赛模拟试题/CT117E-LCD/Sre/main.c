#include "stm32f10x.h"
#include "lcd.h"
#include "i2c.h"
#include "key.h"
#include "led.h"
#include "tim.h"
#include "adc.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"

#define 		PARA					0
#define 		DATA					1

u32 TimingDelay = 0;

uint8_t key_cnt = 0;


uint8_t lcd_str[20];

_Bool interface = DATA;

float Volt = 0.0;

uint8_t v_max = 30;
uint8_t v_min = 10;

uint8_t v_max_temp = 30;
uint8_t v_min_temp = 10;


_Bool timer_flag = 0;
uint8_t total_time = 0;
uint16_t ms_tick = 0;

_Bool cmd_eeror = 0;

uint16_t read_adc_tick = 0;
_Bool read_adc_flag = 1;

uint8_t RxBuffer[20];
uint8_t RxBuffer2[20];
uint8_t RxCounter2 = 0;
uint8_t RxTick = 0;
_Bool RxFlag = 0;
_Bool RxProcFlag = 0;

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
		if(interface == PARA)
		{
			if(v_max_temp >= v_min_temp + 10)
			{
				v_max = v_max_temp;
				v_min = v_min_temp;
			}
			interface = DATA;
		}
		else
		{
			interface = PARA;
		}
	}
	else if(interface == PARA && key_falling == B2)
	{
		v_max_temp = (v_max_temp + 1) % 34;
	}
	else if(interface == PARA && key_falling == B3)
	{
		v_min_temp = (v_min_temp + 1) % 34;
	}
//	else if(key_falling == B4)
//	{
//		
//	}
}

void lcd_proc(void)
{
	if(interface == PARA)
	{
		LCD_DisplayStringLine(Line0 ,(unsigned char *)"      Para          ");
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str," Vmax:%.2fV        ",v_max_temp / 10.0);
		LCD_DisplayStringLine(Line2,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str," Vmin:%.2fV        ",v_min_temp / 10.0);
		LCD_DisplayStringLine(Line3,lcd_str);
	}
	else
	{
		LCD_DisplayStringLine(Line0 ,(unsigned char *)"      Data          ");
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str," V:%.2fV            ",Volt);
		LCD_DisplayStringLine(Line2,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str," T:%ds        ",total_time);
		LCD_DisplayStringLine(Line3,lcd_str);
		
	}
}

void led_proc(void)
{
	if(timer_flag)
	{
		led_ctrl(LD1,ENABLE);
	}
	else
	{
		led_ctrl(LD1,DISABLE);
	}
	if(v_max_temp <= v_min_temp + 10)
	{
		led_ctrl(LD2,ENABLE);
	}
	else
	{
		led_ctrl(LD2,DISABLE);
	}
	if(cmd_eeror)
	{
		led_ctrl(LD2,ENABLE);
	}
	else
	{
		led_ctrl(LD2,DISABLE);
	}
}

void adc_proc(void)
{
	if(read_adc_flag)
	{
		read_adc_flag = 0;
		Volt = adc_conv();
	}
}

void check_state(void)
{
	static float Volt_pre = 3.3;
	if(Volt_pre < (v_min / 10.0) && Volt >= (v_min / 10.0))
	{
		timer_flag = 1;
		ms_tick = 0;
		total_time = 0;
	}
	if(Volt_pre < (v_max / 10.0) && Volt >= (v_max / 10.0))
	{
		timer_flag = 0;
	}
	Volt_pre = Volt;
}

void usart_proc(void)
{
	if(RxProcFlag)
	{
		RxProcFlag = 0;
		
		if(RxBuffer2[0] <= '9' && RxBuffer2[0] >= '0' && RxBuffer2[1] == '.' && RxBuffer2[2] >= '0' && RxBuffer2[2] <= '9' &&  \
			RxBuffer2[3] == ',' && RxBuffer2[4] >= '0' && RxBuffer2[4] <= '9' && RxBuffer2[5] == '.' && RxBuffer2[6] <= '9' && RxBuffer2[6] >= '0')
		{
			uint8_t min,max;
			max = (RxBuffer2[0] - '0') * 10 + (RxBuffer2[2] - '0');
			min = (RxBuffer2[4] - '0') * 10 + (RxBuffer2[6] - '0');
			if((min + 10) < max && min < 33 && max < 33)
			{
				cmd_eeror = 0;
				v_max = max;
				v_min = min;
			}
			else
			{
				cmd_eeror = 1;
			}
		}
		else
		{
			cmd_eeror = 1;
		}
//		memset(lcd_str,0,sizeof(lcd_str));
//		sprintf((char*)lcd_str,"%-20.20s",RxBuffer2);
//		LCD_DisplayStringLine(Line9,(unsigned char*)lcd_str);
		
		memset(RxBuffer2,0,sizeof(RxBuffer2));
		RxCounter2 = 0;
	}
}


//Main Body
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
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
	usart_init();
	
	Delay_Ms(200);
	
	while(1)
	{
		
		usart_proc();
		
		adc_proc();
		
		check_state();
		
		led_proc();
		
		lcd_proc();
		
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
		
		if(timer_flag && ++ms_tick == 1000)
		{
			ms_tick = 0;
			total_time++;
		}
		
		if(++read_adc_tick == 450)
		{
			read_adc_tick = 0;
			read_adc_flag = 1;
		}
		
		if(RxFlag && ++RxTick == 50)
		{
			RxFlag = 0;
			RxTick = 0;
			RxProcFlag = 1;
		}
  }
}


/**
  * @brief  This function handles USART2 global interrupt request.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{
	uint8_t ch;
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
		ch = USART_ReceiveData(USART2);
		RxBuffer2[RxCounter2++] = ch;
		RxFlag = 1;
		RxTick = 0;
    if(RxCounter2 == 20)
    {
      memset(RxBuffer2,0,sizeof(RxBuffer2));
			RxCounter2 = 0;
    }
  }
}

