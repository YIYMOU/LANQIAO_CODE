/*
 * 简介：PWM输出相关程序
 * Copyright (c) 2018 电子设计工坊 dianshe.taobao.com
 * All rights reserved
 */
#include "pwm.h"

void TIM2_PWM_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	uint16_t PrescalerValue = 0;
	uint16_t CCR1_Val = 500;
	
	/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	/* GPIOA  clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1|GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Compute the prescaler value */
	PrescalerValue = (uint16_t) (SystemCoreClock / 1000000) - 1;
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 1000 - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;					/* TIM_OCMode_PWM1：在向上计数时，一旦TIMx_CNT<TIMx_CCR1时通道1为有效电平，否则为无效电平；在向下计数时，一旦TIMx_CNT>TIMx_CCR1时通道1为无效电平(OC1REF=0)，否则为有效电平(OC1REF=1)*/
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		/* 输出状态：使能OC输出*/
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;							/* 设置脉冲宽度*/
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;			/* 输出比较极性：高*/
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	
	/* PWM1 Mode configuration: Channel3 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;					/* TIM_OCMode_PWM1：在向上计数时，一旦TIMx_CNT<TIMx_CCR1时通道1为有效电平，否则为无效电平；在向下计数时，一旦TIMx_CNT>TIMx_CCR1时通道1为无效电平(OC1REF=0)，否则为有效电平(OC1REF=1)*/
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		/* 输出状态：使能OC输出*/
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;							/* 设置脉冲宽度*/
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;			/* 输出比较极性：高*/
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM2, ENABLE);

	/* TIM3 enable counter */
	TIM_Cmd(TIM2, ENABLE);
}

void TIM3_PWM_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	uint16_t PrescalerValue = 0;
	uint16_t CCR1_Val = 50;
	
	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	/* GPIOA  clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* Compute the prescaler value */
	PrescalerValue = (uint16_t) (SystemCoreClock / 1000000) - 1;
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 100 - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

	/* PWM1 Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM3, ENABLE);

	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);
}


__IO uint16_t TIM3_CCR1_Val = 32768;
__IO uint16_t TIM3_CCR2_Val = 16384;

void TIM3_PWM_OCToggle(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  uint16_t PrescalerValue = 0;

  /* System Clocks Configuration */
  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  /* GPIOA clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB|
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);

  /* NVIC Configuration */
  /* Enable the TIM3 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* GPIO Configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
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
  PrescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* Output Compare Toggle Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = TIM3_CCR1_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);

  /* Output Compare Toggle Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = TIM3_CCR2_Val;

  TIM_OC2Init(TIM3, &TIM_OCInitStructure);

  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);

  /* TIM enable counter */
  TIM_Cmd(TIM3, ENABLE);
  
  /* TIM IT enable */
  TIM_ITConfig(TIM3, TIM_IT_CC1 | TIM_IT_CC2 , ENABLE);
}

uint16_t TIM3_capture = 0;
void TIM3_IRQHandler(void)
{
  /* TIM3_CH1 toggling with frequency = 183.1 Hz */
  if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)			//计数器当前值与CCR值相等产生中断，并自动翻转IO口 --by cc
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC1 );				//清除中断标志位
    TIM3_capture = TIM_GetCapture1(TIM3);					//获取当前CCR值，第一次进中断必然CCR为当初设置的值
															//第二次进中断为CCR *2，以此类推 -- by cc
    TIM_SetCompare1(TIM3, TIM3_capture + TIM3_CCR1_Val );	//设置比较寄存器CCR的值为：CCR+CCR，从而形成50%占空比，频率根据CCR值确定
  }
  
  /* TIM3_CH2 toggling with frequency = 366.2 Hz */
  if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
    TIM3_capture = TIM_GetCapture2(TIM3);
    TIM_SetCompare2(TIM3, TIM3_capture + TIM3_CCR2_Val);
  }
}

__IO uint16_t TIM2_CCR2_Val = 8192;
__IO uint16_t TIM2_CCR3_Val = 4096;

void TIM2_PWM_OCToggle(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  uint16_t PrescalerValue = 0;

  /* System Clocks Configuration */
  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  /* GPIOA clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB|
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);

  /* NVIC Configuration */
  /* Enable the TIM2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* GPIO Configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
  /* ---------------------------------------------------------------------------
    TIM2 Configuration: Output Compare Toggle Mode:
    TIM2CLK = SystemCoreClock / 2,
    The objective is to get TIM2 counter clock at 12 MHz:
     - Prescaler = (TIM2CLK / TIM2 counter clock) - 1
    CC1 update rate = TIM2 counter clock / CCR1_Val = 366.2 Hz
    CC2 update rate = TIM2 counter clock / CCR2_Val = 732.4 Hz
    CC3 update rate = TIM2 counter clock / CCR3_Val = 1464.8 Hz
    CC4 update rate = TIM2 counter clock / CCR4_Val = 2929.6 Hz
  ----------------------------------------------------------------------------*/
  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* Output Compare Toggle Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = TIM2_CCR2_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OC2Init(TIM2, &TIM_OCInitStructure);

  TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);

  /* Output Compare Toggle Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = TIM2_CCR3_Val;

  TIM_OC3Init(TIM2, &TIM_OCInitStructure);

  TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Disable);

  /* TIM enable counter */
  TIM_Cmd(TIM2, ENABLE);
  
  /* TIM IT enable */
  TIM_ITConfig(TIM2, TIM_IT_CC2 | TIM_IT_CC3 , ENABLE);
}

uint16_t TIM2_capture = 0;
void TIM2_IRQHandler(void)
{
  /* TIM2_CH2 toggling with frequency = 732.4 Hz */
  if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC2 );
    TIM2_capture = TIM_GetCapture2(TIM2);
    TIM_SetCompare2(TIM2, TIM2_capture + TIM2_CCR2_Val );
  }

  /* TIM2_CH3 toggling with frequency = 1464.8 Hz */
  if (TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
    TIM2_capture = TIM_GetCapture3(TIM2);
    TIM_SetCompare3(TIM2, TIM2_capture + TIM2_CCR3_Val);
  }
}

