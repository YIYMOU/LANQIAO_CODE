#include "bsp_led.h"

void LED_GPIO_Init(void)
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef LED_InitStucture;
	
	/*����LED��ص�GPIO����ʱ��*/
	RCC_APB2PeriphClockCmd(LED_74HC593_GPIO_CLK | LED_GPIO_CLK,ENABLE);
	
	/*ѡ��Ҫ���Ƶ�GPIO����*/
	LED_InitStucture.GPIO_Pin = LED_74HC593_GPIO_PIN;
	/*��������ģʽΪͨ���������*/
	LED_InitStucture.GPIO_Mode = GPIO_Mode_Out_PP;
	/*������������Ϊ50MHz */   
	LED_InitStucture.GPIO_Speed = GPIO_Speed_50MHz;
	/*���ÿ⺯������ʼ��GPIO*/
	GPIO_Init(LED_74HC593_GPIO_PORT,&LED_InitStucture);
	
	/*ѡ��Ҫ���Ƶ�GPIO����*/
	LED_InitStucture.GPIO_Pin = LED_ALL;
	/*��������ģʽΪͨ���������*/
	LED_InitStucture.GPIO_Mode = GPIO_Mode_Out_PP;
	/*������������Ϊ50MHz */   
	LED_InitStucture.GPIO_Speed = GPIO_Speed_50MHz;
	/*���ÿ⺯������ʼ��GPIO*/
	GPIO_Init(LED_GPIO_PORT,&LED_InitStucture);
	
	/* �ر�����led��	*/
	GPIO_SetBits(LED_74HC593_GPIO_PORT,LED_74HC593_GPIO_PIN);
	GPIO_SetBits(LED_GPIO_PORT,LED_ALL);
	GPIO_ResetBits(LED_74HC593_GPIO_PORT,LED_74HC593_GPIO_PIN);
}

// ��LEDx
void LED_ON(uint16_t LEDx)
{
	GPIO_SetBits(LED_74HC593_GPIO_PORT,LED_74HC593_GPIO_PIN);
	GPIO_ResetBits(LED_GPIO_PORT,LEDx);
	GPIO_ResetBits(LED_74HC593_GPIO_PORT,LED_74HC593_GPIO_PIN);
}

// �ر�LEDx
void LED_OFF(uint16_t LEDx)
{
	GPIO_SetBits(LED_74HC593_GPIO_PORT,LED_74HC593_GPIO_PIN);
	GPIO_SetBits(LED_GPIO_PORT,LEDx);
	GPIO_ResetBits(LED_74HC593_GPIO_PORT,LED_74HC593_GPIO_PIN);
}

// LEDx״̬��ת
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

// �ر�����LED��
void LED_ALL_OFF(uint16_t LEDx)
{
	GPIO_SetBits(LED_74HC593_GPIO_PORT,LED_74HC593_GPIO_PIN);
	GPIO_SetBits(LED_GPIO_PORT,LED_ALL);
	GPIO_ResetBits(LED_74HC593_GPIO_PORT,LED_74HC593_GPIO_PIN);
}

