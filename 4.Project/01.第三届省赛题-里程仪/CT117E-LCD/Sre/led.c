#include "led.h"

void Led_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC , ENABLE);
  
	GPIO_InitStructure.GPIO_Pin = 0xff00;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOC,0xff00);
	GPIO_SetBits(GPIOD,GPIO_Pin_2);
	GPIO_ResetBits(GPIOD,GPIO_Pin_2);
}

void Led_Ctrl(uint16_t LEDx,FunctionalState NewState)
{
	if(NewState == ENABLE)
		GPIO_ResetBits(GPIOC,LEDx << 8);
	else
		GPIO_SetBits(GPIOC,LEDx << 8);
	
	GPIO_SetBits(GPIOD,GPIO_Pin_2);
	GPIO_ResetBits(GPIOD,GPIO_Pin_2);
}

void Led_Toggle(uint16_t LEDx)
{
	GPIOC->ODR ^= LEDx << 8;
	GPIO_SetBits(GPIOD,GPIO_Pin_2);
	GPIO_ResetBits(GPIOD,GPIO_Pin_2);
}
