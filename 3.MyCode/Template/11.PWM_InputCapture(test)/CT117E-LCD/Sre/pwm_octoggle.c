#include "pwm_octoggle.h"
/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void PWM_OCTOGGLE_RCC_Configuration(void)
{
  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  /* GPIOA clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
}

/**
  * @brief  Configure the TIM2 Pins.
  * @param  None
  * @retval None
  */
void PWM_OCTOGGLE_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /*GPIOB Configuration: TIM2 channel2, 3*/
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1 | GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void PWM_OCTOGGLE_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the TIM3 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

uint16_t CCR2_Val = 16384;
uint16_t CCR3_Val = 8192;

void PWM_OCTOGGLE_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	uint16_t PrescalerValue = 0;
	
  /* System Clocks Configuration */
  PWM_OCTOGGLE_RCC_Configuration();

  /* NVIC Configuration */
  PWM_OCTOGGLE_NVIC_Configuration();

  /* GPIO Configuration */
  PWM_OCTOGGLE_GPIO_Configuration();

  /* ---------------------------------------------------------------------------
    TIM3 Configuration: Output Compare Toggle Mode:
    TIM3CLK = SystemCoreClock / 2,
    The objective is to get TIM3 counter clock at 12 MHz:
     - Prescaler = (TIM3CLK / TIM3 counter clock) - 1
    CC1 update rate = TIM3 counter clock / CCR1_Val = 366.2 Hz
    CC2 update rate = TIM3 counter clock / CCR2_Val = 732.4 Hz
    CC3 update rate = TIM3 counter clock / CCR3_Val = 1464.8 Hz
    CC4 update rate = TIM3 counter clock / CCR4_Val = 2929.6 Hz
  ----------------------------------------------------------------------------*/
  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 2000) - 1;

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* Output Compare Toggle Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OC2Init(TIM2, &TIM_OCInitStructure);

  TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);

  /* Output Compare Toggle Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;

  TIM_OC3Init(TIM2, &TIM_OCInitStructure);

  TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Disable);

  /* TIM enable counter */
  TIM_Cmd(TIM2, ENABLE);

  /* TIM IT enable */
  TIM_ITConfig(TIM2,TIM_IT_CC2 | TIM_IT_CC3, ENABLE);

}

_Bool CH2_Flag = 0;
/**
  * @brief  This function handles TIM2 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
	uint16_t capture = 0;
	float CH2_Duty = 0.5;
  /* TIM3_CH2 toggling with frequency = 1464.8 Hz */
  if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
    capture = TIM_GetCapture2(TIM2);
		if(CH2_Flag == 0)
		{
			TIM_SetCompare2(TIM2, capture + (uint16_t)CCR2_Val*CH2_Duty);
			CH2_Flag = 1;
		}
		else
		{
			TIM_SetCompare2(TIM2, capture + (uint16_t)(CCR2_Val* (1- CH2_Duty)));
			CH2_Flag = 0;
		}
  }
  /* TIM3_CH3 toggling with frequency = 1464.8 Hz */
  if (TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
    capture = TIM_GetCapture3(TIM2);
    TIM_SetCompare3(TIM2, capture + CCR3_Val);
  }
}
