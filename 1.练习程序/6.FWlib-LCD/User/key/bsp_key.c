#include "bsp_key.h"
#include "bsp_systick.h"

void Key_Init(void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef Key_InitStucture;
	
	/*开启KEY相关的GPIO外设时钟*/
	RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK | KEY3_GPIO_CLK,ENABLE);
	
	/*选择要控制的GPIO引脚*/
	Key_InitStucture.GPIO_Pin = KEY1_GPIO_PIN | KEY2_GPIO_PIN;
	/*由于开发板接了上拉电阻，设置引脚模式为浮空输入*/
	Key_InitStucture.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	/*调用库函数，初始化GPIO*/
	GPIO_Init(KEY1_GPIO_PORT,&Key_InitStucture);
	
	/*选择要控制的GPIO引脚*/
	Key_InitStucture.GPIO_Pin = KEY3_GPIO_PIN | KEY4_GPIO_PIN;
	/*由于开发板接了上拉电阻，设置引脚模式为浮空输入*/
	Key_InitStucture.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	/*调用库函数，初始化GPIO*/
	GPIO_Init(KEY3_GPIO_PORT,&Key_InitStucture);
	
}

uint16_t Key_Scan(void)
{
	unsigned char ucKey_Val = 0;
	// 判断 B1 和 B2 是否按下
	if(~GPIO_ReadInputData(GPIOA) & 0x101)
	{
		SysTick_Delay_ms(10); // 延时 10ms 消抖
		if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)) // B1 按下
		ucKey_Val = KEY1;
		if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8)) // B2 按下
		ucKey_Val = KEY2;
		while(~GPIO_ReadInputData(GPIOA) & 0x101);
	}
	// 判断 B3 和 B4 是否按下
	else if(~GPIO_ReadInputData(GPIOB) & 6)
	{
		SysTick_Delay_ms(10); // 延时 10ms 消抖
		if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)) // B3 按下
		ucKey_Val = KEY3;
		if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2)) // B4 按下
		ucKey_Val = KEY4;
		while(~GPIO_ReadInputData(GPIOB) & 6);
	}
	return ucKey_Val;
}

