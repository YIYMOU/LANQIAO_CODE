#include "buzzer.h"

void Buzzer_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* Enable the GPIO_BUZZER Clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST,ENABLE);
  /* Configure the GPIO_BUZZER pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOB, &GPIO_InitStructure);
}
void Buzzer_Ctrl(FunctionalState NewState)
{
	if(NewState != ENABLE)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_4);
	}
	else
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_4);
	}
}
