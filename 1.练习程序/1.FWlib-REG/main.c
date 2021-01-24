#include "stm32f10x.h"

int main()
{
	// 时钟
	RCC_APB2ENR |= (1) << (4);	// 使能GPIOC的时钟 
	RCC_APB2ENR |= (1) << (5);	// 使能GPIOD的时钟 
	
	// GPIOD
	GPIOD_CRL &= ~((0x0F) << (4 * 2));	// 配置为推挽输出，最大速度10MHz
	GPIOD_CRL |= (1) << (4 * 2);	// 配置为推挽输出，最大速度10MHz
	GPIOD_ODR |= (1 << 2);
	
	// GPIOC
	GPIOC_CRH = 0x11111111;	// 配置为推挽输出，最大速度10MHz
	GPIOC_ODR = 0xFFFFFFFF;
	
	GPIOC_ODR |= (1 << 9);
	
	GPIOC_ODR &= ~(1 << 10);
	
	
	// GPIOD
	GPIOD_ODR &= ~(1 << 2);
	
}

void SystemInit(void)
{
	// 函数体为空，目的是骗过编译器，使其不报错
}
