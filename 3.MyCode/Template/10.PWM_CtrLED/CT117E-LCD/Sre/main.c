#include "stm32f10x.h"
#include "lcd.h"
#include "LED.h"
#include "tim.h"

u32 TimingDelay = 0;

_Bool PWM_LED_flag = 0;

uint8_t PWM_LED_CNT = 0;

uint8_t PWM_LED_VALUE = 1;

uint16_t change_cnt = 0;

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
	while(1){};
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
		
		if(++PWM_LED_CNT < PWM_LED_VALUE)
		{
			LED_Ctrl(0x01,ENABLE);
		}
		else if(PWM_LED_CNT >= PWM_LED_VALUE && PWM_LED_CNT != 10)
		{
			LED_Ctrl(0x01,DISABLE);
		}
		else if(PWM_LED_CNT >= 10)
		{
			PWM_LED_CNT = 0;
		}
		
		if(++change_cnt >= 1000)
		{
			change_cnt = 0;
			PWM_LED_VALUE += 2;
			if(PWM_LED_VALUE >= 10) PWM_LED_VALUE = 0;
		}
  }
}
