/************************************************
*蓝桥杯嵌入式第十二届省赛
*CSDN：https://blog.csdn.net/qq_43715171
*码云：https://gitee.com/yiymou
*GitHub：https://github.com/YIYMOU
*硬件平台：国信长天CT117E
*作者：yiymou   2021-05-06
************************************************/
#include "stm32f10x.h"
#include "lcd.h"
#include "led.h"
#include "tim.h"
#include "key.h"
#include "pwm.h"
#include "stdio.h"
#include "string.h"
#include "usart.h"

#define			PARA				0
#define			DATA				1

uint8_t RxBuffer[30];
__IO uint8_t RxCounter = 0; 
_Bool RxFlag = 0;

uint8_t lcd_str[20];

uint8_t CNBR_cnt = 0;
uint8_t VNBR_cnt = 0;
//uint8_t IDLE_cnt = 8;

uint8_t CNBR_fee = 35;
uint8_t VNBR_fee = 20;

uint8_t CNBR_fee_temp = 35;
uint8_t VNBR_fee_temp = 20;

_Bool interface = DATA;

FunctionalState PwmState = DISABLE;

u32 TimingDelay = 0;


uint8_t key_tick = 0;

// 结构体，存储车辆信息
struct CAR_DATA {
	uint8_t type[5];
	uint8_t id[5];
	uint8_t year_in;
	uint8_t month_in;
	uint8_t day_in;
	uint8_t hour_in;
	uint8_t min_in;
	uint8_t sec_in;
	_Bool notEmpty;
} car[8] = {0};

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}

void key_scan(void)
{
	key_refresh();	// 刷新一次按键的状态信息
	if(key_falling == B1)		// k1按下，切换 LCD 显示“车位显示界面”和“费率设置界面”
	{
		if(interface == DATA)
		{
			interface = PARA;
		}
		else
		{
			interface = DATA;
			CNBR_fee = CNBR_fee_temp;
			VNBR_fee = VNBR_fee_temp;
		}
	}
	else if(key_falling == B2 && interface == PARA)		// “加”按键，B2、B3仅在设置界面有效
	{
		if(CNBR_fee_temp < 250 && VNBR_fee_temp < 250)
		{
			CNBR_fee_temp += 5;
			VNBR_fee_temp += 5;
		}
	}
	else if(key_falling == B3 && interface == PARA)		// 减”按键，B2、B3仅在设置界面有效
	{
		if(CNBR_fee_temp > 0 && VNBR_fee_temp > 0)
		{
			CNBR_fee_temp -= 5;
			VNBR_fee_temp -= 5;
		}
	}
	else if(key_falling == B4)		// “控制”按键
	{
		PwmState = (PwmState == ENABLE)?DISABLE:ENABLE;
		pwm_init(PwmState);
	}
}

void led_proc(void)
{
	led_ctrl(LD1,(8 - CNBR_cnt - VNBR_cnt)? ENABLE:DISABLE);
	led_ctrl(LD2,PwmState);
}

void lcd_proc(void)
{
	if(interface == DATA)
	{
		LCD_DisplayStringLine(Line1 ,(unsigned char *)"       Data         ");
	
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"   CNBR:%d    ",CNBR_cnt);
		LCD_DisplayStringLine(Line3,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"   VNBR:%d    ",VNBR_cnt);
		LCD_DisplayStringLine(Line5,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"   IDLE:%d    ",(8 - CNBR_cnt - VNBR_cnt));
		LCD_DisplayStringLine(Line7,lcd_str);
	}
	else
	{
		LCD_DisplayStringLine(Line1 ,(unsigned char *)"       Para         ");
	
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"   CNBR:%.2f    ",CNBR_fee_temp / 10.0);
		LCD_DisplayStringLine(Line3,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"   VNBR:%.2f    ",VNBR_fee_temp / 10.0);
		LCD_DisplayStringLine(Line5,lcd_str);
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char*)lcd_str,"               ");
		LCD_DisplayStringLine(Line7,lcd_str);
	}
}

void substr(uint8_t* d_str,uint8_t* s_str,uint8_t locate,uint8_t length)
{
	uint8_t i = 0;
	for(i = 0; i < length; i++)
	{
		d_str[i] = s_str[locate + i];
	}
	d_str[length] = '\0';
}

uint8_t findLocate(void)	// 找一个空闲的位置
{
	uint8_t i = 0;
	for(i = 0; i < 8; i++)
	{
		if(!car[i].notEmpty)	// 如果是空闲
			return i;
	}
	return 0xFF;
}

uint8_t isExist(uint8_t* str)		// 判断车辆是否存在
{
	uint8_t i = 0;
	for(i = 0; i < 8; i++)
	{
		if(strcmp((const char*)str,(const char*)car[i].id) == 0)
		{
			return i;			// 如果这辆车存在，则返回这辆车所在的车位i
		}
	}
	return 0xFF;			// 返回0xFF表示这辆车不存在
}

_Bool checkCmd(uint8_t* str)
{
	// VNBR:D583:200202120000
	// 0123456789012345678901
	if(RxCounter != 22)
		return 0;
	if((str[0] == 'C' || str[0] == 'V') && str[1] == 'N' && str[2] == 'B' && str[3] == 'R' && str[4] == ':' && str[9] == ':')
	{
		uint8_t i;
		for(i = 10; i < 22; i++)
		{
			if(str[i] > '9' || str[i] < '0')
				return 0;
		}
	}
	return 1;
}

void usart_proc(void)
{
	if(RxFlag)
	{
		RxFlag = 0;
		
// 将接收到的字符串显示到LCD上面
//		memset(lcd_str,0,sizeof(lcd_str));
//		sprintf((char*)lcd_str,"%-20.20s",RxBuffer);
//		LCD_DisplayStringLine(Line9,lcd_str);
		
		// VNBR:D583:200202120000
		// 0123456789012345678901
		if(checkCmd(RxBuffer))	// 接收到的标准信息应该是22个字符，并且只有在有空闲车位的时候有效
		{
			uint8_t car_id[5];
			uint8_t car_type[5];
			uint8_t locate = 0xFF;
			uint8_t year_temp,month_temp,day_temp,hour_temp,min_temp,sec_temp;
			// 将字符串中时间的信息提取出来
			year_temp = (RxBuffer[10] - '0') * 10 + (RxBuffer[11] - '0');
			month_temp = (RxBuffer[12] - '0') * 10 + (RxBuffer[13] - '0');
			day_temp = (RxBuffer[14] - '0') * 10 + (RxBuffer[15] - '0');
			hour_temp = (RxBuffer[16] - '0') * 10 + (RxBuffer[17] - '0');
			min_temp = (RxBuffer[18] - '0') * 10 + (RxBuffer[19] - '0');
			sec_temp = (RxBuffer[20] - '0') * 10 + (RxBuffer[21] - '0');
			if(year_temp > 99 || month_temp > 12 || day_temp > 31 || hour_temp > 23 || min_temp > 59 || sec_temp > 59)
			{
//				printf("shi jian ge shi error!\r\n");
				goto SEND_ERROR;
			}
			substr(car_id,RxBuffer,5,4);		// 将车辆的编号信息提取到car_id
			substr(car_type,RxBuffer,0,4);	// 将车辆的类型信息提取到car_type
			locate = isExist(car_id);				// 查询这辆车是否在车库从存在
			
			if(locate != 0xFF)	// 这辆车在车库中存在
			{
				int time_val;
				printf("locate:%d,type:%s,id:%s\r\n",locate,car[locate].type,car[locate].id);
				if(strcmp((const char *)car_type,(const char *)car[locate].type))		// 如果车辆的id和车辆的类型不同，则表示信息错误
				{
//					printf("id and type pi pei error!\r\n");
					goto SEND_ERROR;
				}
				// 假设一年365天,一个月30天，秒为单位
				time_val = (year_temp - car[locate].year_in) * 365 * 24 * 3600 + (month_temp - car[locate].month_in) * 30 * 24 * 3600 + (day_temp - car[locate].day_in) * 24 * 3600 + \
									 (hour_temp - car[locate].hour_in) * 3600 + (min_temp - car[locate].min_in) * 60 + (sec_temp - car[locate].sec_in);
				if(time_val < 0)
				{
//					printf("time_val error!\r\n");
					goto SEND_ERROR;
				}
				time_val = (time_val + 3599) / 3600;	// 换算成小时，并且不足一个小时按一个小时算
				
				// 输出计费信息
				printf("%s:%s:%d:%.2f\r\n",car[locate].type,car[locate].id,time_val,time_val / 10.0 * (RxBuffer[0] == 'C'?CNBR_fee:VNBR_fee));
				
				if(RxBuffer[0] == 'C')
					CNBR_cnt--;
				else if(RxBuffer[0] == 'V')
					VNBR_cnt--;
				
				memset(&car[locate],0,sizeof(car[locate]));	// 将当前结构体清空
			}
			else		// 这辆车在车库中不存在
			{
				uint8_t locate = findLocate();	// 找一个空闲的车位
				
				if(locate == 0xFF)	// 没有找到空闲车位
				{
					goto SEND_ERROR;
				}
//				printf("locate:%d,type:%s\r\n",locate,car[locate].type);
				// 保存车辆信息
				substr(car[locate].type,RxBuffer,0,4);
				substr(car[locate].id,RxBuffer,5,4);
				car[locate].year_in = year_temp;
				car[locate].month_in = month_temp;
				car[locate].day_in = day_temp;
				car[locate].hour_in = hour_temp;
				car[locate].min_in = min_temp;
				car[locate].sec_in = sec_temp;
				car[locate].notEmpty = 1;			// 标记为非空闲
				if(RxBuffer[0] == 'C')
					CNBR_cnt++;
				else if(RxBuffer[0] == 'V')
					VNBR_cnt++;
			}
			goto CMD_YES;
		}
SEND_ERROR:printf("ERROR\r\n");
CMD_YES:		memset(RxBuffer,0,sizeof(RxBuffer));
		RxCounter = 0;
	}
}

/*	测试数据，用来测试逻辑是否有错误
1. *
VNBR:D583:200202120000
VNBR:D583:200202213205

2. *
CNBR:D593:200202120000
CNBR:D593:200203213205

3. *
VNBR:D883:200202120000
VNBR:D883:200202223205

4. *
CNBR:D588:200202120000
CNBR:D588:200202313205	会提示时间设置错误
CNBR:D588:200202223205	时间设置正确

5. *
CNBR:D580:200202120000
CNBR:D580:200202215205

6. *
VNBR:D58S:200202120000
VNBR:D58S:200203213205

7. *
CNBR:D58E:200202120000
CNBR:D58E:200204213205

8. *
CNBR:D58B:200202120000
CNBR:D58B:200205213205

9. 
CNBR:D555:200202120000
CNBR:D555:200205213205
*/

//Main Body
int main(void)
{
	SysTick_Config(SystemCoreClock/1000);

	Delay_Ms(200);
	
	led_init();
	tim_init();
	key_init();
	pwm_init(PwmState);
	usart_init();
	
	STM3210B_LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
//	printf("Hello World!!!\r\n");
	
	while(1)
	{
		
		led_proc();
		
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
		
		if(++key_tick == 10)		// 每10ms扫描一次按键
		{
			key_tick = 0;
			key_scan();
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
  }
  
  if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
  {   
    /* Write one byte to the transmit data register */
    USART_ReceiveData(USART2);
		
		RxFlag = 1;
		
		USART_ClearITPendingBit(USART2, TIM_IT_Update);
  }
}
