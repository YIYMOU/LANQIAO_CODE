/*
 * 简介：独立按键 - 三行程序
 * Copyright (c) 2018 电子设计工坊 dianshe.taobao.com
 * All rights reserved
 */

#include "key.h"

unsigned char Trg;
unsigned char Cont;

void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void Key_Read( void )
{
    unsigned char ReadData = (KEYPORT)^0xff;  		   // 1. Read KEYPORT ^(xor)
    Trg = ReadData & (ReadData ^ Cont);      		   // 2 
    Cont = ReadData;                         		   // 3
}
