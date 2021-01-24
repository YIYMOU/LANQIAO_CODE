#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "stm32f10x.h"

/* 定义LED连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的LED引脚 */
#define LED_GPIO_PORT					GPIOC			 						/* GPIO端口 */
#define LED_GPIO_CLK					RCC_APB2Periph_GPIOC	/* GPIO端口时钟 */
#define LED_1_GPIO_PIN				GPIO_Pin_8
#define LED_2_GPIO_PIN				GPIO_Pin_9
#define LED_3_GPIO_PIN				GPIO_Pin_10
#define LED_4_GPIO_PIN				GPIO_Pin_11
#define LED_5_GPIO_PIN				GPIO_Pin_12
#define LED_6_GPIO_PIN				GPIO_Pin_13
#define LED_7_GPIO_PIN				GPIO_Pin_14
#define LED_8_GPIO_PIN				GPIO_Pin_15
#define LED_1									GPIO_Pin_8
#define LED_2									GPIO_Pin_9
#define LED_3									GPIO_Pin_10
#define LED_4									GPIO_Pin_11
#define LED_5									GPIO_Pin_12
#define LED_6									GPIO_Pin_13
#define LED_7									GPIO_Pin_14
#define LED_8									GPIO_Pin_15
#define LED_ALL								LED_1_GPIO_PIN | LED_2_GPIO_PIN \
														| LED_3_GPIO_PIN | LED_4_GPIO_PIN \
														| LED_5_GPIO_PIN | LED_6_GPIO_PIN \
														| LED_7_GPIO_PIN | LED_8_GPIO_PIN

#define LED_74HC593_GPIO_PORT		GPIOD
#define LED_74HC593_GPIO_PIN		GPIO_Pin_2
#define LED_74HC593_GPIO_CLK		RCC_APB2Periph_GPIOD

void LED_GPIO_Init(void);
void LED_ON(uint16_t LEDx);
void LED_OFF(uint16_t LEDx);
void LED_TOGGLE(uint16_t LEDx);
void LED_ALL_OFF(uint16_t LEDx);

#endif /*__BSP_LED_H*/
