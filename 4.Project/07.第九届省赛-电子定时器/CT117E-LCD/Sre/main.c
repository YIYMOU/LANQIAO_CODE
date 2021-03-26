#include "stm32f10x.h"
#include "lcd.h"
#include "led.h"
#include "tim.h"
#include "key.h"
#include "pwm.h"
#include "string.h"
#include "stdio.h"
#include "i2c.h"

// 如何高亮？

#define		Standby		0
#define		Setting		1
#define		Running		2
#define		Pause			3

#define LOCATION_1	0
#define LOCATION_2	3
#define LOCATION_3	6
#define LOCATION_4	9
#define LOCATION_5	12

uint8_t locate = 0;

uint8_t select = 0; // 0:hour,	1:min,	2:sec

struct TIME {
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
}	time = {0,1,55},time_temp = {0,0,0},time_now;

uint16_t time_cnt = 0;

u32 TimingDelay = 0;

uint16_t led_cnt = 0;
_Bool led_flag = 0;

uint8_t key_cnt = 0;

uint8_t state = 0;	// 0：停止，1：启动，2：暂停

uint8_t lcd_str[20];

uint8_t b4_cnt  = 0;
uint8_t b3_cnt  = 0;
uint8_t b2_cnt  = 0;

_Bool eeprom_write_flag = 0;

void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}

void time_load(void)
{
		time.hour = eeprom_read(locate * 3 + 0);
		time.min = eeprom_read(locate * 3 + 1);
		time.sec = eeprom_read(locate * 3 + 2);
		if(time.hour > 23 || time.min > 59 || time.sec > 59)
		{
			time.hour = 0;
			time.min = 0;
			time.sec = 0;
		}
}

void pwm_ctrl(FunctionalState NewState)
{
	if(NewState == ENABLE)
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		/* GPIOA Configuration:TIM3 Channel1 as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	else
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIOA->ODR &=~GPIO_Pin_6;
	}
}

void key_scan(void)
{
	key_refresh();
	
	if((state == Standby || state == Setting) && key_falling == B1)
	{
		locate = (locate + 1) % 5;
		time_load();
	}
	
	if(key_falling == B3)
	{
		if(state == Setting)
		{
FLAG1:switch(select)
			{
				case 0:(time_temp.hour == 23)?(time_temp.hour = 0 ):( time_temp.hour = time_temp.hour + 1);	break;
				case 1:(time_temp.min == 59)?(time_temp.min = 0 ):( time_temp.min = time_temp.min + 1); break;
				case 2:(time_temp.sec == 59)?(time_temp.sec = 0 ):( time_temp.sec = time_temp.sec + 1); break;
				default: break;
			}
		}
	}
	else if(state == Setting && key_state == B3)
	{
		if(b3_cnt < 80)
		{
			b3_cnt++;
		}
	}
	if(state == Setting && b3_cnt >= 80)	// B3按键长按
	{
		if(key_state != B3)
		{
			b3_cnt = 0;
		}
		else
		{
			if(++b3_cnt == 90)
			{
				b3_cnt = 80;
				goto FLAG1;
			}
		}
	}
	
	if(key_state == B2)
	{
		if(b2_cnt < 80)
		{
			b2_cnt++;
		}
		else if(b2_cnt == 80)	// b2按键长按
		{
			b2_cnt = 100;
			state = Standby;
			eeprom_write_flag = 1;
		}
	}
	else if(b2_cnt && key_state != B2)
	{
		if(b2_cnt != 100)	// b2按键短按
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
		b2_cnt = 0;
	}
	
	if(key_state == B4)
	{
		if(b4_cnt < 80)
		{
			b4_cnt++;
		}
		else if(b4_cnt == 80)	// B4按键长按
		{
			b4_cnt = 100;
			state = Standby;
			time_load();
		}
	}
	else if(b4_cnt && key_state != B4)
	{
		if(b4_cnt != 100)	// B4按键短按
		{
			if(state == Running)
			{
				state = Pause;
			}
			else
			{
				if(state == Standby)
				{
					time_load();
				}
				else if(state == Setting)
				{
					time.hour = time_temp.hour;
					time.min = time_temp.min;
					time.sec = time_temp.sec;
				}
				if(time.hour || time.min || time.sec)
					state = Running;
			}
		}
		b4_cnt = 0;
	}
}

void highlight(uint8_t *str,uint8_t pos)
{
	int i = 0;
	LCD_SetBackColor(Yellow);
	//LCD_SetTextColor(Red);
	LCD_DisplayChar(Line4,(320 - (16 * pos)),str[pos]);
	LCD_DisplayChar(Line4,(320 - (16 * (pos + 1))),str[pos + 1]);
	LCD_SetBackColor(White);	
	//LCD_SetTextColor(Blue2);
	for(i = 0; i <= 19; i++)
	{
		if(i != pos && i != pos + 1)
			LCD_DisplayChar(Line4,(320 - (16 * i)),str[i]);
	}
}


//Main Body
int main(void)
{
	
	SysTick_Config(SystemCoreClock/1000);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	Delay_Ms(200);
	
	led_init();
	STM3210B_LCD_Init();
	LCD_Clear(White);
	LCD_SetBackColor(White);
	LCD_SetTextColor(Blue2);
	
	tim_init();
	key_init();
	pwm_init();
	i2c_init();
	
	time_load();
//	
	while(1)
	{	
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"   No %-14d",locate + 1);
		LCD_DisplayStringLine(Line1 ,lcd_str);
		
		switch(state)
		{
			case Standby:	
			{
				memset(lcd_str,0,sizeof(lcd_str));
				sprintf((char *)lcd_str,"      %02d:%02d:%02d      ",time.hour,time.min,time.sec);
				LCD_DisplayStringLine(Line4 ,lcd_str);		
				LCD_DisplayStringLine(Line8 ,(unsigned char *)"       Standby      ");
			}
			break;
			case Setting:	
			{
				LCD_DisplayStringLine(Line8 ,(unsigned char *)"       Setting      ");
				memset(lcd_str,0,sizeof(lcd_str));
				sprintf((char *)lcd_str,"      %02d:%02d:%02d      ",time_temp.hour,time_temp.min,time_temp.sec);
//				LCD_DisplayStringLine(Line4 ,lcd_str);
				switch(select)
				{
					case 0:
					{
						highlight(lcd_str,6);
					}						
					break;
					case 1:
					{
						highlight(lcd_str,9);
					}
					break;
					case 2:
					{
						highlight(lcd_str,12);
					}
					break;
					default: break;
				}
			}
			break;
			case Running:	
			{
				if(led_flag)
				{
					led_flag = 0;
					led_toggle(LD1);
				}
				pwm_ctrl(ENABLE);
				memset(lcd_str,0,sizeof(lcd_str));
				sprintf((char *)lcd_str,"      %02d:%02d:%02d      ",time.hour,time.min,time.sec);
				LCD_DisplayStringLine(Line4 ,lcd_str);	
				LCD_DisplayStringLine(Line8 ,(unsigned char *)"       Running      ");
			}		
			break;
			case Pause:	
			{
				LCD_DisplayStringLine(Line8 ,(unsigned char *)"        Pause       ");
			}		
			break;
			default : break;
		}
		
		if(state != Running)
		{
			pwm_ctrl(DISABLE);
			led_ctrl(LD1,DISABLE);
		}
		
		if(eeprom_write_flag == 1)
		{
			eeprom_write_flag = 0;
			eeprom_write(locate * 3 + 0,time_temp.hour);
			Delay_Ms(5);
			eeprom_write(locate * 3 + 1,time_temp.min);
			Delay_Ms(5);
			eeprom_write(locate * 3 + 2,time_temp.sec);
			Delay_Ms(5);
			time_load();
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
		
		if(state == Running && ((++time_cnt == 500) || (time_cnt == 1000)))
		{
			led_flag = 1;
			if(time_cnt == 1000)
			{
				time_cnt = 0;
				if(time.sec)
				{
					time.sec--;
				}
				else if(time.min)
				{
					time.min--;
					time.sec = 59;
				}
				else if(time.hour)
				{
					time.hour--;
					time.sec = 59;
					time.min = 59;
				}
				if(time.hour == 0 && time.min == 0 && time.sec == 0)
				{
					state = Standby;
				}
			}
		}
  }
}
