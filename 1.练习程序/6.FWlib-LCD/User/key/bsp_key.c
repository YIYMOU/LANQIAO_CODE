#include "bsp_key.h"
#include "bsp_systick.h"

void Key_Init(void)
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef Key_InitStucture;
	
	/*����KEY��ص�GPIO����ʱ��*/
	RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK | KEY3_GPIO_CLK,ENABLE);
	
	/*ѡ��Ҫ���Ƶ�GPIO����*/
	Key_InitStucture.GPIO_Pin = KEY1_GPIO_PIN | KEY2_GPIO_PIN;
	/*���ڿ���������������裬��������ģʽΪ��������*/
	Key_InitStucture.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	/*���ÿ⺯������ʼ��GPIO*/
	GPIO_Init(KEY1_GPIO_PORT,&Key_InitStucture);
	
	/*ѡ��Ҫ���Ƶ�GPIO����*/
	Key_InitStucture.GPIO_Pin = KEY3_GPIO_PIN | KEY4_GPIO_PIN;
	/*���ڿ���������������裬��������ģʽΪ��������*/
	Key_InitStucture.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	/*���ÿ⺯������ʼ��GPIO*/
	GPIO_Init(KEY3_GPIO_PORT,&Key_InitStucture);
	
}

uint16_t Key_Scan(void)
{
	unsigned char ucKey_Val = 0;
	// �ж� B1 �� B2 �Ƿ���
	if(~GPIO_ReadInputData(GPIOA) & 0x101)
	{
		SysTick_Delay_ms(10); // ��ʱ 10ms ����
		if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)) // B1 ����
		ucKey_Val = KEY1;
		if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8)) // B2 ����
		ucKey_Val = KEY2;
		while(~GPIO_ReadInputData(GPIOA) & 0x101);
	}
	// �ж� B3 �� B4 �Ƿ���
	else if(~GPIO_ReadInputData(GPIOB) & 6)
	{
		SysTick_Delay_ms(10); // ��ʱ 10ms ����
		if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)) // B3 ����
		ucKey_Val = KEY3;
		if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2)) // B4 ����
		ucKey_Val = KEY4;
		while(~GPIO_ReadInputData(GPIOB) & 6);
	}
	return ucKey_Val;
}

