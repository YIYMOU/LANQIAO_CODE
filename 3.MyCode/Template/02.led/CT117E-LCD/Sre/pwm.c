#include "pwm.h"

void PWM_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* GPIOA Configuration:TIM2 Channel2, 3 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void PWM_RCC_Configuration(void)
{
  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  /* GPIOA clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_AFIO, ENABLE);
}
void PWM_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	uint16_t CCR2_Val = 100;
	uint16_t CCR3_Val = 900;
	uint16_t PrescalerValue = 0;
	PWM_RCC_Configuration();
	PWM_GPIO_Configuration();
	
	/* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 1000000) - 1;
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 1000 - 1;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	// OC:Output Compare 输出比较
	
	//	输出比较模式时的TIM_OCMode_PWM1和TIM_OCMode_PWM2区别
	
	// 现在假定TIM_OCInitTypeDef.TIM_OCPolarity = TIM_OCPolarity_High,则起始波形为高电位。

  // 若TIM_OCInitTypeDef.TIM_OCMode = TIM_OCMode_PWM1时：

  // 当计时器值小于比较器设定值时则TIMX输出脚此时输出有效高电位。

  // 当计时器值大于或等于比较器设定值时则TIMX输出脚此时输出低电位。

  // 若TIM_OCInitTypeDef.TIM_OCMode = TIM_OCMode_PWM2时：

  //   当计时器值小于比较器设定值时则TIMX输出脚此时输出有效低电位。

  //   当计时器值大于或等于比较器设定值时则TIMX输出脚此时输出高电位。
  
	/* PWM1 Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	// Polarity:极性
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;

  TIM_OC2Init(TIM2, &TIM_OCInitStructure);

  TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);

  /* PWM1 Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;

  TIM_OC3Init(TIM2, &TIM_OCInitStructure);

  TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	/* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);
}
