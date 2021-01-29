#include "stm32f10x.h"
#include "lcd.h"
#include "LED.h"
#include "tim.h" 
#include "stdio.h"
#include "string.h"
#include "uart.h"

u32 TimingDelay = 0;

_Bool uart_display_flag = 0;

uint8_t lcd_buf[20];

uint8_t lcd_buf2[20];

void Delay_Ms(u32 nTime);

//Main Body
int main(void)
{
//	uint8_t i;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	SysTick_Config(SystemCoreClock/1000);
	Delay_Ms(200);
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	LED_Init();
	Tim_Init();
	UART_INIT();
	while(1)
	{
		if(Rx_flag)
		{
			LED_Ctrl(0x01,ENABLE);
		}
		else
		{
			LED_Ctrl(0x01,DISABLE);
		}
		if(uart_display_flag)
		{
			uart_display_flag = 0;
			memset(lcd_buf2,0,sizeof(lcd_buf2));
			sprintf((char*)lcd_buf2,"%-20.20s",lcd_buf);
			LCD_DisplayStringLine(Line4,lcd_buf2);
			printf("%s\r\n",lcd_buf2);
		}
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}

void TIM4_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		
		if(Rx_flag && ++uart_cnt >= 50)
		{
			uart_display_flag = 1;
			uart_cnt = 0;
			RxCounter = 0;
			Rx_flag = 0;
			strcpy((char*)lcd_buf,(char*)RxBuffer);
			memset(RxBuffer,0,sizeof(RxBuffer));
		}
  }
}
