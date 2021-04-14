#include "stm32f10x.h"
#include "lcd.h"
#include "i2c.h"
#include "key.h"
#include "led.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"
#include "beep.h"

#define			MAIN						0
#define			RTC_SETTING			1
#define			ALARM_SETTING		2

#define			NONE						0
#define			LONG_STATE			1
#define			SHORT_STATE 		2

u32 TimingDelay = 0;

uint8_t key_cnt = 0;

_Bool beep_flag = 0;
uint16_t beep_cnt = 0;

uint8_t lcd_str[20];

uint8_t interface = MAIN;

uint8_t select = 0;

struct TIME {
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
} alarm_time = {12,0,0},rtc_time = {11,59,50},rtc_time_temp = {0,0,0},alarm_time_temp = {0,0,0};

uint8_t key3_cnt = 0;
uint8_t key4_cnt = 0;

uint8_t alarm_setting_times = 0;
uint8_t rtc_change_times = 0;
	
_Bool rtc_setting_over = 0;
_Bool alarm_setting_over = 0;
	
uint8_t RxBuffer[20];
uint8_t RxCounter = 0;
uint8_t RxTick = 0;
_Bool RxFlag = 0;
_Bool RxIdleFlag = 0;
	
uint8_t ch;
	
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

uint8_t key_state_check(uint8_t key)
{
	uint8_t *p;
	uint8_t ret_state = NONE;
	
	if(key == B3)
	{
		p = &key3_cnt;
	}
	else if(key == B4)
	{
		p = &key4_cnt;
	}
	
	if(key_state == key && (*p) < 100)
	{
		(*p)++;
	}
	else if(key_state == key && (*p) >= 100)
	{
		if(++(*p) == 110)
		{
			(*p) = 100;
			ret_state = LONG_STATE;
		}
	}
	else if(key_state != key && (*p) != 0)
	{
		if((*p) < 100)
		{
			ret_state = SHORT_STATE;
		}
		(*p) = 0;
	}
	return ret_state;
}

void time_update(uint8_t key)
{
	struct TIME* time;
	int temp = 0;
	if(key == B4)
	{
		temp = -1;
	}
	else
	{
		temp = 1;
	}
	
	if(interface == RTC_SETTING)
		time = &rtc_time_temp;
	else if(interface == ALARM_SETTING)
		time = &alarm_time_temp;
	if(select == 0)
	{
		if((time->hour != 23 && temp == 1) || (time->hour != 0 && temp == -1))
			time->hour += temp;
	}
	else if(select == 1)
	{
		if((time->min != 59 && temp == 1) || (time->min != 0 && temp == -1))
		time->min += temp;
	}
	else if(select == 2)
	{
		if((time->sec != 59 && temp == 1) || (time->sec != 0 && temp == -1))
		time->sec += temp;
	}
}

void key_scan(void)
{
	key_refresh();
	if(key_falling == B1)
	{
		if(interface == MAIN)
		{
			interface = RTC_SETTING;
			rtc_time_temp.hour = 0;
			rtc_time_temp.min = 0;
			rtc_time_temp.sec = 0;
			select = 0;
		}
		else if(interface == ALARM_SETTING)
		{
			select = (select + 1) % 3;
		}
		else if(interface == RTC_SETTING)
		{
			rtc_time.hour = rtc_time_temp.hour;
			rtc_time.min = rtc_time_temp.min;
			rtc_time.sec = rtc_time_temp.sec;
			Time_Adjust(rtc_time.hour,rtc_time.min,rtc_time.sec);
			rtc_setting_over = 1;
			rtc_change_times++;
			interface = MAIN;
		}
	}
	else if(key_falling == B2)
	{
		if(interface == MAIN)
		{
			interface = ALARM_SETTING;
			alarm_time_temp.hour = 0;
			alarm_time_temp.min = 0;
			alarm_time_temp.sec = 0;
			select = 0;
		}
		else if(interface == RTC_SETTING)
		{
			select = (select + 1) % 3;
		}
		else if(interface == ALARM_SETTING)
		{
			alarm_time.hour = alarm_time_temp.hour;
			alarm_time.min =  alarm_time_temp.min;
			alarm_time.sec =  alarm_time_temp.sec;
			alarm_setting_times++;
			alarm_setting_over = 1;
			interface = MAIN;
		}
	}
	else if((interface == ALARM_SETTING || interface == RTC_SETTING) && key_state_check(B3) == SHORT_STATE)
	{
		time_update(B3);
	}
	else if((interface == ALARM_SETTING || interface == RTC_SETTING) && key_state_check(B3) == LONG_STATE)
	{
		time_update(B3);
	}
	else if((interface == ALARM_SETTING || interface == RTC_SETTING) && key_state_check(B4) == SHORT_STATE)
	{
		time_update(B4);
	}
	else if((interface == ALARM_SETTING || interface == RTC_SETTING) && key_state_check(B4) == LONG_STATE)
	{
		time_update(B4);
	}
}

void highlight(u8 Line, u8 *ptr)
{
	u32 i = 0;
	u16 refcolumn = 319;//319;
	// 9 10     12 13     15 16
	while ((*ptr != 0) && (i < 20))	 //	20
	{
		if(((i == 8 || i == 9) && select == 0) || ((i == 11 || i == 12) && select == 1) || ((i == 15 || i == 14) && select == 2))
				LCD_SetTextColor(Green);
		LCD_DisplayChar(Line, refcolumn, *ptr);
		LCD_SetTextColor(White);
		refcolumn -= 16;
		ptr++;
		i++;
	}
	return ;
}

void lcd_proc(void)
{
	switch(interface)
	{
		case MAIN: 
			LCD_DisplayStringLine(Line0 ,(unsigned char *)"        MAIN    ");
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char*)lcd_str,"    RTC:%02d:%02d:%02d    ",rtc_time.hour,rtc_time.min,rtc_time.sec);
			LCD_DisplayStringLine(Line2 ,lcd_str);
		break;
		
		case RTC_SETTING: 
			LCD_DisplayStringLine(Line0 ,(unsigned char *)"    RTC-SETTING   ");
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char*)lcd_str,"    RTC:%02d:%02d:%02d    ",rtc_time_temp.hour,rtc_time_temp.min,rtc_time_temp.sec);
			highlight(Line2 ,lcd_str);
		break;
		
		case ALARM_SETTING:
			LCD_DisplayStringLine(Line0 ,(unsigned char *)"   ALARM-SETTING  ");
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char*)lcd_str,"  Alarm:%02d:%02d:%02d    ",alarm_time_temp.hour,alarm_time_temp.min,alarm_time_temp.sec);
			highlight(Line2 ,lcd_str);
		break;
		
		default : break;
	}
}

void led_proc(void)
{
	if(interface == MAIN)
	{
		if(rtc_time.sec % 2)
		{
			led_ctrl(LD1,ENABLE);
		}
		else
		{
			led_ctrl(LD1,DISABLE);
		}
		led_ctrl(LD2 | LD3,DISABLE);
	}
	else if(interface == RTC_SETTING)
	{
		led_ctrl(LD2,ENABLE);
	}
	else if(interface == ALARM_SETTING)
	{
		led_ctrl(LD3,ENABLE);
	}
	
}

void rtc_proc(void)
{
	uint32_t TimeVar = RTC_GetCounter();
  /* Reset RTC Counter when Time is 23:59:59 */
  if (TimeVar == 0x00015180)
  {
     RTC_SetCounter(0x0);
     /* Wait until last write operation on RTC registers has finished */
     RTC_WaitForLastTask();
  }
  
  /* Compute  hours */
  rtc_time.hour = TimeVar / 3600;
  /* Compute minutes */
  rtc_time.min = (TimeVar % 3600) / 60;
  /* Compute seconds */
  rtc_time.sec = (TimeVar % 3600) % 60;
}

void usart_proc(void)
{
	if(rtc_setting_over)
	{
		rtc_setting_over = 0;
		printf("New RTC::%02d:%02d:%02d\r\n",rtc_time.hour,rtc_time.min,rtc_time.sec);
	}
	if(alarm_setting_over)
	{
		alarm_setting_over = 0;
		printf("New Alarm::%02d:%02d:%02d\r\n",alarm_time.hour,alarm_time.min,alarm_time.sec);
	}
	
	if(RxIdleFlag)
	{
		RxIdleFlag = 0;
		
		RxBuffer[0] = ch;
		if(strcmp((const char*)RxBuffer,"Query!") == 0)
		{
			printf("alarm_setting_times:%d,rtc_setting_times:%d\r\n",alarm_setting_times,rtc_change_times);
		}
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"%20.20s",RxBuffer);
		LCD_SetTextColor(Green);
		LCD_DisplayStringLine(Line8 ,lcd_str);
		LCD_SetTextColor(White);
		
		memset(RxBuffer,0,sizeof(RxBuffer));
		RxCounter = 0;
	}
}

void alarm_proc(void)
{
	if(alarm_time.hour == rtc_time.hour && alarm_time.min == rtc_time.min && alarm_time.sec == rtc_time.sec)
	{
		beep_ctrl(ENABLE);
		beep_flag = 1;
	}
}

void check(void)
{
	uint8_t i;
	for(i = 0; i < 8; i++)
	{
		My_ID.str[i] = eeprom_read(0x80 + i);
	}
	if(My_ID.id == 1375132689)
	{
		alarm_setting_times = eeprom_read(0);
		rtc_change_times = eeprom_read(1);
		alarm_time.hour = eeprom_read(2);
		alarm_time.min = eeprom_read(3);
		alarm_time.sec = eeprom_read(4);
	}
	else
	{
		My_ID.id = 1375132689;
		for(i = 0; i < 8; i++)
		{
			eeprom_write(0x80 + i,My_ID.str[i]); Delay_Ms(5);
		}
		eeprom_write(0,0); Delay_Ms(5);
		eeprom_write(1,0); Delay_Ms(5);
		eeprom_write(2,12); Delay_Ms(5);
		eeprom_write(3,0); Delay_Ms(5);
		eeprom_write(4,0); Delay_Ms(5);
	}
}

void eeprom_proc(void)
{
	static uint8_t alarm_setting_times_pre = 0,rtc_change_times_pre = 0;
	static struct TIME alarm_time_pre;
	if(alarm_setting_times_pre != alarm_setting_times)
	{
		alarm_setting_times_pre = alarm_setting_times;
		eeprom_write(0,alarm_setting_times_pre); Delay_Ms(5);
	}
	if(rtc_change_times != rtc_change_times_pre)
	{
		rtc_change_times_pre = rtc_change_times;
		eeprom_write(1,rtc_change_times_pre); Delay_Ms(5);
	}
	if(alarm_time_pre.hour != alarm_time.hour)
	{
		alarm_time_pre.hour = alarm_time.hour;
		eeprom_write(2,alarm_time_pre.hour); Delay_Ms(5);
	}
	if(alarm_time_pre.min != alarm_time.min)
	{
		alarm_time_pre.min = alarm_time.min;
		eeprom_write(3,alarm_time_pre.min); Delay_Ms(5);
	}
	if(alarm_time_pre.sec != alarm_time.sec)
	{
		alarm_time_pre.sec = alarm_time.sec;
		eeprom_write(4,alarm_time_pre.sec); Delay_Ms(5);
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
	Time_Adjust(rtc_time.hour,rtc_time.min,rtc_time.sec);
	usart_init();
	beep_init();
	check();
	
	while(1)
	{
		rtc_proc();
		
		alarm_proc();
		
		led_proc();
		
		usart_proc();
		
		lcd_proc();
		
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
		
		if(beep_flag && ++beep_cnt == 1000)
		{
			beep_ctrl(DISABLE);
			beep_cnt = 0;
			beep_flag = 0;
		}
		
		if(RxFlag && ++RxTick == 50)
		{
			RxTick = 0;
			RxFlag = 0;
			RxIdleFlag = 1;
		}
  }
}


/**
  * @brief  This function handles USARTy global interrupt request.
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
		if(RxCounter == 1)
		{
			ch = RxBuffer[0];
		}
		// printf("%c",RxBuffer[RxCounter - 1]);
    if(RxCounter == 20)
    {
			memset(RxBuffer,0,sizeof(RxBuffer));
			RxCounter = 0;
    }
  }
}

