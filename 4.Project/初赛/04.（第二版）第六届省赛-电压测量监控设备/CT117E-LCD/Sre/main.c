#include "stm32f10x.h"
#include "lcd.h"
#include "i2c.h"
#include "key.h"
#include "led.h"
#include "tim.h"
#include "string.h"
#include "stdio.h"
#include "rtc.h"
#include "adc.h"
#include "usart.h"


#define			SETTING					1
#define			DATA						0

#define			OFF							0
#define			ON							1

u32 TimingDelay = 0;

uint8_t key_cnt = 0;

_Bool led_flag = 0;
uint16_t led_cnt = 0;

uint8_t lcd_str[20];

float V1 = 0.0;

float k = 0.1;
uint8_t k_fre = 0;

_Bool interface = DATA;

_Bool led_state = ON;

uint32_t THH_temp = 23,TMM_temp = 59,TSS_temp = 55;
uint32_t THH_alarm = 0,TMM_alarm = 0,TSS_alarm = 0;
_Bool alarm_flag = 0;
uint8_t select = 0;

union ID {
	uint32_t id;
	uint8_t str[8];
} My_ID = {0};

uint8_t RxBuffer[20];
uint8_t RxCounter = 0;
_Bool RxFlag = 0;
uint8_t RxCnt = 0;
_Bool RxCntFlag = 0;

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
		led_state = !led_state;
	}
	else if(key_falling == B2)
	{
		if(interface == SETTING)
		{
			interface = DATA;
			THH_alarm = THH_temp;
			TMM_alarm = TMM_temp;
			TSS_alarm = TSS_temp;
		}
		else
		{
			interface = SETTING;
			select = 0;
			THH_temp = THH;
			TMM_temp = TMM;
			TSS_temp = TSS;
		}
	}
	else if(interface == SETTING && key_falling == B3)
	{
		select = (select + 1) % 3;
	}
	else if(key_falling == B4)
	{
		if(select == 0)
		{
			THH_temp = (THH_temp + 1) % 24;
		}
		else if(select == 1)
		{
			TMM_temp = (TMM_temp + 1) % 60;
		}
		else
		{
			TSS_temp = (TSS_temp + 1) % 60;
		}
	}
}

void lcd_proc(void)
{
	if(interface == DATA)
	{
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"  V1:%.2fV      ",V1);
		LCD_DisplayStringLine(Line1 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"  k:%.1f        ",k);
		LCD_DisplayStringLine(Line3 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"  LED:%s      ",led_state == OFF ? "OFF":"ON");
		LCD_DisplayStringLine(Line5 ,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"  T:%02d:%02d:%02d      ",THH,TMM,TSS);
		LCD_DisplayStringLine(Line7 ,lcd_str);
		
		LCD_DisplayStringLine(Line9 ,(unsigned char *)"                  1");
	}
	else
	{
		LCD_DisplayStringLine(Line1 ,(unsigned char *)"                   ");
		
		LCD_DisplayStringLine(Line3 ,(unsigned char *)"       SETTING     ");
		
		LCD_DisplayStringLine(Line5 ,(unsigned char *)"                   ");
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"       %02d:%02d:%02d      ",THH_temp,TMM_temp,TSS_temp);
		LCD_DisplayStringLine(Line7 ,lcd_str);
		
		LCD_DisplayStringLine(Line9 ,(unsigned char *)"                  2");
	}
}

void alarm_check(void)
{
	if(THH == THH_alarm && TMM == TMM_alarm && TSS == TSS_alarm && alarm_flag == 0)
	{
		alarm_flag = 1;
		printf("%.2f+%.1f+%02d%02d%02d\r\n",V1,k,THH,TMM,TSS);
	}
	if(THH != THH_alarm || TMM != TMM_alarm || TSS != TSS_alarm)
	{
		alarm_flag = 0;
	}
}

void led_proc(void)
{
	if(led_state == OFF || 3.3 * k > V1)
	{
		led_ctrl(LD1,DISABLE);
	}
	else if(led_flag && led_state == ON)
	{
		led_flag =  0;
		led_toggle(LD1);
	}
}

void adc_proc(void)
{
	V1 = adc_refresh();
}

void check_id(void)
{
	uint8_t i;
	for(i = 0;i < 8;i++)
	{
		My_ID.str[i] = eeprom_read(0x30 + i);
	}
	if(My_ID.id == 1375132689)
	{
		k = eeprom_read(0) * 0.1;
	}
	else
	{
		My_ID.id = 1375132689;
		for(i = 0;i < 8;i++)
		{
			eeprom_write(0x30 + i,My_ID.str[i]);
			Delay_Ms(5);
		}
		eeprom_write(0,1);
		Delay_Ms(5);
	}
}

void eeprom_proc(void)
{
	if((uint8_t)(k * 10) != k_fre)
	{
		eeprom_write(0,(uint8_t)(k * 10));
		Delay_Ms(5);
		k_fre = (uint8_t)(k * 10);
	}
}

void usart_proc(void)
{
	if(RxCntFlag)
	{
		RxCntFlag = 0;
		
		// k0.1\n
		// 0123
		if(RxBuffer[0] == 'k' && RxBuffer[1] == '0' && RxBuffer[2] == '.' && RxBuffer[3] <= '9' && RxBuffer[3] >= '1' && RxBuffer[4] == '\r' && RxBuffer[5] == '\n')
		{
			k = (RxBuffer[3] - '0') * 0.1;
			printf("ok\r\n");
		}	
		
//		memset(lcd_str,0,sizeof(lcd_str));
//		sprintf((char*)lcd_str,"%-20.20s",RxBuffer);
//		LCD_DisplayStringLine(Line0 ,lcd_str);
		
		
		memset(RxBuffer,0,sizeof(RxBuffer));
		RxCounter = 0;
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
	rtc_init();
	Time_Adjust(23,59,55);
	adc_init();
	i2c_init();
	check_id();
	usart_init();
	
	printf("\r");
	
	while(1)
	{
		adc_proc();
		
		Time_Display();
		
		alarm_check();
		
		lcd_proc();
		
		usart_proc();
		
		led_proc();
		
		eeprom_proc();
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
		
		if(led_state && ++led_cnt == 200)
		{
			led_cnt = 0;
			led_flag = 1;
		}
		
		if(RxFlag && ++RxCnt == 50)
		{
			RxFlag = 0;
			RxCnt = 0;
			RxCntFlag = 1;
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
		RxFlag = 1;
		RxCnt = 0;
  }
}
