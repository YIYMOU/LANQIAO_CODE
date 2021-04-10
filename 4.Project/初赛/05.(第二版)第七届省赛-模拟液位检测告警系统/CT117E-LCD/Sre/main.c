#include "stm32f10x.h"
#include "lcd.h"
#include "i2c.h"
#include "key.h"
#include "led.h"
#include "adc.h"
#include "tim.h"
#include "string.h"
#include "stdio.h"
#include "usart.h"

#define				SETTING							1
#define				DATA								0

u32 TimingDelay = 0;

uint8_t RxBuffer[20];
uint8_t RxCounter = 0x00;
_Bool RxFlag = 0;
uint8_t RxCnt = 0;
_Bool RxCntFlag = 0;

uint16_t usart_cnt = 0;
_Bool usart_flag = 0;
_Bool usart_cnt_flag = 0;

uint8_t key_cnt = 0;

_Bool led_flag = 0;
uint16_t led_cnt = 0;

uint8_t lcd_str[20];

uint8_t Height = 0;

uint8_t Level = 0;
uint8_t Level_pre = 0;
uint16_t Level_cnt = 0;
_Bool Level_flag = 0;
_Bool Level_cnt_flag = 0;

float	ADC = 1.65;
uint16_t adc_cnt = 0;
_Bool adc_flag = 1;

_Bool first_boot = 1;

uint8_t select = 0;

_Bool interface = DATA;

struct THRESHOLD {
	uint8_t para1;
	uint8_t para2;
	uint8_t para3;
} Threshold = {30,50,70}, Threshold_temp = {0,0,0};

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
		if(interface == SETTING)
		{
			if(Threshold.para1 < Threshold.para2 && Threshold.para2 < Threshold.para3)
				interface = DATA;
		}
		else
		{
			interface = SETTING;
			select = 0;
		}
	}
	else if(interface == SETTING && key_falling == B2)
	{
		select = (select + 1) % 3;
	}
	else if(key_falling == B3)
	{
		if(select == 0 && Threshold.para1 < 95)
		{
			Threshold.para1 += 5;
		}
		else if(select == 1 && Threshold.para2 < 95)
		{
			Threshold.para2 += 5;
		}
		else if(select == 2 && Threshold.para3 < 95)
		{
			Threshold.para3 += 5;
		}
	}
	else if(key_falling == B4)
	{
		if(select == 0 && Threshold.para1 > 5)
		{
			Threshold.para1 -= 5;
		}
		else if(select == 1 && Threshold.para2 > 5)
		{
			Threshold.para2 -= 5;
		}
		else if(select == 2 && Threshold.para3 > 5)
		{
			Threshold.para3 -= 5;
		}
	}
}

void Highlight(u8 Line, u8 *ptr)
{
	u32 i = 0;
	u16 refcolumn = 319;//319;
	if((Line == Line3 && select == 0) || (Line == Line5 && select == 1) || (Line == Line7 && select == 2))
		LCD_SetTextColor(Green);
	while ((*ptr != 0) && (i < 20))	 //	20
	{
		LCD_DisplayChar(Line, refcolumn, *ptr);
		refcolumn -= 16;
		ptr++;
		i++;
	}
	
	LCD_SetTextColor(White);
}

void hight_proc(void)
{
	if(adc_flag)
	{
		adc_flag = 0;
		ADC = adc_get();
		Height = (ADC / 3.3 * 100 + 0.5);
	}
}

void lcd_proc(void)
{
	
	
	if(interface == DATA)
	{
		
		LCD_DisplayStringLine(Line1 ,(unsigned char *)"    Liquid Level   ");
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"  Height: %d      ",Height);
		LCD_DisplayStringLine(Line3 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"  ADC: %.2f       ",ADC);
		LCD_DisplayStringLine(Line5 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"  Level: %d       ",Level);
		LCD_DisplayStringLine(Line7 ,lcd_str);
	}
	else
	{

		LCD_DisplayStringLine(Line1 ,(unsigned char *)"   Parameter Setup ");
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"  Threshold 1: %d  ",Threshold.para1);
		Highlight(Line3 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"  Threshold 2: %d  ",Threshold.para2);
		Highlight(Line5 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"  Threshold 3: %d  ",Threshold.para3);
		Highlight(Line7 ,lcd_str);
	}
}

void level_check(void)
{
	if(Height > Threshold.para3)
	{
		Level = 3;
	}
	else if(Height > Threshold.para2)
	{
		Level = 2;
	}
	else if(Height > Threshold.para1)
	{
		Level = 1;
	}
	else
	{
		Level = 0;
	}
	
	if(Level != Level_pre)
	{
		if(first_boot)
		{
			first_boot = 0;
		}
		else
		{
			Level_flag  = 1;
			Level_cnt = 0;
			printf("A:H%d+L%d+%c\r\n",Height,Level,Level > Level_pre ? 'U' : 'D');
		}
		Level_pre = Level;
	}
}

void led_proc(void)
{
	if(led_flag)
	{
		led_flag =  0;
		led_toggle(LD1);
	}
	
	if(Level_flag)
	{
		if(Level_cnt_flag)
		{
			Level_cnt_flag = 0;
			led_toggle(LD2);
		}
	}
	else
	{
		led_ctrl(LD2,DISABLE);
	}
	
	if(usart_flag)
	{
		if(usart_cnt_flag)
		{
			usart_cnt_flag = 0;
			led_toggle(LD3);
		}
	}
	else
	{
		led_ctrl(LD3,DISABLE);
	}
}

void eeprom_proc(void)
{
	if(Threshold_temp.para1 != Threshold.para1)
	{
		eeprom_write(0,Threshold.para1);
		Delay_Ms(5);
		Threshold_temp.para1 = Threshold.para1;
	}
	if(Threshold_temp.para2 != Threshold.para2)
	{
		eeprom_write(1,Threshold.para2);
		Delay_Ms(5);
		Threshold_temp.para2 = Threshold.para2;
	}
	if(Threshold_temp.para3 != Threshold.para3)
	{
		eeprom_write(2,Threshold.para3);
		Delay_Ms(5);
		Threshold_temp.para3 = Threshold.para3;
	}
}

void check(void)
{
	union ID {
		uint32_t ID;
		uint8_t str[8];
	}	My_ID;
	uint8_t i = 0;
	for(i = 0; i < 8; i++)
	{
		My_ID.str[i] = eeprom_read(0x40 + i);
	}
	
	if(My_ID.ID == 1375132689)
	{
		Threshold.para1 = eeprom_read(0);
		Threshold.para2 = eeprom_read(1);
		Threshold.para3 = eeprom_read(2);
		if(!(Threshold.para1 % 5 == 0 && Threshold.para1 <= 95 && Threshold.para1 >= 5))
		{
			Threshold.para1 = 30;
		}
		if(!(Threshold.para2 % 5 == 0 && Threshold.para2 <= 95 && Threshold.para2 >= 5))
		{
			Threshold.para2 = 50;
		}
		if(!(Threshold.para3 % 5 == 0 && Threshold.para3 <= 95 && Threshold.para3 >= 5))
		{
			Threshold.para3 = 70;
		}
	}
	else
	{
		My_ID.ID = 1375132689;
		for(i = 0; i < 8; i++)
		{
			eeprom_write(0x40 + i,My_ID.str[i]);
			Delay_Ms(5);
		}
		eeprom_write(0,30);
		Delay_Ms(5);
		eeprom_write(0,50);
		Delay_Ms(5);
		eeprom_write(0,70);
		Delay_Ms(5);
	}
}

void usart_proc(void)
{
	if(RxCntFlag)
	{
		RxCntFlag = 0;
		
		if(RxBuffer[0] == 'C' && RxCounter == 1)
		{
			printf("C:H%d+L%d\r\n",Height,Level);
			usart_flag = 1;
			usart_cnt = 0;
		}
		else if(RxBuffer[0] == 'S' && RxCounter == 1)
		{
			printf("S:TL%d+TM%d+TH%d\r\n",Threshold.para1,Threshold.para2,Threshold.para3);
			usart_flag = 1;
			usart_cnt = 0;
		}
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"%-20.20s",RxBuffer);
		LCD_DisplayStringLine(Line0 ,lcd_str);
		
		memset(RxBuffer,0,sizeof(RxBuffer));
		RxCounter = 0;
		
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
	usart_init();
	check();
	
	printf("\r");
	
	while(1)
	{
		if(interface != SETTING)
		{
			hight_proc();
		
			level_check();
			
			led_proc();
			
			eeprom_proc();
		}
		else
		{
			led_ctrl(LD1,DISABLE);
			led_ctrl(LD2,DISABLE);
			led_ctrl(LD3,DISABLE);
		}
		
		lcd_proc();
		
		usart_proc();
		
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
		
		if(++led_cnt == 1000)
		{
			led_cnt = 0;
			led_flag = 1;
		}
		
		if(Level_flag && (++Level_cnt) % 200 == 0)
		{
			if(Level_cnt == 2000)
			{
				Level_cnt = 0;
				Level_flag = 0;
				Level_cnt_flag = 0;
			}
			else
				Level_cnt_flag = 1;
		}
		
		
		if(usart_flag && (++usart_cnt) % 200 == 0)
		{
			if(usart_cnt == 2000)
			{
				usart_cnt = 0;
				usart_flag = 0;
				usart_cnt_flag = 0;
			}
			else
				usart_cnt_flag = 1;
		}
		
		if(RxFlag && ++RxCnt == 50)
		{
			RxCnt = 0;
			RxFlag = 0;
			RxCntFlag = 1;
		}
		
		if(!adc_flag && ++adc_cnt == 990)
		{
			adc_flag = 1;
			adc_cnt = 0;
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

		RxCnt = 0;
		RxFlag = 1;
		
    if(RxCounter == 20)
    {
			memset(RxBuffer,0,sizeof(RxBuffer));
			RxCounter = 0;
    }
		
		
  }
}
