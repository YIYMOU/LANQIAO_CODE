#include "led.h"

void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD , ENABLE);
  
	GPIO_InitStructure.GPIO_Pin = LD_ALL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIOC->ODR |= LD_ALL;
	GPIOD->ODR |= GPIO_Pin_2;
	GPIOD->ODR &= ~GPIO_Pin_2;
}

void led_ctrl(uint16_t LEDx,FunctionalState NewState)
{
	if(NewState == ENABLE)
		GPIOC->ODR &= ~LEDx;
	else
		GPIOC->ODR |= LEDx;
	GPIOD->ODR |= GPIO_Pin_2;
	GPIOD->ODR &= ~GPIO_Pin_2;
}

void led_toggle(uint16_t LEDx)
{
	GPIOC->ODR ^= LEDx;
	GPIOD->ODR |= GPIO_Pin_2;
	GPIOD->ODR &= ~GPIO_Pin_2;
}
