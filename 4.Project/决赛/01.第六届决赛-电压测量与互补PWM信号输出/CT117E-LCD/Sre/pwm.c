#include "pwm.h"


/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void PWM_RCC_Configuration(void)
{
  /* TIM1, GPIOA and GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
}

/**
  * @brief  Configures TIM1, TIM3 and TIM4 Pins.
  * @param  None
  * @retval None
  */
void PWM_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* GPIOA Configuration: TIM1 Channel2 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  /* GPIOB Configuration: TIM1 Channel2 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void pwm_init(uint16_t period,uint16_t pulse, FunctionalState NewState)
{   
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	TIM_DeInit(TIM1);
	
	if(NewState == ENABLE)
	{
		/* System Clocks Configuration */
		PWM_RCC_Configuration();

		/* GPIO Configuration */
		PWM_GPIO_Configuration();
		
		/* TIM1 Peripheral Configuration ----------------------------------------*/
		/* Time Base configuration */
		TIM_TimeBaseStructure.TIM_Prescaler = 5;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseStructure.TIM_Period = 12000000 / period;
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 4;

		TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

		/* Channel 1 Configuration in PWM mode */
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
		TIM_OCInitStructure.TIM_Pulse = (uint16_t)(12000000 / period / 100 * pulse);
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
		TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
		TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
		TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

		TIM_OC2Init(TIM1, &TIM_OCInitStructure);

	//  /* Automatic Output enable, Break, dead time and lock configuration*/
	//  TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
	//  TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
	//  TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
	//  TIM_BDTRInitStructure.TIM_DeadTime = 5;
	//  TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
	//  TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
	//  TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;

	//  TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);

	//  /* Master Mode selection */
	//  TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);

	//  /* Select the Master Slave Mode */
	//  TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);
		
		/* TIM1 counter enable */
		TIM_Cmd(TIM1, ENABLE);

	//  /* TIM enable counter */
	//  TIM_Cmd(TIM3, ENABLE);
	//  TIM_Cmd(TIM4, ENABLE);

		/* Main Output Enable */
		TIM_CtrlPWMOutputs(TIM1, ENABLE);
	}
}
