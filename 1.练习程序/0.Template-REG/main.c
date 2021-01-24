#include "stm32f10x.h"

int main()
{
	// ʱ��
	RCC_APB2ENR |= (1) << (4);	// ʹ��GPIOC��ʱ�� 
	RCC_APB2ENR |= (1) << (5);	// ʹ��GPIOD��ʱ�� 
	
	// GPIOD
	GPIOD_CRL &= ~((0x0F) << (4 * 2));	// ����Ϊ�������������ٶ�10MHz
	GPIOD_CRL |= (1) << (4 * 2);	// ����Ϊ�������������ٶ�10MHz
	GPIOD_ODR |= (1 << 2);
	
	// GPIOC
	GPIOC_CRH = 0x11111111;	// ����Ϊ�������������ٶ�10MHz
	GPIOC_ODR = 0xFFFFFFFF;
	
	GPIOC_ODR |= (1 << 9);
	
	GPIOC_ODR &= ~(1 << 10);
	
	
	// GPIOD
	GPIOD_ODR &= ~(1 << 2);
	
}

void SystemInit(void)
{
	// ������Ϊ�գ�Ŀ����ƭ����������ʹ�䲻����
}
