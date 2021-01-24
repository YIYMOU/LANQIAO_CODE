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
	key_Value = key_Value ^ 0xff;		// key1按下，key_state = 0x01,如果没有按键按下，则key_Value = 0
	
	//Key_Trg = (!key_Value) * (key_Value ^ key_State); // 检测按键的下降沿
	
	Key_Trg = key_Value & (key_Value ^ key_State); // 检测按键的上降沿
	// 情况1：key1按键第一次按下，此时key_State，Key_Trg = 0x01
	// 情况2：key1按键持续按下，第二次扫描的时候，此时key_State = 0x01，Key_Trg = 0
	// 情况3：如果没有按键按下，则key_Value = 0，Key_Trg = 0
	// 由以上的分析可以看出，Key_Trg只有在按键的下降沿那一时刻有值。
	key_State = key_Value;
	// key_State即保存的是扫描过后的按键的值
}
