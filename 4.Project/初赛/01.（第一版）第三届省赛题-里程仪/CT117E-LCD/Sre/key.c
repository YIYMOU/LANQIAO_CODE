#include "key.h"

uint8_t falling_flag = 0;
uint8_t rising_flag = 0;
uint8_t key_state = 0;

void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB , ENABLE);
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
void Key_Read(void)
{
	uint8_t key_temp = 0xf0;
	key_temp |= key_read;
	key_temp ^= 0xff;
	rising_flag = (!key_temp) * (key_temp ^ key_state);
	falling_flag = key_temp & (key_temp ^ key_state);
	key_state = key_temp;
}
