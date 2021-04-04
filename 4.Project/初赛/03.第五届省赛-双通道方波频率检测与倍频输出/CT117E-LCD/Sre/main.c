#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"
#include "lcd.h"
#include "led.h"
#include "tim.h"
#include "key.h"
#include "i2c.h"
#include "uart.h"
#include "pwm.h"
#include "InputCapture.h"

#define LOCAL_SETTING 1
#define USART_SETTING 0
#define CH1 0
#define CH2 1
#define ch1_add		0x0
#define ch2_add		0x1

_Bool setting_mode = 1;
_Bool channel_select = 0;

u32 ch1_N = 2;
u32 ch2_N = 2;

u32 TimingDelay = 0;

uint8_t key_cnt = 0;
_Bool key_flag = 0;

uint8_t usart_ticker = 0;
_Bool DataFlag = 0;

uint8_t lcd_str[20];
uint8_t RxBuffer_temp[20];

void Delay_Ms(u32 nTime);

//定义 3 个 LED 指示灯，“LED1”到“LED3”，其中：
//当通道 1 有输出时，“LED1”点亮，否则熄灭；
//当通道 2 有输出时，“LED2”点亮，否则熄灭；
//当系统处于“串口设定”方式时，“LED3”点亮，否则熄灭
void led_handler(void)
{
	if(channel_select == CH1)
	{
		led_ctrl(LD1,ENABLE);
		led_ctrl(LD2,DISABLE);
	}
	else
	{
		led_ctrl(LD2,ENABLE);
		led_ctrl(LD1,DISABLE);
	}
	if(setting_mode == USART_SETTING)
	{
		led_ctrl(LD3,ENABLE);
	}
	else
	{
		led_ctrl(LD3,DISABLE);
	}
}

void lcd_handler()
{
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"Channel(1): %-10d",TIM2_IC2_Freq);
		LCD_DisplayStringLine(Line1,lcd_str);	
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"N(1): %-10d",ch1_N);
		LCD_DisplayStringLine(Line3,lcd_str);	
	
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"Channel(2): %-10d",TIM2_IC3_Freq);
		LCD_DisplayStringLine(Line5,lcd_str);	
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"N(2): %-10d",ch2_N);
		LCD_DisplayStringLine(Line7,lcd_str);	
}

void pwm_handler(void)
{
	/*输入通道 1 为 PA1，相应的倍频输出通道为 PA6；输入通道 2 为 PA2，相应的倍频输出
		通道为 PA7。输入信号频率范围为 50Hz 到 50KHz；输入信号超出范围时，输出为低电平。*/
	if(channel_select == CH1)
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIOA->ODR &= ~GPIO_Pin_7;
		
		if(TIM2_IC2_Freq <= 50000 && TIM2_IC2_Freq >= 50)
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
			CCR1_Val = (12000000 / (ch1_N * TIM2_IC2_Freq));
		}
		else
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
			GPIOA->ODR &= ~GPIO_Pin_6;
		}
	}
	else if(channel_select == CH2)
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIOA->ODR &= ~GPIO_Pin_6;
		
		if(TIM2_IC3_Freq <= 50000 && TIM2_IC3_Freq >= 50)
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
			CCR2_Val = (12000000 / (ch2_N * TIM2_IC3_Freq) );
		}
		else
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
			GPIOA->ODR &= ~GPIO_Pin_7;
		}
	}
}

void usart_setting_handler(void)
{
	if(RxFlag)
	{
		RxFlag = 0;
		if(setting_mode == USART_SETTING)
		{
			if(RxBuffer[0] == 'S' && RxBuffer[1] == 'E' && RxBuffer[2] == 'T' && RxBuffer[3] == ':')
			{
				if(RxBuffer[4] == '1' && RxBuffer[5] == ':')
				{
					if(RxBuffer[6] <= '9' && RxBuffer[6] >= '1' && RxBuffer[7] == 0)
					{
						channel_select = CH1;
						ch1_N = (RxBuffer[6]-'0');
						write_at24c02(ch1_add,ch1_N);
						Delay_Ms(5);
					}
					else if(RxBuffer[6] == '0' && RxBuffer[7] <= '9' && RxBuffer[7] >= '1')
					{
						channel_select = CH1;
						ch1_N = (RxBuffer[7]-'0');
						write_at24c02(ch1_add,ch1_N);
						Delay_Ms(5);
					}
					else if(RxBuffer[6] == '1' && RxBuffer[7] == '0')
					{
						channel_select = CH1;
						ch1_N = 10;
						write_at24c02(ch1_add,ch1_N);
						Delay_Ms(5);
					}
				}
				else if(RxBuffer[4] == '2' && RxBuffer[5] == ':')
				{
					if(RxBuffer[6] <= '9' && RxBuffer[6] >= '1' && RxBuffer[7] == 0)
					{
						channel_select = CH2;
						ch2_N = (RxBuffer[6]-'0');
						write_at24c02(ch2_add,ch2_N);
						Delay_Ms(5);
					}
					else if(RxBuffer[6] == '0' && RxBuffer[7] <= '9' && RxBuffer[7] >= '1')
					{
						channel_select = CH2;
						ch2_N = (RxBuffer[7]-'0');
						write_at24c02(ch2_add,ch2_N);
						Delay_Ms(5);
					}
					else if(RxBuffer[6] == '1' && RxBuffer[7] == '0')
					{
						channel_select = CH2;
						ch2_N = 10;
						write_at24c02(ch2_add,ch2_N);
						Delay_Ms(5);
					}
				}
			}
		}
		memset(RxBuffer,0,sizeof(RxBuffer));
	}
}

void key_scan()
{
	key_refresh();
	
	if(key_falling == B1)		// 用于切换“本地设定” 和“串口设定”
	{
		setting_mode = !setting_mode;
	}
	else if(key_falling == B2 && setting_mode == LOCAL_SETTING)		// 用于在两个通道之间来回切换
	{
		channel_select = !channel_select;
	}
	else if(key_falling == B3 && setting_mode == LOCAL_SETTING)		// 对当前通道的倍频数减 1，减到 1 时停止减数
	{
		if(channel_select == CH1)
		{
			if(ch1_N > 1)
			{
				ch1_N--;
				write_at24c02(ch1_add,ch1_N);
				Delay_Ms(5);
			}
		}
		else if(channel_select == CH2)
		{
			if(ch2_N > 1)	
			{
				ch2_N--;
				write_at24c02(ch2_add,ch2_N);
				Delay_Ms(5);
			}
		}
		
	}
	else if(key_falling == B4 && setting_mode == LOCAL_SETTING)		// 对当前通道的倍频数加 1，加到 10 时停止加数
	{
		if(channel_select == CH1)
		{
			if(ch1_N < 10)
			{
				ch1_N++;
				write_at24c02(ch1_add,ch1_N);
				Delay_Ms(5);
			}
		}
		else if(channel_select == CH2)
		{
			if(ch2_N < 10)
			{
				ch2_N++;
				write_at24c02(ch2_add,ch2_N);
				Delay_Ms(5);
			}
		}
	}
}

//Main Body
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	SysTick_Config(SystemCoreClock/1000);
	
	Delay_Ms(200);
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	led_init();
	tim_init();
	key_init();
	i2c_init();
	uart_init();
	pwm_init();
	InputCapture_init();

	ch1_N = read_at24c02(ch1_add);
	ch2_N = read_at24c02(ch2_add);
	
	while(1)
	{
		lcd_handler();

		if(key_flag)
		{
			key_flag = 0;
			key_scan();
		}
		usart_setting_handler();
		pwm_handler();
		led_handler();
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
	
		if(++key_cnt == 10)
		{
			key_cnt = 0;
			key_flag = 1;
		}
		
		if(DataFlag && ++usart_ticker >= 50)
		{
			usart_ticker = 0;
			DataFlag = 0;
//			memset(lcd_str,0,sizeof(lcd_str));
//			sprintf((char*)lcd_str,"%-20.20s",RxBuffer );
//			LCD_DisplayStringLine(Line9,lcd_str);	
			RxFlag = 1;
			RxCnt = 0;
		}
  }
}

void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		RxBuffer[RxCnt++] = USART_ReceiveData(USART2);
		usart_ticker = 0;
		DataFlag = 1;
//		USART_SendData(USART2, (uint8_t) RxBuffer[RxCnt - 1]);
//		/* Loop until the end of transmission */
//		while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
//		{}
	}
}
