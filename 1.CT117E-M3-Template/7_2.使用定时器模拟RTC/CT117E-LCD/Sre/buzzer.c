/*
 * ��飺��������ʼ��
 * Copyright (c) 2018 ������ƹ��� dianshe.taobao.com
 * All rights reserved
 */
#include "buzzer.h"

void Buzzer_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);	//��������ʱ��
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST,ENABLE);							//����NoJTRST��ʹ��PB4����ͨio
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}
