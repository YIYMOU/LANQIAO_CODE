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
  /* GPIOA Configuration:TIM3 Channel1 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void pwm_init(uint16_t fre)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	uint16_t PrescalerValue = 0;
	
	TIM_DeInit(TIM3);
	
	/* System Clocks Configuration */
  PWM_RCC_Configuration();

  /* GPIO Configuration */
  PWM_GPIO_Configuration();
	if(fre == 0)
	{
		TIM_OC1PolarityConfig(TIM3,TIM_OCPolarity_Low);
		TIM_ForcedOC1Config(TIM3,TIM_ForcedAction_Active);
	}
	else
	{
		/* -----------------------------------------------------------------------
			TIM3 Configuration: generate 4 PWM signals with 4 different duty cycles:
			The TIM3CLK frequency is set to SystemCoreClock (Hz), to get TIM3 counter
			clock at 24 MHz the Prescaler is computed as following:
			 - Prescaler = (TIM3CLK / TIM3 counter clock) - 1
			SystemCoreClock is set to 72 MHz for Low-density, Medium-density, High-density
			and Connectivity line devices and to 24 MHz for Low-Density Value line and
			Medium-Density Value line devices

			The TIM3 is running at 36 KHz: TIM3 Frequency = TIM3 counter clock/(ARR + 1)
																										= 24 MHz / 666 = 36 KHz
		----------------------------------------------------------------------- */
		/* Compute the prescaler value */
		PrescalerValue = (uint16_t) (SystemCoreClock / 10000) - 1;
		/* Time base configuration */
		TIM_TimeBaseStructure.TIM_Period = 10000 / fre;
		TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

		/* PWM1 Mode configuration: Channel1 */
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = 5000 / fre;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

		TIM_OC1Init(TIM3, &TIM_OCInitStructure);

		TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

		TIM_ARRPreloadConfig(TIM3, ENABLE);
	}
	/* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);
}
