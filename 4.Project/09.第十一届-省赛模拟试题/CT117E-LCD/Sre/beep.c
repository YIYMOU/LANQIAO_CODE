#include "led.h"

void beep_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST,ENABLE);
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIOB->ODR |= GPIO_Pin_4;
}

void beep_ctrl(FunctionalState NewState)
{
	if(NewState == ENABLE)
		GPIOB->ODR &= ~GPIO_Pin_4;
	else
		GPIOB->ODR |= GPIO_Pin_4;
}
