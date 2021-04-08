#include "tim.h"

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void TIM_RCC_Configuration(void)
{

  /* TIM4 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

  /* GPIOC clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
}

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void TIM_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the TIM4 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
}


void tim_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	uint16_t PrescalerValue = 0;
	/* System Clocks Configuration */
  TIM_RCC_Configuration();

  /* NVIC Configuration */
  TIM_NVIC_Configuration();

  /* ---------------------------------------------------------------
    TIM4 Configuration: Output Compare Timing Mode:
    TIM4 counter clock at 1 MHz
  --------------------------------------------------------------- */

  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 1000000) - 1;

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 1000 - 1;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM4, PrescalerValue, TIM_PSCReloadMode_Immediate);

  /* TIM IT enable */
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

  /* TIM4 enable counter */
  TIM_Cmd(TIM4, ENABLE);
}
