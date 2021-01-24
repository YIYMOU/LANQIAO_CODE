#include "bsp_led.h"

void LED_GPIO_Init(void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef LED_InitStucture;
	
	/*开启LED相关的GPIO外设时钟*/
	RCC_APB2PeriphClockCmd(LED_74HC593_GPIO_CLK | LED_GPIO_CLK,ENABLE);
	
	/*选择要控制的GPIO引脚*/
	LED_InitStucture.GPIO_Pin = LED_74HC593_GPIO_PIN;
	/*设置引脚模式为通用推挽输出*/
	LED_InitStucture.GPIO_Mode = GPIO_Mode_Out_PP;
	/*设置引脚速率为50MHz */   
	LED_InitStucture.GPIO_Speed = GPIO_Speed_50MHz;
	/*调用库函数，初始化GPIO*/
	GPIO_Init(LED_74HC593_GPIO_PORT,&LED_InitStucture);
	
	/*选择要控制的GPIO引脚*/
	LED_InitStucture.GPIO_Pin = LED_ALL;
	/*设置引脚模式为通用推挽输出*/
	LED_InitStucture.GPIO_Mode = GPIO_Mode_Out_PP;
	/*设置引脚速率为50MHz */   
	LED_InitStucture.GPIO_Speed = GPIO_Speed_50MHz;
	/*调用库函数，初始化GPIO*/
	GPIO_Init(LED_GPIO_PORT,&LED_InitStucture);
	
	/* 关闭所有led灯	*/
	GPIO_SetBits(LED_74HC593_GPIO_PORT,LED_74HC593_GPIO_PIN);
	GPIO_SetBits(LED_GPIO_PORT,LED_ALL);
	GPIO_ResetBits(LED_74HC593_GPIO_PORT,LED_74HC593_GPIO_PIN);
}

// 打开LEDx
void LED_ON(uint16_t LEDx)
{
	GPIO_SetBits(LED_74HC593_GPIO_PORT,LED_74HC593_GPIO_PIN);
	GPIO_ResetBits(LED_GPIO_PORT,LEDx);
	GPIO_ResetBits(LED_74HC593_GPIO_PORT,LED_74HC593_GPIO_PIN);
}

// 关闭LEDx
void LED_OFF(uint16_t LEDx)
{
	GPIO_SetBits(LED_74HC593_GPIO_PORT,LED_74HC593_GPIO_PIN);
	GPIO_SetBits(LED_GPIO_PORT,LEDx);
	GPIO_ResetBits(LED_74HC593_GPIO_PORT,LED_74HC593_GPIO_PIN);
}

// LEDx状态翻转
void LED_TOGGLE(uint16_t LEDx)
{
	if(GPIO_ReadOutputDataBit(LED_GPIO_PORT,LEDx) == Bit_RESET)
	{
		LED_OFF(LEDx);
	}
	else
	{
		LED_ON(LEDx);
	}
}

// 关闭所有LED灯
void LED_ALL_OFF(uint16_t LEDx)
{
	GPIO_SetBits(LED_74HC593_GPIO_PORT,LED_74HC593_GPIO_PIN);
	GPIO_SetBits(LED_GPIO_PORT,LED_ALL);
	GPIO_ResetBits(LED_74HC593_GPIO_PORT,LED_74HC593_GPIO_PIN);
}

