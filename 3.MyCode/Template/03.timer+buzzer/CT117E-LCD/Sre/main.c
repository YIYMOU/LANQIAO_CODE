#include "stm32f10x.h"
#include "lcd.h"
#include "LED.h"
#include "tim.h" 
#include "buzzer.h"

u32 TimingDelay = 0;

uint16_t led_cnt = 0;
uint16_t buzzer_cnt = 0;
_Bool led_flag = 0;
_Bool buzzer_flag = 0;

void Delay_Ms(u32 nTime);

//Main Body
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	SysTick_Config(SystemCoreClock/1000);
	Delay_Ms(200);
	LED_Init();
	Buzzer_Init();
	Tim_Init();
	while(1)
	{
		if(!led_flag)
		{
			LED_Ctrl(0x01,ENABLE);
		}
		else
		{
			LED_Ctrl(0x01,DISABLE);
		}
		if(!buzzer_flag)
		{
			Buzzer_Ctrl(ENABLE);
		}
		else
		{
			Buzzer_Ctrl(DISABLE);
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
		if(++led_cnt == 500)
		{
			led_cnt = 0;
			led_flag = !led_flag;
		}
		if(++buzzer_cnt == 500)
		{
			buzzer_cnt = 0;
			buzzer_flag = !buzzer_flag;
		}
  }
}
