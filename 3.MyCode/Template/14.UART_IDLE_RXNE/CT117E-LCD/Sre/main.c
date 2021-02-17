#include "stm32f10x.h"
#include "lcd.h"
#include "uart_idle_rxne.h"
#include "stdio.h"
#include "string.h"

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
	UART_IDLE_RXNE_INIT();
	while(1);
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
