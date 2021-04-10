#include "input_capture.h"

uint32_t Capture = 0;

uint32_t IC2ReadValue1 = 0, IC2ReadValue2 = 0;
uint32_t TIM2Freq_IC2 = 0;
uint8_t CaptureNumber_IC2 = 0;

uint32_t IC3ReadValue1 = 0, IC3ReadValue2 = 0;
uint32_t TIM2Freq_IC3 = 0;
uint8_t CaptureNumber_IC3 = 0;

uint16_t  IC3Counter = 0;
uint16_t  IC2Counter = 0;

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void INPUT_CAPTURE_RCC_Configuration(void)
{
  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  /* GPIOA and GPIOA clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
}

/**
  * @brief  Configure the GPIOD Pins.
  * @param  None
  * @retval None
  */
void INPUT_CAPTURE_GPIO_Configuration(void)
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
void INPUT_CAPTURE_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the TIM2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void input_capture_init(void)
{
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	/* System Clocks Configuration */
  INPUT_CAPTURE_RCC_Configuration();

  /* NVIC configuration */
  INPUT_CAPTURE_NVIC_Configuration();

  /* Configure the GPIO ports */
  INPUT_CAPTURE_GPIO_Configuration();

  /* TIM2 configuration: Input Capture mode ---------------------
     The external signal is connected to TIM2 channel 2,3 pin (PA.01,PA.02)  
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
  TIM_ITConfig(TIM2, TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_Update, ENABLE);
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
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		IC2Counter++;
		IC3Counter++;
  }
  if(TIM_GetITStatus(TIM2, TIM_IT_CC2) == SET) 
  {
    /* Clear TIM2 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
    if(CaptureNumber_IC2 == 0)
    {
      /* Get the Input Capture value */
      IC2ReadValue1 = TIM_GetCapture2(TIM2);
      CaptureNumber_IC2 = 1;
			IC2Counter = 0;
    }
    else if(CaptureNumber_IC2 == 1)
    {
      /* Get the Input Capture value */
      IC2ReadValue2 = TIM_GetCapture2(TIM2); 
      
			Capture = (0xFFFF * IC2Counter) + IC2ReadValue2 - IC2ReadValue1;
			
//      /* Capture computation */
//      if (IC2ReadValue2 > IC2ReadValue1)
//      {
//        Capture = (IC2ReadValue2 - IC2ReadValue1); 
//      }
//      else
//      {
//        Capture = ((0xFFFF - IC2ReadValue1) + IC2ReadValue2); 
//      }
      /* Frequency computation */ 
      TIM2Freq_IC2 = (uint32_t) (SystemCoreClock * 1.0 / Capture + 0.5);
      CaptureNumber_IC2 = 0;
			IC2Counter = 0;
    }
  }
	
	if(TIM_GetITStatus(TIM2, TIM_IT_CC3) == SET) 
  {
    /* Clear TIM2 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
    if(CaptureNumber_IC3 == 0)
    {
      /* Get the Input Capture value */
      IC3ReadValue1 = TIM_GetCapture3(TIM2);
      CaptureNumber_IC3 = 1;
			IC3Counter = 0;
    }
    else if(CaptureNumber_IC3 == 1)
    {
      /* Get the Input Capture value */
      IC3ReadValue2 = TIM_GetCapture3(TIM2); 
      
			Capture = (0xFFFF * IC3Counter) + IC3ReadValue2 - IC3ReadValue1;
			
//      /* Capture computation */
//      if (IC3ReadValue2 > IC3ReadValue1)
//      {
//        Capture = (IC3ReadValue2 - IC3ReadValue1); 
//      }
//      else
//      {
//        Capture = ((0xFFFF - IC3ReadValue1) + IC3ReadValue2); 
//      }
      /* Frequency computation */ 
      TIM2Freq_IC3 = (uint32_t) (SystemCoreClock * 1.0 / Capture + 0.5);
      CaptureNumber_IC3 = 0;
    }
  }
}

