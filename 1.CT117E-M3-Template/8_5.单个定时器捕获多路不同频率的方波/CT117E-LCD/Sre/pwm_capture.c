/*
 * 简介： PWM捕获相关程序
 * Copyright (c) 2018 电子设计工坊 dianshe.taobao.com
 * All rights reserved
 */
#include "pwm_capture.h"

void TIM3_Input_Mode_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;

	
	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* GPIOA and GPIOB clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/* Enable the TIM3 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	/* TIM3 channel 2 pin (PA.07，PA.06) configuration */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* TIM3 configuration: Input Capture mode ---------------------
     The external signal is connected to TIM3 CH2 pin (PA.07)  
     The Rising edge is used as active edge,
     The TIM3 CCR2 is used to compute the frequency value 
	------------------------------------------------------------ */

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x0;

	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInit(TIM3, &TIM_ICInitStructure);

	/* TIM enable counter */
	TIM_Cmd(TIM3, ENABLE);

	/* Enable the CC1 / CC2 Interrupt Request */
	TIM_ITConfig(TIM3,TIM_IT_CC1|TIM_IT_CC2, ENABLE);
}

__IO uint16_t TIM3_CH2_ReadValue1 = 0, TIM3_CH2_ReadValue2 = 0;
__IO uint16_t TIM3_CH2_CaptureNumber = 0;
__IO uint32_t TIM3_CH2_Capture = 0;
__IO uint32_t TIM3_CH2_Freq = 0;

__IO uint16_t TIM3_CH1_ReadValue1 = 0, TIM3_CH1_ReadValue2 = 0;
__IO uint16_t TIM3_CH1_CaptureNumber = 0;
__IO uint32_t TIM3_CH1_Capture = 0;
__IO uint32_t TIM3_CH1_Freq = 0;

void TIM3_IRQHandler(void)
{ 
  if(TIM_GetITStatus(TIM3, TIM_IT_CC2) == SET) 
  {
    /* Clear TIM3 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
    if(TIM3_CH2_CaptureNumber == 0)
    {
      /* Get the Input Capture value */
      TIM3_CH2_ReadValue1 = TIM_GetCapture2(TIM3);
      TIM3_CH2_CaptureNumber = 1;
    }
    else if(TIM3_CH2_CaptureNumber == 1)
    {
      /* Get the Input Capture value */
      TIM3_CH2_ReadValue2 = TIM_GetCapture2(TIM3); 
      
      /* Capture computation */
      if (TIM3_CH2_ReadValue2 > TIM3_CH2_ReadValue1)
      {
       TIM3_CH2_Capture = (TIM3_CH2_ReadValue2 - TIM3_CH2_ReadValue1); 
      }
      else
      {
        TIM3_CH2_Capture = ((0xFFFF - TIM3_CH2_ReadValue1) + TIM3_CH2_ReadValue2); 
      }
      /* Frequency computation */ 
      TIM3_CH2_Freq = (uint32_t) SystemCoreClock / TIM3_CH2_Capture;
      TIM3_CH2_CaptureNumber = 0;
    }
  }
  
  if(TIM_GetITStatus(TIM3, TIM_IT_CC1) == SET) 
  {
    /* Clear TIM3 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
		if(TIM3_CH1_CaptureNumber == 0)
    {
      /* Get the Input Capture value */
      TIM3_CH1_ReadValue1 = TIM_GetCapture1(TIM3);
      TIM3_CH1_CaptureNumber = 1;
    }
    else if(TIM3_CH1_CaptureNumber == 1)
    {
      /* Get the Input Capture value */
      TIM3_CH1_ReadValue2 = TIM_GetCapture1(TIM3); 
      
      /* Capture computation */
      if (TIM3_CH1_ReadValue2 > TIM3_CH1_ReadValue1)
      {
       TIM3_CH1_Capture = (TIM3_CH1_ReadValue2 - TIM3_CH1_ReadValue1); 
      }
      else
      {
        TIM3_CH1_Capture = ((0xFFFF - TIM3_CH1_ReadValue1) + TIM3_CH1_ReadValue2); 
      }
      /* Frequency computation */ 
      TIM3_CH1_Freq = (uint32_t) SystemCoreClock / TIM3_CH1_Capture;
      TIM3_CH1_CaptureNumber = 0;
    }
  }
}

