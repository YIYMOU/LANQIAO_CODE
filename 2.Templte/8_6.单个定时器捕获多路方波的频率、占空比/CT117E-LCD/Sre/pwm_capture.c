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
	
	
	/* TIM3 channel 2 pin (PA.07) configuration */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* TIM3 configuration: Input Capture mode ---------------------
     The external signal is connected to TIM3 CH2 pin (PA.07)  
     The Rising edge is used as active edge,
     The TIM3 CCR2 is used to compute the frequency value 
	------------------------------------------------------------ */

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;									// 输入通道选择
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;				// 输入捕获触发选择，上升沿触发
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;		// 输入捕获选择
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;							// 输入捕获预分频器
	TIM_ICInitStructure.TIM_ICFilter = 0x0;														// 输入捕获滤波器，一般我们不使用滤波器，即设置为 0。

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
__IO uint32_t TIM3_CH2_Freq = 0;

__IO uint16_t TIM3_CH1_ReadValue1 = 0, TIM3_CH1_ReadValue2 = 0;
__IO uint16_t TIM3_CH1_CaptureNumber = 0;
__IO uint32_t TIM3_CH1_Freq = 0;
u8 TIM3_CH1_Duty, TIM3_CH2_Duty;

u8 capture_flag = 1;
void TIM3_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM3, TIM_IT_CC2) == SET) 
  {
    /* Clear TIM3 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
		if(capture_flag == 2)
		{
			if(TIM3_CH2_CaptureNumber == 0)
			{
				TIM_SetCounter(TIM3, 0);
				TIM_OC2PolarityConfig(TIM3,TIM_ICPolarity_Falling);
				TIM3_CH2_CaptureNumber = 1;
			}
			else if (TIM3_CH2_CaptureNumber == 1)
			{
				/* Get the Input Capture value */
				TIM3_CH2_ReadValue1 = TIM_GetCounter(TIM3);
				TIM_OC2PolarityConfig(TIM3,TIM_ICPolarity_Rising);
				TIM3_CH2_CaptureNumber = 2;
			}
			else if(TIM3_CH2_CaptureNumber == 2)
			{
				/* Get the Input Capture value */
				TIM3_CH2_ReadValue2 = TIM_GetCounter(TIM3); 
				/* Frequency computation */ 
				TIM3_CH2_Freq = (uint32_t) SystemCoreClock / TIM3_CH2_ReadValue2;
				TIM3_CH2_Duty = TIM3_CH2_ReadValue1 * 100 / TIM3_CH2_ReadValue2;
				TIM3_CH2_CaptureNumber = 0;
			}
		} 
  }
  
  if(TIM_GetITStatus(TIM3, TIM_IT_CC1) == SET) 
  {
    /* Clear TIM3 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
		if(capture_flag == 1)
		{
			if(TIM3_CH1_CaptureNumber == 0)
			{
				TIM_SetCounter(TIM3, 0);
				TIM_OC1PolarityConfig(TIM3,TIM_ICPolarity_Falling);	// 将下一次的捕获设置为下降沿触发
				TIM3_CH1_CaptureNumber = 1;
			}
			else if (TIM3_CH1_CaptureNumber == 1)
			{
				 /* Get the Input Capture value */
				TIM3_CH1_ReadValue1 = TIM_GetCounter(TIM3);
				TIM_OC1PolarityConfig(TIM3,TIM_ICPolarity_Rising);
				TIM3_CH1_CaptureNumber = 2;
			}
			else if(TIM3_CH1_CaptureNumber == 2)
			{
				/* Get the Input Capture value */
				TIM3_CH1_ReadValue2 = TIM_GetCounter(TIM3); 
				
				/* Frequency computation */ 
				TIM3_CH1_Freq = (uint32_t) SystemCoreClock / TIM3_CH1_ReadValue2;
				TIM3_CH1_Duty = TIM3_CH1_ReadValue1 * 100 / TIM3_CH1_ReadValue2;
				TIM3_CH1_CaptureNumber = 0;
			}  
		}
  }
}

