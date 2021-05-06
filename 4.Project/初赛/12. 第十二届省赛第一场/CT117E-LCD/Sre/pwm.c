#include "pwm.h"

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void PWM_RCC_Configuration(void)
{
  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  /* GPIOA and GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
}

/**
  * @brief  Configure the TIM3 Ouput Channels.
  * @param  None
  * @retval None
  */
void PWM_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* GPIOA Configuration:TIM3 Channel2 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void pwm_init(FunctionalState NewState)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	uint16_t PrescalerValue = 0;
	     
  /* System Clocks Configuration */
  PWM_RCC_Configuration();

  /* GPIO Configuration */
  PWM_GPIO_Configuration();

  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 10000000) - 1;
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 5000;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	if(NewState == ENABLE)
  {
		TIM_OCInitTypeDef  TIM_OCInitStructure;
		uint16_t CCR2_Val = 1000;
		/* PWM1 Mode configuration: Channel2 */
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = CCR2_Val;

		TIM_OC2Init(TIM3, &TIM_OCInitStructure);

		TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

		TIM_ARRPreloadConfig(TIM3, ENABLE);
	}
	else
	{
		TIM_OC2PolarityConfig(TIM3,TIM_OCPolarity_Low);
		TIM_ForcedOC2Config(TIM3,TIM_ForcedAction_Active);
	}
  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);
}
