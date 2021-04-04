#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "tim.h"
#include "i2c.h"
#include "usart.h"
#include "rtc.h"
#include "adc.h"

#define ON		1
#define OFF 	0

_Bool Tx_Flag = 1;

_Bool led_on = 1;

_Bool setting_flag = 0;


struct ALARM
{
	uint8_t h;
	uint8_t m;
	uint8_t s;
}ALARM_TIME = {0,0,0},Time_Setting = {0,0,0};

_Bool rx_flag = 0;

_Bool led_state = 0;

u32 TimingDelay = 0;

uint16_t led_cnt = 0;
_Bool led_flag = 0;

_Bool key_flag = 0;
uint8_t key_cnt = 0;

uint8_t lcd_str[20];
float k = 0.1;

float adc_value = 0.0;

uint8_t setting_ch = 0;

void Delay_Ms(u32 nTime);

void key_scan(void)
{
	key_refresh();
	
	if(key_falling == B1)
	{
		led_on = !led_on;
	}
	else if(key_falling == B2)
	{
		if(setting_flag)
		{
			setting_flag = 0;
			ALARM_TIME.h = Time_Setting.h;
			ALARM_TIME.m = Time_Setting.m;
			ALARM_TIME.s = Time_Setting.s;
		}
		else
		{
			setting_flag = 1;
		}
	}
	else if(key_falling == B3)
	{
		if(setting_flag)
		{
			setting_ch = (setting_ch + 1) % 3;
		}
	}
	else if(key_falling == B4)
	{
		if(setting_flag)
		{
			if(setting_ch == 0)
			{
				Time_Setting.h = (Time_Setting.h + 1) % 24;
			}
			else if(setting_ch == 1)
			{
				Time_Setting.m = (Time_Setting.m + 1) % 60;
			}
			else if(setting_ch == 2)
			{
				Time_Setting.s = (Time_Setting.s + 1) % 60;
			}
		}
	}
}

void lcd_handler(void)
{
	if(!setting_flag)
	{
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char * )lcd_str,"V1: %.2fV         ",adc_value);
		LCD_DisplayStringLine(Line1 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char * )lcd_str,"k: %.1f         ",k);
		LCD_DisplayStringLine(Line3 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char * )lcd_str,"LED: %s         ",(led_state == ON)?"ON":"OFF");
		LCD_DisplayStringLine(Line5 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char * )lcd_str,"T:%02d-%02d-%02d         ",THH,TMM,TSS);
		LCD_DisplayStringLine(Line7 ,lcd_str);
			
		LCD_DisplayStringLine(Line9 ,(uint8_t *)"                   1");
	}
	else
	{
		LCD_DisplayStringLine(Line1 ,(uint8_t *)"                    ");
		LCD_DisplayStringLine(Line3 ,(uint8_t *)"       Setting      ");
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char * )lcd_str,"      %02d-%02d-%02d      ",Time_Setting.h,Time_Setting.m,Time_Setting.s);
		LCD_DisplayStringLine(Line5 ,lcd_str);
		
		LCD_DisplayStringLine(Line7 ,(uint8_t *)"                    ");
		LCD_DisplayStringLine(Line9 ,(uint8_t *)"                   2");
	}
	
	/* 不可以用下面这种方法
	 * memset(lcd_str,0,sizeof(lcd_str));
	 * sprintf((char * )lcd_str,"%20d",1);
	 * LCD_DisplayStringLine(Line7 ,lcd_str);
	 */
}

void usart_handler(void)
{
	if(RxBuffer[0] == 'k' && RxBuffer[1] == '0' && \
		RxBuffer[2] == '.' && RxBuffer[3] <= '9' 	&& \
		RxBuffer[3] >= '1')
	{
		k = 0.1 * (RxBuffer[3] - '0');
		printf("ok\n");
		write_at24c02(0,(uint8_t)(k * 10));
		Delay_Ms(5);
	}
	
//	printf("%s\n",RxBuffer);
	memset(RxBuffer,0,sizeof(RxBuffer));
	RxCounter = 0;
}

void rtc_handler(void)
{
	Time_Display();
	
	if(THH == ALARM_TIME.h && TMM == ALARM_TIME.m && TSS == ALARM_TIME.s)
	{
		if(Tx_Flag)
		{
			Tx_Flag = 0;
			printf("%.2f+%.1f+%02d%02d%02d\n",adc_value,k,THH,TMM,TSS);
		}
	}
	else
	{
		Tx_Flag = 1;
	}
}

//Main Body
int main(void)
{
	uint8_t temp;
	/* Configure the NVIC Preemption Priority Bits */  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	SysTick_Config(SystemCoreClock/1000);

	Delay_Ms(200);
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	led_init();
	key_init();
	tim_init();
	i2c_init();
	usart_init();
	rtc_init();
	Time_Adjust(23,59,55);
	adc_init();
	
	printf("\r");
	
	temp = read_at24c02(0);
	if(temp >= 1 && temp <= 9)
	{
		k = temp / 10.0;
	}
	else
		k = 0.1;
	
	while(1)
	{
		rtc_handler();
		
		lcd_handler();
		
		adc_value = adc_get();

		if(key_flag)
		{
			key_flag = 0;
			key_scan();
		}
		
		if(rx_flag)
		{
			rx_flag = 0;
			usart_handler();
		}
		
		if(led_flag)
		{
			led_flag = 0;
			led_toggle(LD1);
		}
		
		if(!led_on)
		{
			led_ctrl(LD1,DISABLE);
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
		if(led_on && adc_value > 3.3 * k && ++led_cnt >= 200)
		{
			led_cnt = 0;
			led_flag = 1;
		}
		if(++key_cnt >= 10)
		{
			key_cnt = 0;
			key_flag = 1;
		}
		if(UsartFlag && ++usart_ticker >= 50)
		{
			usart_ticker = 0;
			UsartFlag = 0;
			rx_flag = 1;
			RxCounter = 0;
		}
  }
}
