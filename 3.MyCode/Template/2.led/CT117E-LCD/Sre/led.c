#include "led.h"

void LED_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* Enable the GPIO_LED Clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);

  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	// 初始化LED，将所有的LED都关闭
	GPIO_SetBits(GPIOD,GPIO_Pin_2);
	GPIO_SetBits(GPIOC,GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
	GPIO_ResetBits(GPIOD,GPIO_Pin_2);
	
}

void LED_Ctrl(uint16_t LEDx,FunctionalState NewState)
{
	if(NewState == ENABLE)
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_2);
		GPIO_ResetBits(GPIOC,LEDx << 8);
		GPIO_ResetBits(GPIOD,GPIO_Pin_2);
	}
	else
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_2);
		GPIO_SetBits(GPIOC,LEDx << 8);
		GPIO_ResetBits(GPIOD,GPIO_Pin_2);
	}
}

