#include "key.h"

uint8_t Key_Trg;
uint8_t key_State;

void KEY_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* Enable the KEY Clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

  /* Configure the KEY pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void Key_Read(void)
{
	uint8_t key_Value = 0xF0;
	key_Value |= GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) | (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8) << 1) | \
					(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) << 2) | (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2) << 3);
	key_Value = key_Value ^ 0xff;		// key1���£�key_state = 0x01,���û�а������£���key_Value = 0
	
	//Key_Trg = (!key_Value) * (key_Value ^ key_State); // ��ⰴ�����½���
	
	Key_Trg = key_Value & (key_Value ^ key_State); // ��ⰴ�����Ͻ���
	// ���1��key1������һ�ΰ��£���ʱkey_State��Key_Trg = 0x01
	// ���2��key1�����������£��ڶ���ɨ���ʱ�򣬴�ʱkey_State = 0x01��Key_Trg = 0
	// ���3�����û�а������£���key_Value = 0��Key_Trg = 0
	// �����ϵķ������Կ�����Key_Trgֻ���ڰ������½�����һʱ����ֵ��
	key_State = key_Value;
	// key_State���������ɨ�����İ�����ֵ
}
