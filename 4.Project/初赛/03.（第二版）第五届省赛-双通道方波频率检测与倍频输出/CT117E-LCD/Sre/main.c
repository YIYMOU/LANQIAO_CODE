#include "stm32f10x.h"
#include "lcd.h"
#include "i2c.h"
#include "key.h"
#include "led.h"
#include "tim.h"
#include "pwm.h"
#include "input_capture.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"

#define			USART_MODE				1
#define			LOCAL_MODE				0

u32 TimingDelay = 0;

uint8_t key_cnt = 0;

_Bool led_flag = 0;
uint16_t led_cnt = 0;

uint8_t lcd_str[20];

uint8_t N1 = 2;
uint8_t N2 = 3;

uint8_t current_ch = 1;

_Bool mode = LOCAL_MODE;

union ID {
	uint32_t id;
	uint8_t str[8];
} My_ID;




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
		if(mode == LOCAL_MODE)
		{
			mode = USART_MODE;
		}
		else
		{
			mode = LOCAL_MODE;
		}
	}
	else if(mode == LOCAL_MODE && key_falling == B2)
	{
		current_ch = (current_ch == 1)? 2 : 1;
	}
	else if(mode == LOCAL_MODE && key_falling == B3)
	{
		if(current_ch == 1)
		{
			if(N1 != 1)
				N1--;
		}
		else
		{
			if(N2 != 1)
				N2--;
		}
	}
	else if(mode == LOCAL_MODE && key_falling == B4)
	{
		if(current_ch == 1)
		{
			if(N1 != 10)
				N1++;
		}
		else
		{
			if(N2 != 10)
				N2++;
		}
	}
}

void lcd_proc(void)
{
	memset(lcd_str,0,sizeof(lcd_str));
	sprintf((char*)lcd_str," Channel(1): %dHz   ",IC2_TIM2Freq);
	LCD_DisplayStringLine(Line1 ,lcd_str);
	
	memset(lcd_str,0,sizeof(lcd_str));
	sprintf((char*)lcd_str," N(1): %d    ",N1);
	LCD_DisplayStringLine(Line3 ,lcd_str);
	
	memset(lcd_str,0,sizeof(lcd_str));
	sprintf((char*)lcd_str," Channel(2): %dHz   ",IC3_TIM2Freq);
	LCD_DisplayStringLine(Line5 ,lcd_str);
	
	memset(lcd_str,0,sizeof(lcd_str));
	sprintf((char*)lcd_str," N(2): %d    ",N2);
	LCD_DisplayStringLine(Line7 ,lcd_str);
	
	
	memset(lcd_str,0,sizeof(lcd_str));
	sprintf((char*)lcd_str,"                  %d",current_ch);
	LCD_DisplayStringLine(Line9 ,lcd_str);
	
}

void led_proc(void)
{
	if(mode == USART_MODE)
	{
		led_ctrl(LD3,ENABLE);
	}
	else
	{
		led_ctrl(LD3,DISABLE);
	}
	
	if(current_ch == 1)
	{
		led_ctrl(LD1,ENABLE);
		led_ctrl(LD2,DISABLE);
	}
	else
	{
		led_ctrl(LD2,ENABLE);
		led_ctrl(LD1,DISABLE);
	}
}

void check(void)
{
	uint8_t i;
	for(i = 0; i < 8; i++)
		My_ID.str[i] = eeprom_read(0x90 + i);
	if(My_ID.id == 1375132689)
	{
		N1 = eeprom_read(1);
		N2 = eeprom_read(2);
	}
	else
	{
		My_ID.id = 1375132689;
		for(i = 0; i < 8; i++)
		{
			eeprom_write(0x90 + i,My_ID.str[i]);	Delay_Ms(5);
		}
		eeprom_write(1,2);	Delay_Ms(5);
		eeprom_write(2,2);	Delay_Ms(5);
	}
}

void eeprom_proc(void)
{
	static uint8_t N1_pre = 2,N2_pre = 2;
	if(N1_pre != N1)
	{
		N1_pre = N1;
		eeprom_write(1,N1);	Delay_Ms(5);
	}
	if(N2_pre != N2)
	{
		N2_pre = N2;
		eeprom_write(2,N2);	Delay_Ms(5);
	}
}

void pwm_enable(uint16_t PAx,FunctionalState NewState)
{
	if(NewState == ENABLE)
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		/* GPIOA Configuration:TIM3 Channel1, 2as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = PAx;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	else
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Pin = PAx;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
}

void usart_proc(void)
{
	if(RxIdleFlag)
	{
		uint8_t CH_select = 1,N_temp = 2;
		RxIdleFlag = 0;
		
		Buffer[0] = ch[0];
		Buffer[1] = ch[1];
		Buffer[2] = ch[2];
		//Buffer[3] = ch[3];
//		memset(lcd_str,0,sizeof(lcd_str));
//		sprintf((char*)lcd_str,"%20.20s",Buffer);
//		LCD_DisplayStringLine(Line0 ,lcd_str);
//		
//		memset(lcd_str,0,sizeof(lcd_str));
//		sscanf((const char *)Buffer,"%[SET.]%d%d",lcd_str);
//		LCD_DisplayStringLine(Line0 ,lcd_str);
		
		if(mode == USART_MODE)
		{
			if(Buffer[0] == 'S' && Buffer[1] == 'E' && Buffer[2] == 'T' && Buffer[3] == ':' && Buffer[5] == ':')
			{
				CH_select = (Buffer[4] - '0');
				if(RxNumOfReceived == 8)
				{
					N_temp = (Buffer[6] - '0') * 10 + (Buffer[7] - '0');
					if(N_temp <= 10 && N_temp >= 2)
					{
						if(CH_select == 1)
						{
							current_ch = CH_select;
							N1 = N_temp;
						}
						else if(CH_select == 2)
						{
							current_ch = CH_select;
							N2 = N_temp;
						}
					}
				}
				else if(RxNumOfReceived == 7)
				{
					N_temp = (Buffer[6] - '0');
					if(N_temp <= 9 && N_temp >= 2)
					{
						if(CH_select == 1)
						{
							current_ch = CH_select;
							N1 = N_temp;
						}
						else if(CH_select == 2)
						{
							current_ch = CH_select;
							N2 = N_temp;
						}
					}
				}
			}
		}
		
		memset(Buffer,0,sizeof(Buffer));
		RxNumOfReceived = 0;
	}
}

void pwm_proc(void)
{
//	static uint8_t N1_pre1 = 0,N2_pre1 = 0;
//	static uint8_t IC2_TIM2Freq_pre = 0,IC3_TIM2Freq_pre = 0;
//	// (N2_pre1 != N2 || IC3_TIM2Freq_pre != IC3_TIM2Freq) &&
//	// (N1_pre1 != N1 || IC2_TIM2Freq_pre != IC2_TIM2Freq) && 
	if(IC2_TIM2Freq <= 50000 && IC2_TIM2Freq >= 50 && current_ch == 1)
	{
		pwm_enable(GPIO_Pin_6,ENABLE);
//		N1_pre1 = N1;
		CCR1_Val = 1000000 / (IC2_TIM2Freq * N1);
//		IC2_TIM2Freq_pre = IC2_TIM2Freq;
	}
	else
	{
//		N1_pre1 = N1;
		pwm_enable(GPIO_Pin_6,DISABLE);
	}
	if(IC3_TIM2Freq <= 50000 && IC3_TIM2Freq >= 50 && current_ch == 2)
	{
		pwm_enable(GPIO_Pin_7,ENABLE);
//		N2_pre1 = N2;
		CCR2_Val = 1000000 / (IC3_TIM2Freq * N2);
//		IC3_TIM2Freq_pre = IC3_TIM2Freq;
	}
	else
	{
//		N2_pre1 = N2;
		pwm_enable(GPIO_Pin_7,DISABLE);
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
	usart_init();
	input_capture_init();
	pwm_init();
	pwm_enable(GPIO_Pin_6 | GPIO_Pin_7,DISABLE);
	check();
	
	while(1)
	{
		
		lcd_proc();
		
		led_proc();
		
		eeprom_proc();
		
		usart_proc();
		
		pwm_proc();
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
		
		if(RxFlag && ++RxCnt == 100)
		{
			RxFlag = 0;
			RxCnt = 0;
			RxIdleFlag = 1;
		}
  }
}
