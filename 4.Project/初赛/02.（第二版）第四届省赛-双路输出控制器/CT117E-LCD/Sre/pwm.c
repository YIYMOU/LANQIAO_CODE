#include "pwm.h"

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void 	PWM_RCC_Configuration(void)
{
  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  /* GPIOA and GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
}

/**
  * @brief  Configure the TIM2 Ouput Channels.
  * @param  None
  * @retval None
  */
void PWM_GPIO_Configuration(uint8_t PA1, FunctionalState PA1State,uint8_t PA2, FunctionalState PA2State)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	if(PA1 == 0 || PA1State == DISABLE)
	{
		/* GPIOA Configuration:TIM2 Channel 2, 3 as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIOA->ODR &= ~GPIO_Pin_1;
	}
	else if(PA1 == 10)
	{
		/* GPIOA Configuration:TIM2 Channel 2, 3 as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIOA->ODR |= GPIO_Pin_1;
	}
	else
	{
		/* GPIOA Configuration:TIM2 Channel 2, 3 as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	
	if(PA2 == 0 || PA2State == DISABLE)
	{
		/* GPIOA Configuration:TIM2 Channel 2, 3 as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIOA->ODR &= ~GPIO_Pin_2;
	}
	else if(PA2 == 10)
	{
		/* GPIOA Configuration:TIM2 Channel 2, 3 as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIOA->ODR |= GPIO_Pin_2;
	}
	else
	{
		/* GPIOA Configuration:TIM2 Channel 2, 3 as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
}

void pwm_init(uint8_t PA1, FunctionalState PA1State,uint8_t PA2, FunctionalState PA2State)
{
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	uint16_t PrescalerValue = 0;
  /* System Clocks Configuration */
  PWM_RCC_Configuration();

  /* GPIO Configuration */
  PWM_GPIO_Configuration(PA1,PA1State,PA2,PA2State);
	
	TIM_DeInit(TIM2);

  /* -----------------------------------------------------------------------
    TIM2 Configuration: generate 4 PWM signals with 4 different duty cycles:
    The TIM2CLK frequency is set to SystemCoreClock (Hz), to get TIM2 counter
    clock at 24 MHz the Prescaler is computed as following:
     - Prescaler = (TIM2CLK / TIM2 counter clock) - 1
    SystemCoreClock is set to 72 MHz for Low-density, Medium-density, High-density
    and Connectivity line devices and to 24 MHz for Low-Density Value line and
    Medium-Density Value line devices
  ----------------------------------------------------------------------- */
  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 1000000) - 1;
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 1000 - 1;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	/* PWM1 Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = PA1 * 100;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC2Init(TIM2, &TIM_OCInitStructure);

	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
			
	/* PWM1 Mode configuration: Channel3 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = PA2 * 100;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC3Init(TIM2, &TIM_OCInitStructure);

	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
	
  TIM_ARRPreloadConfig(TIM2, ENABLE);

  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);

}
