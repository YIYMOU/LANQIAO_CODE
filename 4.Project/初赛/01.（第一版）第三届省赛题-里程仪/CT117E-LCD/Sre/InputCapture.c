#include "InputCapture.h"

uint16_t Time2_IC2ReadValue1 = 0;
uint16_t Time2_IC2ReadValue2 = 0;
uint8_t Time2_CaptureNumber = 0;
uint32_t Time2_Capture = 0;
uint32_t TIM2Freq = 0;
uint16_t Tim2_Update_Cnt = 0;
uint32_t total_meter_now = 0;

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void Time2_InputCapture_RCC_Configuration(void)
{
  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  /* GPIOA and GPIOA clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
}

/**
  * @brief  Configure the GPIOA Pins.
  * @param  None
  * @retval None
  */
void Time2_InputCapture_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* TIM3 channel 2 pin (PA.01) configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void Time2_InputCapture_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the TIM2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void Time2_InputCapture_Init(void)
{
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	/* System Clocks Configuration */
  Time2_InputCapture_RCC_Configuration();

  /* NVIC configuration */
  Time2_InputCapture_NVIC_Configuration();

  /* Configure the GPIO ports */
  Time2_InputCapture_GPIO_Configuration();

  /* TIM2 configuration: Input Capture mode ---------------------
     The external signal is connected to TIM2 CH2 pin (PA.01)  
     The Rising edge is used as active edge,
     The TIM2 CCR2 is used to compute the frequency value 
  ------------------------------------------------------------ */

  TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;

  TIM_ICInit(TIM2, &TIM_ICInitStructure);
  
  /* TIM enable counter */
  TIM_Cmd(TIM2, ENABLE);

  /* Enable the CC2 Interrupt Request */
  TIM_ITConfig(TIM2, TIM_IT_CC2 | TIM_IT_Update, ENABLE);
}

/**
  * @brief  This function handles TIM2 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{ 
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) 
  {
    /* Clear TIM2 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    Tim2_Update_Cnt++;
  }
	
  if(TIM_GetITStatus(TIM2, TIM_IT_CC2) == SET) 
  {
		total_meter_now++;
    /* Clear TIM2 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
    if(Time2_CaptureNumber == 0)
    {
      /* Get the Input Capture value */
			Tim2_Update_Cnt = 0;
      Time2_IC2ReadValue1 = TIM_GetCapture2(TIM2);
      Time2_CaptureNumber = 1;
    }
    else if(Time2_CaptureNumber == 1)
    {
      /* Get the Input Capture value */
      Time2_IC2ReadValue2 = TIM_GetCapture2(TIM2); 
      
      Time2_Capture = ( Tim2_Update_Cnt * (0xFFFF) + Time2_IC2ReadValue2 - Time2_IC2ReadValue1);
      
      /* Frequency computation */ 
      TIM2Freq = (uint32_t) ((SystemCoreClock * 1.0 / Time2_Capture) + 0.5);
      Time2_CaptureNumber = 0;
    }
  }
}
