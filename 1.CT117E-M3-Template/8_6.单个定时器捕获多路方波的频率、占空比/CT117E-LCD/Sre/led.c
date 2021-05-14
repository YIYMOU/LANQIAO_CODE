/*
 * 简介：LED相关程序
 * Copyright (c) 2020 电子设计工坊 dianshe.taobao.com
 * All rights reserved
 */
#include "led.h"

void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD,&GPIO_InitStructure);	
	
	LED_Control(0xff,0);
}

//GPIO_ResetBits 把对应的Pin清0	--by cc
//GPIO_SetBits 	 把对应的Pin置1	--by cc
void LED_Control(u16 led_ctrl,u8 led_status)//为什么要以u16输入呢？ 因为0x01<<8的话，会超过u8的范围！ --by cc
{
	if(led_status!=0)
	{
		GPIO_ResetBits(GPIOC,led_ctrl<<8);  //0xfe<<8 :1111 1110 0000 00000 ；   0X01<<8->LD1
		GPIO_SetBits(GPIOD,GPIO_Pin_2); 	//PD2->H GPIO_Pin_2：0x0004：		0000 0000 0000 0100
		GPIO_ResetBits(GPIOD,GPIO_Pin_2);	//PD2->L
	}
	else
	{
		GPIO_SetBits(GPIOC,led_ctrl<<8);  	//0xfe<<8 :1111 1110 0000 00000 ；   0X01<<8->LD1
		GPIO_SetBits(GPIOD,GPIO_Pin_2); 	//PD2->H GPIO_Pin_2：0x0004：		0000 0000 0000 0100
		GPIO_ResetBits(GPIOD,GPIO_Pin_2);	//PD2->L
	}
}
