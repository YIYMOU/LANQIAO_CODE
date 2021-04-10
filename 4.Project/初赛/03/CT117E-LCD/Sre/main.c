#include "stm32f10x.h"
#include "lcd.h"
#include "i2c.h"
#include "key.h"
#include "led.h"
#include "tim.h"
#include "pwm.h"
#include "input_capture.h"
#include "string.h"
#include "stdio.h"
#include "usart.h"

#define			LOCAL_SETTING					0
#define			SERIAL_SETTING				1

#define			PA1										0
#define			PA2										1

u32 TimingDelay = 0;

uint8_t key_cnt = 0;

_Bool led_flag = 0;
uint16_t led_cnt = 0;

uint8_t lcd_str[20];
uint8_t lcd_str_temp[20];

uint8_t PA1_Multiplier = 2;
uint8_t PA2_Multiplier = 2;

uint32_t PA1_Fre = 0;
uint32_t PA2_Fre = 0;

_Bool input_channel = PA1;

_Bool setting_mode = LOCAL_SETTING;

union ID {
	uint32_t id;
	uint8_t str[8];
}	My_ID = {0};


struct USART_RX {
	uint8_t RxCounter;
	_Bool RxFlag;
	uint8_t RxCnt;
	_Bool	RxIdle;
} RxStruct = {0,0,0,0};

uint8_t RxBuffer[20];

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
		if(setting_mode == LOCAL_SETTING)
		{
			setting_mode = SERIAL_SETTING;
		}
		else
		{
			setting_mode = LOCAL_SETTING;
		}
	}
	else if(key_falling == B2 && setting_mode == LOCAL_SETTING)
	{
		if(input_channel == PA1)
		{
			input_channel = PA2;
		}
		else
		{
			input_channel = PA1;
		}
	}
	else if(key_falling == B3 && setting_mode == LOCAL_SETTING)
	{
		if(input_channel == PA1)
		{
			if(PA1_Multiplier != 1)
				PA1_Multiplier--;
		}
		else
		{
			if(PA2_Multiplier != 1)
				PA2_Multiplier--;
		}
	}
	else if(key_falling == B4 && setting_mode == LOCAL_SETTING)
	{
		if(input_channel == PA1)
		{
			if(PA1_Multiplier != 10)
				PA1_Multiplier++;
		}
		else
		{
			if(PA2_Multiplier != 10)
				PA2_Multiplier++;
		}
	}
}

void lcd_proc(void)
{
	memset(lcd_str,0,sizeof(lcd_str));
	sprintf((char*)lcd_str,"N(1): %d   ",PA1_Multiplier);
	LCD_DisplayStringLine(Line3 ,lcd_str);
	
	memset(lcd_str,0,sizeof(lcd_str));
	sprintf((char*)lcd_str,"Channel(2): %dHz    ",TIM2Freq_IC3);
	LCD_DisplayStringLine(Line5 ,lcd_str);
	
	memset(lcd_str,0,sizeof(lcd_str));
	sprintf((char*)lcd_str,"N(2): %d   ",PA2_Multiplier);
	LCD_DisplayStringLine(Line7 ,lcd_str);
	
	memset(lcd_str,0,sizeof(lcd_str));
	sprintf((char*)lcd_str,"%20d",input_channel + 1);
	LCD_DisplayStringLine(Line9 ,lcd_str);
	
	memset(lcd_str,0,sizeof(lcd_str));
	sprintf((char*)lcd_str,"Channel(1): %dHz    ",TIM2Freq_IC2);
	LCD_DisplayStringLine(Line1 ,lcd_str);
}

void led_proc(void)
{
	if(input_channel == PA1)
	{
		led_ctrl(LD1,ENABLE);
		led_ctrl(LD2,DISABLE);
	}
	else
	{
		led_ctrl(LD2,ENABLE);
		led_ctrl(LD1,DISABLE);
	}
	
	if(setting_mode == SERIAL_SETTING)
	{
		led_ctrl(LD3,ENABLE);
	}
	else
	{
		led_ctrl(LD3,DISABLE);
	}
}

void eeprom_proc(void)
{
	static uint8_t PA1_Multiplier_Pre = 0;
	static uint8_t PA2_Multiplier_Pre = 0;
	if(PA1_Multiplier_Pre != PA1_Multiplier)
	{
		eeprom_write(1,PA1_Multiplier);
		Delay_Ms(5);
		PA1_Multiplier_Pre = PA1_Multiplier;
	}
	if(PA2_Multiplier_Pre != PA2_Multiplier)
	{
		eeprom_write(2,PA2_Multiplier);
		Delay_Ms(5);
		PA2_Multiplier_Pre = PA2_Multiplier;
	}
}

void pwm_output_proc(void)
{
	static _Bool flag1 = 0;
	static _Bool flag2 = 0;
	if(TIM2Freq_IC2 > 50 && TIM2Freq_IC2 < 50000)
	{
		if(flag1)
		{
			GPIO_InitTypeDef GPIO_InitStructure;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
			flag1 = 0;
		}
		CCR1_Val = 2000000 / (TIM2Freq_IC2 * PA1_Multiplier);
	}
	else
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIOA->ODR &= ~GPIO_Pin_6;
		flag1 = 1;
	}
	
	if(TIM2Freq_IC3 > 50 && TIM2Freq_IC3 < 50000)
	{
		if(flag2)
		{
			GPIO_InitTypeDef GPIO_InitStructure;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
			flag2 = 0;
		}
		CCR2_Val = 2000000 / (TIM2Freq_IC3 * PA2_Multiplier);
	}
	else
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIOA->ODR &= ~GPIO_Pin_7;
		flag2 = 1;
	}
	
}

void check(void)
{
	uint8_t i ;
	for(i = 0; i < 8; i++)
	{
		My_ID.str[i] = eeprom_read(0x20 + i);
	}
	
	if(My_ID.id == 1375132689)
	{
		PA1_Multiplier = eeprom_read(1);
		PA2_Multiplier = eeprom_read(2);
	}
	else
	{
		My_ID.id = 1375132689;
		for(i = 0; i < 8; i++)
		{
			eeprom_write(0x20 + i, My_ID.str[i]);
			Delay_Ms(5);
		}
		eeprom_write(1,PA1_Multiplier);
		Delay_Ms(5);
		eeprom_write(2,PA2_Multiplier);
		Delay_Ms(5);
	}
}

void rx_proc(void)
{
	if(RxStruct.RxIdle)
	{
		RxStruct.RxIdle = 0;
		
		// LCD_DisplayStringLine(Line8,(uint8_t*)RxBuffer);
		
//		memset(lcd_str,0,sizeof(lcd_str));
//		sprintf((char*)lcd_str,"%s        ",RxBuffer);
//		LCD_DisplayStringLine(Line8 ,lcd_str);
//		
//		memset(lcd_str,0,sizeof(lcd_str));
//		sprintf((char*)lcd_str,"cnt:%d   ",RxStruct.RxCounter);
//		LCD_DisplayStringLine(Line0 ,lcd_str);
		// SET:x:yy
		// 01234567
		if(setting_mode == SERIAL_SETTING && RxBuffer[0] == 'S' && RxBuffer[1] == 'E' && RxBuffer[2] == 'T' && RxBuffer[3] == ':' && RxBuffer[5] == ':' && RxBuffer[4] >= '1' && RxBuffer[4] <= '2')
		{
			if(RxStruct.RxCounter == 7 && RxBuffer[6] >= '2' && RxBuffer[6] <= '9')
			{
				if(RxBuffer[4] == '2')
				{
					input_channel = PA2;
					PA2_Multiplier = RxBuffer[6] - '0';
				}
				else
				{
					input_channel = PA1;
					PA1_Multiplier = RxBuffer[6] - '0';
				}
			}
			else if(RxStruct.RxCounter == 8 && RxBuffer[7] == '0' && RxBuffer[6] <= '1')
			{
				if(RxBuffer[4] == '2')
				{
					input_channel = PA2;
					PA2_Multiplier = 10;
				}
				else
				{
					input_channel = PA1;
					PA1_Multiplier = 10;
				}
			}
		}
		
//		
		memset(RxBuffer,0,sizeof(RxBuffer));
		RxStruct.RxCounter = 0;
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
	pwm_init();
	input_capture_init();
	usart_init();
	
	check();
	
	while(1)
	{
		
		pwm_output_proc();
		
		eeprom_proc();
		
		lcd_proc();
		
		led_proc();
		
		rx_proc();
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
		
		if(RxStruct.RxFlag && ++RxStruct.RxCnt == 50)
		{
			RxStruct.RxCnt = 0;
			RxStruct.RxFlag = 0;
			RxStruct.RxIdle = 1;
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
    RxBuffer[RxStruct.RxCounter++] = USART_ReceiveData(USART2);
		RxStruct.RxFlag = 1;
		RxStruct.RxCnt = 0;
  }
}

