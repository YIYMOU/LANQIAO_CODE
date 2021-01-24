#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "core_cm3.h"


//����ȫ�ֱ���
unsigned long TimingDelay=0;//���ڵδ�ʱ������
unsigned char key_val=0;//���ڶ�ȡ��ֵ

int main(void)
{
	LED_GPIO_Init();
	Key_Init();
	SysTick_Config(SystemCoreClock / 1000);
	
	while(1)
	{
		key_val = KEY_Scan();
//		if(key_val==1)	{LED_GPIO_Init(); LED_ON(LED_1);}//����KEY1�����е�Ϩ��LED1��
//		if(key_val==2)	{LED_GPIO_Init(); LED_ON(LED_2);}//����KEY2�����е�Ϩ��LED2��
//		if(key_val==3)	{LED_GPIO_Init(); LED_ON(LED_3);}//����KEY3�����е�Ϩ��LED3��
//		if(key_val==4)	{LED_GPIO_Init(); LED_ON(LED_4);}//����KEY4�����е�Ϩ��LED4��
		if(key_val==1)	{LED_TOGGLE(LED_1);}//����KEY1�����е�Ϩ��LED1��
		if(key_val==2)	{LED_TOGGLE(LED_2);}//����KEY2�����е�Ϩ��LED2��
		if(key_val==3)	{LED_TOGGLE(LED_3);}//����KEY3�����е�Ϩ��LED3��
		if(key_val==4)	{LED_TOGGLE(LED_4);}//����KEY4�����е�Ϩ��LED4��
	}
}

void SysTick_Handler(void)//�δ�ʱ������
{
	TimingDelay++;//��ʼ��ʱ
	if(TimingDelay<100)//0~100ms
	{
		LED_ON(LED_8);//LED2��
	}
	if(TimingDelay==200)//���������200ms
	{
		TimingDelay=0;//�ֻص�0����0��ʼ��ʱ
	}
	if(TimingDelay>100)//100ms~200ms
	{
		LED_OFF(LED_8);;//LED2��
	}
}
