#include "stm32f10x.h"
#include "lcd.h"
#include "key.h"
#include "led.h"
#include "tim.h"
#include "string.h"
#include "stdio.h"
#include "rtc.h"
#include "usart.h"
#include "beep.h"
#include "i2c.h"

#define 		MAIN									0
#define 		RTC_SETTING						1
#define 		ALARM_SETTING					2

#define 		Column(x)							(320 - (16 * (x - 1)))

struct TIME
{
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
}	time = {0,0,0},time_temp = {0,0,0},time_alarm = {12,0,0};

uint8_t RTC_SETTING_cnt = 0;
uint8_t ALARM_SETTING_cnt = 0;

u32 TimingDelay = 0;

uint8_t key_cnt = 0;

uint8_t interface = 0;

uint8_t lcd_str[20];
	
uint8_t para_select = 0;

uint16_t led_cnt = 0;
_Bool led_flag = 0;

uint8_t b3_cnt = 0;
uint8_t b4_cnt = 0;

uint16_t beep_cnt = 0;
_Bool beep_flag = 0;

_Bool eeprom_write_flag = 0;

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
		if(interface == MAIN)
		{
			para_select = 0;
			interface = RTC_SETTING;
			time_temp.hour = THH;
			time_temp.min = TMM;
			time_temp.sec = TSS;
		}
		else if(interface == RTC_SETTING)
		{
			RTC_SETTING_cnt++;
			eeprom_write_flag = 1;
			interface = MAIN;
			Time_Adjust(time_temp.hour,time_temp.min,time_temp.sec);
			printf("New RTC:%02d:%02d:%02d\r\n",time_temp.hour,time_temp.min,time_temp.sec);
			printf("RTC_SETTING_cnt:%d\r\n",RTC_SETTING_cnt);
			printf("ALARM_SETTING_cnt:%d\r\n",ALARM_SETTING_cnt);
		}
		else if(interface == ALARM_SETTING)
		{
			para_select = (para_select + 1) % 3;
		}
	}
	else if(key_falling == B2)
	{
		if(interface == MAIN)
		{
			para_select = 0;
			interface = ALARM_SETTING;
			time_temp.hour = time_alarm.hour;
			time_temp.min = time_alarm.min;
			time_temp.sec = time_alarm.sec;
		}
		else if(interface == ALARM_SETTING)
		{
			ALARM_SETTING_cnt++;
			eeprom_write_flag = 1;
			interface = MAIN;
			time_alarm.hour = time_temp.hour;
			time_alarm.min = time_temp.min;
			time_alarm.sec = time_temp.sec;
			printf("New Alarm:%02d:%02d:%02d\r\n",time_alarm.hour,time_alarm.min,time_alarm.sec);
			printf("RTC_SETTING_cnt:%d\r\n",RTC_SETTING_cnt);
			printf("ALARM_SETTING_cnt:%d\r\n",ALARM_SETTING_cnt);
		}
		else if(interface == RTC_SETTING)
		{
			para_select = (para_select + 1) % 3;
		}
	}
	else if(key_falling == B3)
	{
		if(interface == RTC_SETTING || interface == ALARM_SETTING)
		{
LABAL1:switch(para_select)
			{
				case 0: 
						if(time_temp.hour != 59)
							time_temp.hour++;
				break;
				case 1: 
						if(time_temp.min != 59)
							time_temp.min++;
				break;
				case 2: 
						if(time_temp.sec != 59)
							time_temp.sec++;
				break;
				default : break;
			}
		}
	}
	else if(key_falling == B4)
	{
		if(interface == RTC_SETTING || interface == ALARM_SETTING)
		{
LABAL2:switch(para_select)
			{
				case 0: 
						if(time_temp.hour != 0)
							time_temp.hour--;
				break;
				case 1: 
						if(time_temp.min != 0)
							time_temp.min--;
				break;
				case 2: 
						if(time_temp.sec != 0)
							time_temp.sec--;
				break;
				default : break;
			}
		}
	}
	if(key_state == B3)
	{
		if(key_state == B3 && b3_cnt <= 100)
		{
			b3_cnt++;
		}
		else if(b3_cnt >= 100 && key_state == B3)
		{
			if(++b3_cnt == 110)
			{
				b3_cnt = 100;
				goto LABAL1;
			}
		}
	}
	else if(key_state != B3 && b3_cnt != 0)
	{
		b3_cnt = 0;
	}
	
	if(key_state == B4)
	{
		if(key_state == B4 && b4_cnt <= 100)
		{
			b4_cnt++;
		}
		else if(b4_cnt >= 100 && key_state == B4)
		{
			if(++b4_cnt == 110)
			{
				b4_cnt = 100;
				goto LABAL2;
			}
		}
	}
	else if(key_state != B4 && b4_cnt != 0)
	{
		b4_cnt = 0;
	}
	
	
}

void highlight(uint8_t* str)
{
	uint8_t i = 0;
	if(interface == RTC_SETTING)
	{
		for(i = 0;i < 20; i++)
		{
			if(	(para_select == 0 && (i == 8 || i == 9)) || \
					(para_select == 1 && (i == 11 || i == 12)) || \
					(para_select == 2 && (i == 14 || i == 15))
			)
			{
				LCD_SetTextColor(Red);
			}
			LCD_DisplayChar(Line2, Column(i), str[i]);
			LCD_SetTextColor(White);
		}
	}
	else if(interface == ALARM_SETTING)
	{
		for(i = 0;i < 20; i++)
		{
			if(	(para_select == 0 && (i == 9 || i == 10)) || \
					(para_select == 1 && (i == 12 || i == 13)) || \
					(para_select == 2 && (i == 15 || i == 16))
			)
			{
				LCD_SetTextColor(Red);
			}
			LCD_DisplayChar(Line2, Column(i), str[i]);
			LCD_SetTextColor(White);
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
	STM3210B_LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
	key_init();
	tim_init();
	rtc_init();
	Time_Adjust(11,59,50);
	usart_init();
	
	beep_init();
	
	i2c_init();
	
	if(eeprom_read(7) == 21 && eeprom_read(8) == 43 && eeprom_read(9) == 65)
	{
		int temp;
		temp = eeprom_read(0);
		if(temp < 24)
			time_alarm.hour = temp;
		temp = eeprom_read(1);
		if(temp < 60)
			time_alarm.min = temp;
		temp = eeprom_read(2);
		if(temp < 60)
			time_alarm.sec = temp;
		
		RTC_SETTING_cnt = eeprom_read(3);
		ALARM_SETTING_cnt = eeprom_read(3);
	}
	else
	{
		eeprom_write(7,21);
		Delay_Ms(5);
		eeprom_write(8,43);
		Delay_Ms(5);
		eeprom_write(9,65);
		Delay_Ms(5);
		eeprom_write(0,12);
		Delay_Ms(5);
		eeprom_write(1,0);
		Delay_Ms(5);
		eeprom_write(2,0);
		Delay_Ms(5);
		eeprom_write(3,0);
		Delay_Ms(5);
		eeprom_write(4,0);
		Delay_Ms(5);
	}
	
	while(1)
	{
		Time_Reresh();
		if(eeprom_write_flag)
		{
			eeprom_write_flag = 0;
			eeprom_write(3,RTC_SETTING_cnt);
			Delay_Ms(5);
			eeprom_write(4,ALARM_SETTING_cnt);
			Delay_Ms(5);
			eeprom_write(0,time_alarm.hour);
			Delay_Ms(5);
			eeprom_write(1,time_alarm.min);
			Delay_Ms(5);
			eeprom_write(2,time_alarm.sec);
			Delay_Ms(5);
		}
		if(interface == MAIN)
		{
			if(TMM == time_alarm.min && THH == time_alarm.hour && TSS == time_alarm.sec)
				beep_flag = 1;
		}
		else
		{
			beep_flag = 0;
			beep_cnt = 0;
		}
		
		if(beep_flag)
		{
			beep_ctrl(ENABLE);
		}
		else
		{
			beep_ctrl(DISABLE);
		}
		
		if(interface == MAIN)
		{
			//RTC:11:59:50
			LCD_DisplayStringLine(Line0 ,(unsigned char *)"        MAIN        ");
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char*)lcd_str,"    RTC:%02d:%02d:%02d    ",THH,TMM,TSS);
			LCD_DisplayStringLine(Line2 ,lcd_str);
			led_ctrl(LD2 | LD3,DISABLE);
			if(led_flag)
			{
				led_flag = 0;
				led_toggle(LD1);
			}
		}
		else if(interface == RTC_SETTING)
		{
			LCD_DisplayStringLine(Line0 ,(unsigned char *)"    RTC-SETTING     ");
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char*)lcd_str,"    RTC:%02d:%02d:%02d    ",time_temp.hour,time_temp.min,time_temp.sec);
			highlight(lcd_str);
			led_ctrl(LD1 | LD3,DISABLE);
			led_ctrl(LD2,	ENABLE);
		}
		else if(interface == ALARM_SETTING)
		{
			LCD_DisplayStringLine(Line0 ,(unsigned char *)"   ALARM-SETTING    ");
			memset(lcd_str,0,sizeof(lcd_str));
			
			sprintf((char*)lcd_str,"   Alarm:%02d:%02d:%02d   ",time_temp.hour,time_temp.min,time_temp.sec);
			highlight(lcd_str);
			led_ctrl(LD1 | LD2,DISABLE);
			led_ctrl(LD3,	ENABLE);
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
		if(++led_cnt == 1000)
		{
			led_cnt = 0;
			led_flag = 1;
		}
		if(beep_flag && ++beep_cnt == 500)
		{
			beep_cnt = 0;
			beep_flag = 0;
		}
  }
}


#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
  

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART2, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
  {}

  return ch;
}
