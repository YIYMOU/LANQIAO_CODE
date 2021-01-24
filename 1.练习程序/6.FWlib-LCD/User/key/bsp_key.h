#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "stm32f10x.h"

#define KEY1_GPIO_PORT			GPIOA
#define KEY2_GPIO_PORT			GPIOA
#define KEY3_GPIO_PORT			GPIOB
#define KEY4_GPIO_PORT			GPIOB

#define KEY1_GPIO_CLK			RCC_APB2Periph_GPIOA
#define KEY2_GPIO_CLK			RCC_APB2Periph_GPIOA
#define KEY3_GPIO_CLK			RCC_APB2Periph_GPIOB
#define KEY4_GPIO_CLK			RCC_APB2Periph_GPIOB

#define KEY1_GPIO_PIN			GPIO_Pin_0
#define KEY2_GPIO_PIN			GPIO_Pin_8
#define KEY3_GPIO_PIN			GPIO_Pin_1
#define KEY4_GPIO_PIN			GPIO_Pin_2

#define KEY1		((uint16_t)0x0001)
#define KEY2		((uint16_t)0x0002)
#define KEY3		((uint16_t)0x0003)
#define KEY4		((uint16_t)0x0004)

#define GET_KEY1_STATE		(!GPIO_ReadInputDataBit(KEY1_GPIO_PORT,KEY1_GPIO_PIN))
#define GET_KEY2_STATE		(!GPIO_ReadInputDataBit(KEY2_GPIO_PORT,KEY2_GPIO_PIN))
#define GET_KEY3_STATE		(!GPIO_ReadInputDataBit(KEY3_GPIO_PORT,KEY3_GPIO_PIN))
#define GET_KEY4_STATE		(!GPIO_ReadInputDataBit(KEY4_GPIO_PORT,KEY4_GPIO_PIN))
#define GET_ALL_KEY_STATE	(GET_KEY1_STATE || GET_KEY2_STATE || GET_KEY3_STATE || GET_KEY4_STATE)



void Key_Init(void);
uint16_t Key_Scan(void);

#endif /*__BSP_KEY_H*/

