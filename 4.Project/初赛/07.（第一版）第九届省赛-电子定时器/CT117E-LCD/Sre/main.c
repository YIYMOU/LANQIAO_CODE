#include "stm32f10x.h"
#include "lcd.h"
#include "i2c.h"
#include "key.h"
#include "led.h"
#include "tim.h"
#include "pwm.h"
#include "string.h"
#include "stdio.h"

#define Standby			0
#define Setting			1
#define Running			2
#define Pause				3

#define LONG				2
#define SHORT				1

uint8_t state = Standby;

uint8_t location = 0;

u32 TimingDelay = 0;

uint8_t key_cnt = 0;

_Bool led_flag = 0;
uint16_t led_cnt = 0;

uint8_t lcd_str[20];

struct TIME {
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
} time_set[5],time_temp = {0,0,0};

uint8_t select = 4;

uint8_t key2_tick = 0;
uint8_t key3_tick = 0;
uint8_t key4_tick = 0;

union ID {
	uint32_t id;
	uint8_t str[20];
} My_ID;

uint16_t tick = 0;
uint32_t total_sec = 0;

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}

uint8_t key_state_get(uint8_t key)
{
	uint8_t ret = 0;
	uint8_t *p;
	
	if(key == B2)
		p = &key2_tick;
	else if(key == B3)
		p = &key3_tick;
	else if(key == B4)
		p = &key4_tick;
	
	if(key_state == key && (*p) < 80)
	{
		(*p) += 1;
	}
	else if(key_state == key && (*p) >= 80)
	{
		// 长按
		ret = LONG;		// 表示key长按
	}
	else if((*p) && key_state != key)
	{
		if((*p) < 80)
		{
			// 短按
			ret = SHORT; // 表示key短按
		}
		// 初始化
		(*p) = 0;
	}
	return ret;	// 返回值为0表示key没有按下
}

void modify_time(void)
{
	if(select == 0)
	{
		time_temp.hour = (time_temp.hour + 1) % 24;
	}
	else if(select == 1)
	{
		time_temp.min = (time_temp.min + 1) % 60;
	}
	else if(select == 2)
	{
		time_temp.sec = (time_temp.sec + 1) % 60;
	}
}

void key_scan(void)
{
	key_refresh();
	if(key_falling == B1)
	{
		location = (location + 1) % 5;
		total_sec = time_set[location].hour * 3600 + time_set[location].min * 60 + time_set[location].sec;
		tick = 0;
	}
	else if(key_state_get(B2) == SHORT)
	{
		if(state != Setting)
		{
			state = Setting;
			select = 0;
			time_temp.hour = 0;
			time_temp.min = 0;
			time_temp.sec = 0;
		}
		else
		{
			select = (select + 1) % 3;
		}
	}
	else if(key_state_get(B2) == LONG)
	{
		// 存储
		if(state == Setting)
		{
			state = Standby;
			time_set[location].hour = time_temp.hour;
			time_set[location].min = time_temp.min;
			time_set[location].sec = time_temp.sec;
			total_sec = time_set[location].hour * 3600 + time_set[location].min * 60 + time_set[location].sec;
			tick = 0;
		}
	}
	else if(state == Setting && key_state_get(B3) == SHORT)
	{
		modify_time();
	}
	else if(state == Setting && key_state_get(B3) == LONG)
	{
		if(++key3_tick == 90)
		{
			key3_tick = 80;
			modify_time();
		}
	}
	else if(key_state_get(B4) == SHORT)
	{
		if(state == Standby || state == Pause)
		{
			if(state == Standby)
			{
				total_sec = time_set[location].hour * 3600 + time_set[location].min * 60 + time_set[location].sec;
				tick = 0;
			}
			if(total_sec != 0)
				state = Running;
		}
		else if(state == Running)
		{
			state = Pause;
		}
	}
	else if(key_state_get(B4) == LONG)
	{
		if(state == Running || state == Pause)
		{
			state = Standby;
			total_sec = time_set[location].hour * 3600 + time_set[location].min * 60 + time_set[location].sec;
			tick = 0;
		}
	}
	
}

void highlight(u8 Line, u8 *ptr)
{
	u32 i = 0;
	u16 refcolumn = 319;//319;
	u8 temp1 = 6 + select * 3;
	u8 temp2 = 7 + select * 3;
	// 6 7   8   9 10   11  12 13

	while ((*ptr != 0) && (i < 20))	 //	20
	{
		if(i == temp1 || i == temp2)
		{
			LCD_SetTextColor(Yellow);
		}
		LCD_DisplayChar(Line, refcolumn, *ptr);
		LCD_SetTextColor(White);
		refcolumn -= 16;
		ptr++;
		i++;
	}
}

void lcd_proc(void)
{
	
	memset(lcd_str,0,sizeof(lcd_str));
	sprintf((char*)lcd_str,"  No %d     ",location + 1);
	LCD_DisplayStringLine(Line1 ,lcd_str);
	
	
	memset(lcd_str,0,sizeof(lcd_str));
	if(state != Setting)
	{
		sprintf((char*)lcd_str,"      %02d:%02d:%02d   ",total_sec / 3600,total_sec % 3600 / 60,total_sec % 60);
		LCD_DisplayStringLine(Line5 ,lcd_str);
	}
	else
	{
		sprintf((char*)lcd_str,"      %02d:%02d:%02d   ",time_temp.hour,time_temp.min,time_temp.sec);
		highlight(Line5 ,lcd_str);
	}
	
	switch(state)
	{
		case Standby: LCD_DisplayStringLine(Line7 ,(unsigned char *)"       Standby   "); break;
		case Setting: LCD_DisplayStringLine(Line7 ,(unsigned char *)"       Setting   "); break;
		case Running: LCD_DisplayStringLine(Line7 ,(unsigned char *)"       Running   "); break;
		case Pause: LCD_DisplayStringLine(Line7 ,(unsigned char *)"       Pause   "); break;
		default : break;
	}
	
}

void check(void)
{
	uint8_t i;
	for(i = 0;i < 8;i++)
	{
		My_ID.str[i] = eeprom_read(0x60 + i);
	}
	if(My_ID.id == 1375132689)
	{
		for(i = 0;i < 5;i++)
		{
			time_set[i].hour = eeprom_read(3 * i + 0);
			time_set[i].min =  eeprom_read(3 * i + 1);
			time_set[i].sec =  eeprom_read(3 * i + 2);
			if(time_set[i].hour >= 24)
				time_set[i].hour = 0;
			if(time_set[i].min >= 60)
				time_set[i].min = 0;
			if(time_set[i].sec >= 60)
				time_set[i].sec = 0;
		}
		total_sec = time_set[0].hour * 3600 + time_set[0].min * 60 + time_set[0].sec;
	}
	else
	{
		My_ID.id = 1375132689;
		for(i = 0;i < 8;i++)
		{
			eeprom_write(0x60 + i,My_ID.str[i]);
			Delay_Ms(5);
		}
		for(i = 0;i < 15;i++)
		{
			eeprom_write(i,0);
			Delay_Ms(5);
		}
	}
}

void led_proc(void)
{
	
	if(led_flag)
	{
		led_flag =  0;
		if(state == Running)
			led_toggle(LD1);
		else
			led_ctrl(LD1,DISABLE);
	}
}

void eeprom_proc(void)
{
	static uint8_t state_pre = Standby;
	if(state != Setting && state_pre == Setting)
	{
		eeprom_write(3 * location + 0,time_set[location].hour);
		Delay_Ms(5);
		eeprom_write(3 * location + 1,time_set[location].min);
		Delay_Ms(5);
		eeprom_write(3 * location + 2,time_set[location].sec);
		Delay_Ms(5);
	}
	state_pre = state;
}
void pwm_proc(void)
{
	static uint8_t state_pre = Standby;
	if(state == Running && state_pre != Running)
	{
		pwm_init(ENABLE);
	}
	else if(state != Running && state_pre == Running)
	{
		pwm_init(DISABLE);
	}
	state_pre = state;
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
	pwm_init(DISABLE);
	check();
	
	
	while(1)
	{
		
		lcd_proc();
		
		led_proc();
		
		eeprom_proc();
		
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
		
		if(++led_cnt == 500)
		{
			led_cnt = 0;
			led_flag = 1;
		}
		
		if(state == Running && ++tick == 1000)
		{
			total_sec--;
			if(total_sec == 0)
				state = Standby;
			tick = 0;	
		}
  }
}
