#include "stm32f10x.h"
#include "lcd.h"
#include "i2c.h"
#include "key.h"
#include "led.h"
#include "tim.h"
#include "string.h"
#include "stdio.h"
#include "pwm.h"
#include "rtc.h"
#include "usart.h"

u32 TimingDelay = 0;

uint8_t PA1_Duty = 0;
uint8_t PA2_Duty = 0;

FunctionalState PA1_STATE = DISABLE;
FunctionalState PA2_STATE = DISABLE;

uint8_t key_cnt = 0;

_Bool led_flag = 0;
uint16_t led_cnt = 0;

uint8_t lcd_str[20];


uint8_t RxBuffer[20];
uint8_t RxCounter = 0x00; 

_Bool RxFlag = 0;
uint8_t RxCnt = 0;
_Bool RxIDLE = 0;

union EEPROM_DATA {
	uint32_t ID;
	uint8_t str[8];
} My_ID;

struct TIME {
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint8_t out_time;
	uint16_t sec_cnt;
	_Bool out_flag;
	_Bool cnt_start;
	_Bool rx_flag;
	uint8_t ch;
} setting = {0,0,0,0,0,0,0,0,0};

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
		if(PA1_STATE)
		{
			PA1_STATE = DISABLE;
			pwm_init(PA1_Duty,PA1_STATE,PA2_Duty,PA2_STATE);
		}
		else
		{
			PA1_STATE = ENABLE;
			PA2_STATE = DISABLE;
			pwm_init(PA1_Duty,PA1_STATE,PA2_Duty,PA2_STATE);
			usart_init();
		}
	}
	else if(key_falling == B2)
	{
		PA1_Duty = (PA1_Duty + 1) % 11;
		eeprom_write(0,PA1_Duty);
		pwm_init(PA1_Duty,PA1_STATE,PA2_Duty,PA2_STATE);
	}
	else if(key_falling == B3)
	{
		if(PA2_STATE)
		{
			PA2_STATE = DISABLE;
			pwm_init(PA1_Duty,PA1_STATE,PA2_Duty,PA2_STATE);
			usart_init();
		}
		else
		{
			USART_DeInit(USART2);
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);  
			PA2_STATE = ENABLE;
			PA1_STATE = DISABLE;
			pwm_init(PA1_Duty,PA1_STATE,PA2_Duty,PA2_STATE);
		}
	}
	else if(key_falling == B4)
	{
		PA2_Duty = (PA2_Duty + 1) % 11;
		eeprom_write(1,PA2_Duty);
		pwm_init(PA1_Duty,PA1_STATE,PA2_Duty,PA2_STATE);
	}
}

//Main Body
int main(void)
{
	uint8_t i;
	
  /* Configure the NVIC Preemption Priority Bits */  
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
	pwm_init(PA1_Duty,PA1_STATE,PA2_Duty,PA2_STATE);
	rtc_init();
	Time_Adjust(23,59,50);
	usart_init();
	
	for(i = 0;i < 8;i++)
	{
		My_ID.str[i] = eeprom_read(0x10 + i);
	}
	
	if(My_ID.ID == 1375132689)
	{
		PA1_Duty = eeprom_read(0);
		PA2_Duty = eeprom_read(1);
	}
	else
	{
		My_ID.ID = 1375132689;
		for(i = 0;i < 8;i++)
		{
			eeprom_write(0x10 + i,My_ID.str[i]);
		}
	}
	
	
	while(1)
	{
		Time_Get();
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"PWM-PA1: %02d%%  ",PA1_Duty * 10);
		LCD_DisplayStringLine(Line0 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"PWM-PA2: %02d%%  ",PA2_Duty * 10);
		LCD_DisplayStringLine(Line2 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"Time: %02d: %02d: %02d   ",THH,TMM,TSS);
		LCD_DisplayStringLine(Line4 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		if(PA1_STATE && PA2_STATE)
			sprintf((char*)lcd_str,"Channel: PA1,PA2     ");
		else if(PA1_STATE)
			sprintf((char*)lcd_str,"Channel: PA1     ");
		else if(PA2_STATE)
			sprintf((char*)lcd_str,"Channel: PA2     ");
		else
			sprintf((char*)lcd_str,"Channel: NONE     ");
		LCD_DisplayStringLine(Line6 ,lcd_str);
		
		LCD_DisplayStringLine(Line8 ,(unsigned char *)"Command:   ");
		
		if(PA1_STATE && PA2_STATE)
			led_ctrl(LD1 | LD2,ENABLE);
		else if(PA1_STATE)
		{
			led_ctrl(LD1,ENABLE);
			led_ctrl(LD2,DISABLE);
		}
		else if(PA2_STATE)
		{
			led_ctrl(LD2,ENABLE);
			led_ctrl(LD1,DISABLE);
		}
		else
			led_ctrl(LD1 | LD2,DISABLE);
		
		if(RxIDLE)
		{
			RxIDLE = 0;
			
			if(RxBuffer[0] >= '0' && RxBuffer[0] <= '9' && \
				 RxBuffer[1] >= '0' && RxBuffer[1] <= '9' && \
				 RxBuffer[2] == ':' && RxBuffer[5] <= ':' && \
				 RxBuffer[3] >= '0' && RxBuffer[3] <= '9' && \
				 RxBuffer[4] >= '0' && RxBuffer[4] <= '9' && \
				 RxBuffer[6] >= '0' && RxBuffer[6] <= '9' && \
				 RxBuffer[7] >= '0' && RxBuffer[7] <= '9' && \
				 RxBuffer[8] == '-' && RxBuffer[9] == 'P' && \
				 RxBuffer[10] == 'A' && (RxBuffer[11] == '1' || \
				 RxBuffer[11] == '2') && RxBuffer[12] == '-' && \
				 RxBuffer[13] >= '1' && RxBuffer[13] <= '9' && \
				 RxBuffer[12] == '-' && RxBuffer[14] == 'S' && \
				 RxBuffer[15] == 0)
			{
				setting.rx_flag = 1;
				setting.hour = (RxBuffer[0] - '0') * 10 + (RxBuffer[1] - '0');
				setting.min = (RxBuffer[3] - '0') * 10 + (RxBuffer[4] - '0');
				setting.sec = (RxBuffer[6] - '0') * 10 + (RxBuffer[7] - '0');
				setting.out_time = RxBuffer[13] - '0';
				if(RxBuffer[11] == '1')
				{
					setting.ch = 1;
				}
				else
				{
					setting.ch = 2;
				}
			}
			
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char*)lcd_str,"%20s",RxBuffer);
			LCD_DisplayStringLine(Line9 ,lcd_str);
			memset(RxBuffer,0,sizeof(RxBuffer));
			RxCounter = 0;
		}
		
		if(setting.rx_flag && setting.hour == THH && setting.min == TMM && setting.sec == TSS)
		{
			setting.rx_flag = 0;
			setting.cnt_start = 1;
			if(setting.ch == 1)
			{
				PA1_STATE = ENABLE;
				PA2_STATE = DISABLE;
				pwm_init(PA1_Duty,PA1_STATE,PA2_Duty,PA2_STATE);
				usart_init();
			}
			else
			{
				USART_DeInit(USART2);
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);  
				PA2_STATE = ENABLE;
				PA1_STATE = DISABLE;
				pwm_init(PA1_Duty,PA1_STATE,PA2_Duty,PA2_STATE);
			}
		}
		if(setting.out_flag)
		{
			setting.out_flag = 0;
			PA1_STATE = DISABLE;
			PA2_STATE = DISABLE;
			pwm_init(PA1_Duty,PA1_STATE,PA2_Duty,PA2_STATE);
			usart_init();
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
			key_cnt =0;
			key_scan();
		}
		
		if(RxFlag && ++RxCnt == 50)
		{
			RxCnt = 0;
			RxFlag = 0;
			RxIDLE = 1;
		}
		
		if(setting.cnt_start && ++setting.sec_cnt == (1000 * setting.out_time))
		{
			setting.cnt_start = 0;
			setting.sec_cnt = 0;
			setting.out_flag = 1;
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
		RxFlag = 1;
    RxBuffer[RxCounter++] = USART_ReceiveData(USART2);
  }
}
