#include "stm32f10x.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "tim.h"
#include "stdio.h"
#include "string.h"
#include "adc.h"
#include "usart.h"

#define PARA		1
#define DATA		0

u32 TimingDelay = 0;

uint8_t adc_cnt = 0;
_Bool adc_flag = 0;

_Bool interface = 0;

uint8_t key_cnt = 0;
_Bool key_flag = 0;

uint8_t T = 0;
uint16_t T_cnt = 0;
_Bool T_flag = 0;

uint8_t v_max = 30;
uint8_t v_min = 10;

uint8_t v_max_temp = 32;
uint8_t v_min_temp = 21;

float adc_value = 0.0;
uint8_t last_adc_value_u8 = 33;
uint8_t adc_value_u8 = 0;

uint8_t lcd_str[20];

#define RxBufferSize   20

uint8_t RxBuffer[20];
uint8_t RxCounter = 0;
uint8_t RxFlag = 0;
uint8_t RxCnt = 0;


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
			v_max_temp = v_max;
			v_min_temp = v_min;
		}
		else
		{
			if((v_max_temp - v_min_temp) >= 10)
			{
				v_max = v_max_temp;
				v_min = v_min_temp;
			}
			interface = DATA;
		}
	}
	else if(interface == PARA && key_falling == B2)
	{
		if(v_max_temp == 33)
			v_max_temp = 0;
		else
			v_max_temp++;
	}
	else if(interface == PARA && key_falling == B3)
	{
		if(v_min_temp == 33)
			v_min_temp = 0;
		else
			v_min_temp++;
	}
}

//Main Body
int main(void)
{
	 
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	SysTick_Config(SystemCoreClock/1000);

	Delay_Ms(200);
	
	led_init();
	STM3210B_LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
	tim_init();
	key_init();
	adc_init();
	usart_init();
	
	
	while(1)
	{
		if(adc_flag)
		{
			adc_flag = 0;
			adc_value = adc_get();
			adc_value_u8 = adc_value * 10;
			if(adc_value_u8 >= v_min && last_adc_value_u8 < v_min)
			{
				T_flag = 1;	
				T_cnt = 0;
				T = 0;
				led_ctrl(LD1,ENABLE);
			}
			
			if(adc_value_u8 >= v_max && T_flag)
			{
				T_flag = 0;
				T_cnt = 0;
				led_ctrl(LD1,DISABLE);
			}
			last_adc_value_u8 = adc_value_u8;
		}
		
		if(interface == DATA)
		{
			LCD_DisplayStringLine(Line0 ,(unsigned char *)"      Data          ");
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char*)lcd_str," V:%.2fV           ",adc_value);
			LCD_DisplayStringLine(Line2 ,lcd_str);

			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char*)lcd_str," T:%02ds           ",T);
			LCD_DisplayStringLine(Line3 ,lcd_str);
		}
		else
		{
			LCD_DisplayStringLine(Line0 ,(unsigned char *)"      Para          ");
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char*)lcd_str," Vmax:%.1fV   ",v_max_temp / 10.0);
			LCD_DisplayStringLine(Line2 ,lcd_str);
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char*)lcd_str," Vmin:%.1fV   ",v_min_temp / 10.0);
			LCD_DisplayStringLine(Line3 ,lcd_str);
		}
		
		if(RxFlag && RxCounter)
		{
			uint8_t temp1,temp2;
			if(	RxBuffer[0] <= '9' && RxBuffer[0] >= '0' && RxBuffer[1] == '.' && \
					RxBuffer[2] <= '9' && RxBuffer[2] >= '0' && RxBuffer[3] == ',' && \
					RxBuffer[4] <= '9' && RxBuffer[4] >= '0' && RxBuffer[5] == '.' && \
					RxBuffer[6] <= '9' && RxBuffer[6] >= '0' && RxBuffer[7] == 0)
			{
				temp1 = (RxBuffer[0] - '0') * 10 + (RxBuffer[2] - '0');
				temp2 = (RxBuffer[4] - '0') * 10 + (RxBuffer[6] - '0');
				if((temp1 - temp2) >= 10 && temp1 <= 33 && temp2 <= 33)
				{
					v_max = temp1;
					v_min = temp2;
					v_max_temp = v_max;
					v_min_temp = v_min;
					led_ctrl(LD3,DISABLE);
				}
				else
				{
					led_ctrl(LD3,ENABLE);
				}
			}
			else
			{
				led_ctrl(LD3,ENABLE);
			}
//			memset(lcd_str,0,sizeof(lcd_str));
//			sprintf((char*)lcd_str,"%-20.20s",RxBuffer);
//			LCD_DisplayStringLine(Line9 ,lcd_str);
			memset(RxBuffer,0,sizeof(RxBuffer));
			RxCounter = 0;
		}
		if((v_max_temp - v_min_temp) >= 10)
		{
			led_ctrl(LD2,DISABLE);
		}
		else
		{
			led_ctrl(LD2,ENABLE);
		}
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
			key_cnt = 0;
			key_scan();
		}
		if(T_flag && ++T_cnt == 1000)
		{
			T_cnt = 0;
			T++;
		}
		if(++adc_cnt == 100)
		{
			adc_cnt = 0;
			adc_flag = 1;
		}
		if(!RxFlag && ++RxCnt == 50)
		{
			RxCnt = 0;
			RxFlag = 1;
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
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
		
    RxBuffer[RxCounter++] = USART_ReceiveData(USART2);
		if(RxCounter == RxBufferSize)
		{
			RxCounter = 0;
			memset(RxBuffer,0,sizeof(RxBuffer));
		}
		RxCnt = 0;
		RxFlag = 0;
  }
}
