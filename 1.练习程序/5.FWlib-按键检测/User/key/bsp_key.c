#include "bsp_key.h"

void Key_Init(void)
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef Key_InitStucture;
	
	/*����KEY��ص�GPIO����ʱ��*/
	RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK | KEY3_GPIO_CLK,ENABLE);
	
	/*ѡ��Ҫ���Ƶ�GPIO����*/
	Key_InitStucture.GPIO_Pin = KEY1_GPIO_PIN | KEY2_GPIO_PIN;
	Key_InitStucture.GPIO_Speed = GPIO_Speed_50MHz;
	/*���ڿ���������������裬��������ģʽΪ��������*/
	Key_InitStucture.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	/*���ÿ⺯������ʼ��GPIO*/
	GPIO_Init(KEY1_GPIO_PORT,&Key_InitStucture);
	
	/*ѡ��Ҫ���Ƶ�GPIO����*/
	Key_InitStucture.GPIO_Pin = KEY3_GPIO_PIN | KEY4_GPIO_PIN;
	Key_InitStucture.GPIO_Speed = GPIO_Speed_50MHz;
	/*���ڿ���������������裬��������ģʽΪ��������*/
	Key_InitStucture.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	/*���ÿ⺯������ʼ��GPIO*/
	GPIO_Init(KEY3_GPIO_PORT,&Key_InitStucture);
	
}
void Delay_KEY(unsigned int ms)
{
	unsigned int i, j;
	for(i=0; i<ms; i++)
		for(j=0; j<7992; j++); // SYSCLK = 72MHz
		// for(j=0; j<1598; j++); // SYSCLK = 8MHz
}

//��������
//����ֵΪ��Ӧ�ļ�ֵ������KEY1��reture 1������KEY2��reture 2���Դ����ơ�
unsigned char KEY_Scan(void)
{	
	unsigned char key_val=0;//����ֲ���������ż�ֵ
	if(((key1==0)||(key2==0)||(key3==0)||(key4==0))==1)//�����һ��������
	{
		Delay_KEY(10);
		if(((key1==0)||(key2==0)||(key3==0)||(key4==0))==1)//�����һ��������
		{
			if(key1==0)key_val=1;//����KEY1��key_val=1
			else if(key2==0)key_val=2;//����KEY1��key_val=2
			else if(key3==0)key_val=3;//����KEY1��key_val=3
			else if(key4==0)key_val=4;//����KEY1��key_val=4
		}
	}
	return key_val;//����key_val(��ֵ)
}

