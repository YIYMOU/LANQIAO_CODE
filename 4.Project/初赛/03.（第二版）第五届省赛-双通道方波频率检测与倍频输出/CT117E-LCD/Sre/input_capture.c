#include "input_capture.h"

__IO uint16_t IC2ReadValue1 = 0, IC2ReadValue2 = 0;
__IO uint16_t IC2_CaptureNumber = 0;
__IO uint32_t IC2_Capture = 0;
__IO uint32_t IC2_TIM2Freq = 0;

__IO uint16_t IC3ReadValue1 = 0, IC3ReadValue2 = 0;
__IO uint16_t IC3_CaptureNumber = 0;
__IO uint32_t IC3_Capture = 0;
__IO uint32_t IC3_TIM2Freq = 0;

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void INPUT_Capture_RCC_Configuration(void)
{
  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  /* GPIOA and GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
}

/**
  * @brief  Configure the GPIOD Pins.
  * @param  None
  * @retval None
  */
void INPUT_Capture_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* TIM2 channel 2,3 pin (PA.01,PA.02) configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1 | GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void INPUT_Capture_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the TIM2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void input_capture_init(void)
{
	TIM_ICInitTypeDef  TIM_ICInitStructure;
  /* System Clocks Configuration */
  INPUT_Capture_RCC_Configuration();

  /* NVIC configuration */
  INPUT_Capture_NVIC_Configuration();

  /* Configure the GPIO ports */
  INPUT_Capture_GPIO_Configuration();

	TIM_PrescalerConfig(TIM2,72 - 1,TIM_PSCReloadMode_Immediate);
  /* TIM2 configuration: Input IC2_Capture mode ---------------------
     The external signal is connected to TIM2 CH2 pin (PA.07)  
     The Rising edge is used as active edge,
     The TIM2 CCR2 is used to compute the frequency value 
  ------------------------------------------------------------ */

  TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;

  TIM_ICInit(TIM2, &TIM_ICInitStructure);
	
	
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
  TIM_ICInit(TIM2, &TIM_ICInitStructure);
  
  /* TIM enable counter */
  TIM_Cmd(TIM2, ENABLE);

  /* Enable the CC2 Interrupt Request */
  TIM_ITConfig(TIM2, TIM_IT_CC2 | TIM_IT_CC3, ENABLE);
}


/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/
/**
  * @brief  This function handles TIM2 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{ 
  if(TIM_GetITStatus(TIM2, TIM_IT_CC2) == SET) 
  {
    /* Clear TIM3 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
    if(IC2_CaptureNumber == 0)
    {
      /* Get the Input Capture value */
      IC2ReadValue1 = TIM_GetCapture2(TIM2);
      IC2_CaptureNumber = 1;
    }
    else if(IC2_CaptureNumber == 1)
    {
      /* Get the Input Capture value */
      IC2ReadValue2 = TIM_GetCapture2(TIM2); 
      
      /* Capture computation */
      if (IC2ReadValue2 > IC2ReadValue1)
      {
        IC2_Capture = (IC2ReadValue2 - IC2ReadValue1); 
      }
      else
      {
        IC2_Capture = ((0xFFFF - IC2ReadValue1) + IC2ReadValue2); 
      }
      /* Frequency computation */ 
      IC2_TIM2Freq = (uint32_t) SystemCoreClock / 72 / IC2_Capture;
      IC2_CaptureNumber = 0;
    }
  }
	
	if(TIM_GetITStatus(TIM2, TIM_IT_CC3) == SET) 
  {
    /* Clear TIM3 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
    if(IC3_CaptureNumber == 0)
    {
      /* Get the Input Capture value */
      IC3ReadValue1 = TIM_GetCapture3(TIM2);
      IC3_CaptureNumber = 1;
    }
    else if(IC3_CaptureNumber == 1)
    {
      /* Get the Input Capture value */
      IC3ReadValue2 = TIM_GetCapture3(TIM2); 
      
      /* Capture computation */
      if (IC3ReadValue2 > IC3ReadValue1)
      {
        IC3_Capture = (IC3ReadValue2 - IC3ReadValue1); 
      }
      else
      {
        IC3_Capture = ((0xFFFF - IC3ReadValue1) + IC3ReadValue2); 
      }
      /* Frequency computation */ 
      IC3_TIM2Freq = (uint32_t) SystemCoreClock / 72 / IC3_Capture;
      IC3_CaptureNumber = 0;
    }
  }
}
