#include "InputCapture.h"

uint32_t IC3ReadValue1 = 0, IC3ReadValue2 = 0;
uint32_t CaptureNumber = 0;
uint32_t Capture = 0;
uint32_t TIM3Freq = 0;
uint32_t TIM3Duty = 0;
uint32_t TIM3_Update_Cnt = 0;
/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void InputCapture_RCC_Configuration(void)
{
  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  /* GPIOA clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
}
/**
  * @brief  Configure the GPIOA Pins.
  * @param  None
  * @retval None
  */
void InputCapture_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* TIM3 channel 2 pin (PA.07) configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void InputCapture_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the TIM3 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void InputCapture_Init(void)
{
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	
	/* System Clocks Configuration */
  InputCapture_RCC_Configuration();

  /* NVIC configuration */
  InputCapture_NVIC_Configuration();

  /* Configure the GPIO ports */
  InputCapture_GPIO_Configuration();

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
  
  /* TIM enable counter */
  TIM_Cmd(TIM3, ENABLE);

  /* Enable the CC2 Interrupt Request */
  TIM_ITConfig(TIM3, TIM_IT_CC2 | TIM_IT_Update, ENABLE);
}
/**
  * @brief  This function handles TIM3 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{ 
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) 
  {
		/* Clear TIM3 Update compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		TIM3_Update_Cnt++;
	}
	
  if(TIM_GetITStatus(TIM3, TIM_IT_CC2) == SET) 
  {
    /* Clear TIM3 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
    if(CaptureNumber == 0)
    {
      /* Get the Input Capture value */
			TIM_SetCounter(TIM3, 0);
			TIM3_Update_Cnt = 0;
      CaptureNumber = 1;
			TIM_OC2PolarityConfig(TIM3,TIM_ICPolarity_Falling);	// ≈‰÷√Œ™œ¬Ωµ—ÿ
    }
    else if(CaptureNumber == 1)
    {
      /* Get the Input Capture value */
      IC3ReadValue1 = TIM_GetCounter(TIM3) + 65536*TIM3_Update_Cnt;
			CaptureNumber = 2;
			TIM_OC2PolarityConfig(TIM3,TIM_ICPolarity_Rising);	// ≈‰÷√Œ™…œΩµ—ÿ
    }
		else if(CaptureNumber == 2)
		{
			/* Get the Input Capture value */
      IC3ReadValue2 = TIM_GetCounter(TIM3) + 65536*TIM3_Update_Cnt; 
      
			/* Frequency computation */
      TIM3Freq = (uint32_t) (SystemCoreClock * 1.0 / IC3ReadValue2 + 0.5);
			TIM3Duty = (uint32_t) (IC3ReadValue1 * 100.0 / IC3ReadValue2 + 0.5);
      CaptureNumber = 0;
		}
  }
}
