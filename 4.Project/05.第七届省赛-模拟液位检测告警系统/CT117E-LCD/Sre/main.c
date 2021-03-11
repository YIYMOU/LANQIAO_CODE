#include "stm32f10x.h"
#include "lcd.h"
#include "led.h"
#include "tim.h"
#include "i2c.h"
#include "key.h"
#include "stdio.h"
#include "string.h"
#include "adc.h"
#include "usart.h"

u32 TimingDelay = 0;

_Bool key_flag = 0;
uint8_t key_cnt = 0;

uint8_t lcd_str[20];

uint16_t adc_out_cnt = 0;
_Bool adc_out_flag = 0;

_Bool setting = 0;


struct LED_ {
	_Bool flag;
	uint16_t threshold;
	uint16_t cnt;
	uint8_t flash_cnt;
} led1 = {0,1000,0,0}, led2 = {0,200,0,0}, led3 = {0,200,0,0};

struct YUZHI {
	uint8_t yz1;
	uint8_t yz2;
	uint8_t yz3;
} yuzhi = {30,50,70},yuzhi_temp = {5,5,5};

uint8_t level = 0;
uint8_t last_level = 0;
uint8_t height = 0;
float adc_value = 0.0;

_Bool level_change_flag = 0;
_Bool level_change_temp = 0;

uint8_t ch_select = 0;

void Delay_Ms(u32 nTime);

void key_scan(void)
{
	key_refresh();
	if(key_falling == B1)
	{
		if(setting)
		{
			setting = 0;
			yuzhi.yz1 = yuzhi_temp.yz1;
			yuzhi.yz2 = yuzhi_temp.yz2;
			yuzhi.yz3 = yuzhi_temp.yz3;
			write_at24c02(0,yuzhi.yz1);
			Delay_Ms(5);
			write_at24c02(1,yuzhi.yz2);
			Delay_Ms(5);
			write_at24c02(2,yuzhi.yz3);
			Delay_Ms(5);
		}
		else
		{
			setting = 1;
			yuzhi_temp.yz1 = yuzhi.yz1;
			yuzhi_temp.yz2 = yuzhi.yz2;
			yuzhi_temp.yz3 = yuzhi.yz3;
		}
	}
	else if(key_falling == B2)
	{
		ch_select = (ch_select + 1) % 3;
	}
	else if(key_falling == B3)
	{
		if(ch_select == 0)
		{
			if(yuzhi_temp.yz1 < 95)
				yuzhi_temp.yz1++;
		}
		else if(ch_select == 1)
		{
			if(yuzhi_temp.yz2 < 95)
				yuzhi_temp.yz2++;
		}
		else if(ch_select == 2)
		{
			if(yuzhi_temp.yz3 < 95)
				yuzhi_temp.yz3++;
		}
	}
	else if(key_falling == B4)
	{
		if(ch_select == 0)
		{
			if(yuzhi_temp.yz1 > 5)
				yuzhi_temp.yz1--;
		}
		else if(ch_select == 1)
		{
			if(yuzhi_temp.yz2 > 5)
				yuzhi_temp.yz2--;
		}
		else if(ch_select == 2)
		{
			if(yuzhi_temp.yz3 > 5)
				yuzhi_temp.yz3--;
		}
	}
}

void lcd_display(void)
{
	if(!setting)
	{
		LCD_DisplayStringLine(Line1 ,(uint8_t *)"Liquid Level        ");
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"Height: %dcm      ",height);
		LCD_DisplayStringLine(Line3 ,(uint8_t *)lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"ADC: %.2fV       ",adc_value);
		LCD_DisplayStringLine(Line5 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"Level: %d            ",level);
		LCD_DisplayStringLine(Line7 ,lcd_str);
	}
	else if(setting)
	{
		LCD_DisplayStringLine(Line1 ,(uint8_t *)"Parameter Setup     ");
				
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"Threshold 1: %dcm   ",yuzhi_temp.yz1);
		LCD_DisplayStringLine(Line3 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"Threshold 2: %dcm   ",yuzhi_temp.yz2);
		LCD_DisplayStringLine(Line5 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"Threshold 3: %dcm   ",yuzhi_temp.yz3);
		LCD_DisplayStringLine(Line7 ,lcd_str);
	}
}


//Main Body
int main(void)
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	SysTick_Config(SystemCoreClock/1000);

	Delay_Ms(200);
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	led_init();
	tim_init();
	key_init();
	i2c_init();
	adc_init();
	usart_init();
	
	adc_value = adc_get();
	height = adc_value / 3.3 * 100;
	
	yuzhi.yz1 = read_at24c02(0);
	yuzhi.yz2 = read_at24c02(1);
	yuzhi.yz3 = read_at24c02(2);
	
	if(height <= yuzhi.yz1)	level = 0;
	else if(height <= yuzhi.yz2)	level = 1;
	else if(height <= yuzhi.yz3)	level = 2;
	else	level = 3;
	last_level = level;

	while(1)
	{
		
		if(height <= yuzhi.yz1)	level = 0;
		else if(height <= yuzhi.yz2)	level = 1;
		else if(height <= yuzhi.yz3)	level = 2;
		else	level = 3;
		if(level ^ last_level)
		{
			level_change_flag = 1;
			if(level > last_level)
			{
				printf("A:H%d+L%d+U\r\n",height,level);
			}
			else if(level < last_level)
			{
				printf("A:H%d+L%d+D\r\n",height,level);
			}
		}
		last_level = level;
		
		lcd_display();
		
		if(key_flag)
		{
			key_flag = 0;
			key_scan();
		}
		
		if(adc_out_flag)
		{
			adc_out_flag = 0;
			adc_value = adc_get();
			height = adc_value / 3.3 * 100;
		}
		
		if(led1.flag)
		{
			led1.flag = 0;
			led_toggle(LD1);
		}
		
		if(level_change_flag && led2.flag)
		{
			led2.flag = 0;
			led_toggle(LD2);
			led2.flash_cnt++;
			if(led2.flash_cnt == 9)
			{
				led2.flash_cnt = 0;
				led2.cnt = 0;
				level_change_flag = 0;
				led_ctrl(LD2,DISABLE);
			}
		}
		
		if(led3.flag && rx_flag)
		{
			led3.flag = 0;
			led_toggle(LD3);
			if(++RxCounter == 10)
			{
				led_ctrl(LD3,DISABLE);
				RxCounter = 0;
				rx_flag = 0;
				led3.cnt = 0;
			}
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
		if(++key_cnt >= 10)
		{
			key_cnt = 0;
			key_flag = 1;
		}
		
		if(++adc_out_cnt >= 1000)
		{
			adc_out_cnt = 0;
			adc_out_flag = 1;
		}
		
//		if(rx_flag && ++rx_idle_cnt >= 50)
//		{
//			rx_idle_cnt = 0;
//			rx_flag = 0;
//			printf("%s\r\n",RxBuffer);
//			RxCounter = 0;
//			memset(RxBuffer,0,sizeof(RxBuffer));
//		}
		
		if(++led1.cnt >= led1.threshold)
		{
			led1.cnt = 0;
			led1.flag = 1;
		}
		
		if(++led2.cnt >= led2.threshold)
		{
			led2.cnt = 0;
			led2.flag = 1;
		}
		
		if(++led3.cnt >= led3.threshold)
		{
			led3.cnt = 0;
			led3.flag = 1;
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
	uint8_t temp;
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
    temp = USART_ReceiveData(USART2);
		if(temp == 'C')
		{
			printf("C:H%d+L%d\r\n",height,level);
			rx_flag = 1;
			RxCounter = 0;
		}
		else if(temp == 'S')
		{
			printf("S:TL%d+TM%d+TH%d\r\n",yuzhi.yz1,yuzhi.yz2,yuzhi.yz3);
			rx_flag = 1;
			RxCounter = 0;
		}
  }
}
