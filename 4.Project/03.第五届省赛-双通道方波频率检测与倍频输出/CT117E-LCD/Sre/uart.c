#include "uart.h"

uint8_t RxBuffer[20];
uint8_t RxCnt = 0;
_Bool RxFlag = 0;

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void UART_IDLE_RXNE_RCC_Configuration(void)
{
  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

  /* Enable USART2 Clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void UART_IDLE_RXNE_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure USART2 Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
  
//  /* Configure USART2 Tx as alternate function push-pull */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);  
}
/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void UART_IDLE_RXNE_NVIC_Configuration(void)
{
   NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the USART2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void uart_init(void)
{
	USART_InitTypeDef USART_InitStructure;
	/* System Clocks Configuration */
  UART_IDLE_RXNE_RCC_Configuration();
       
  /* NVIC configuration */
  UART_IDLE_RXNE_NVIC_Configuration();

  /* Configure the GPIO ports */
  UART_IDLE_RXNE_GPIO_Configuration();

/* USART2 configuration -------------------------------------------*/
  /* USART2 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */

  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx;
  
  /* Configure USART2 */
  USART_Init(USART2, &USART_InitStructure);

  /* Enable the USART2 Receive Interrupt */
	//USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  
  /* Enable USART2 */
  USART_Cmd(USART2, ENABLE);
}

/**
  * @brief  This function handles USART2 global interrupt request.
  * @param  None
  * @retval None
  */

