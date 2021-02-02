#include "uart.h"
#include <stdio.h>
#include <string.h>

uint8_t RxBuffer[20];
uint8_t RxCounter = 0x00; 
_Bool Rx_flag = 0;
uint16_t uart_cnt = 0;

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void UART_RCC_Configuration(void)
{   
  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);  
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void UART_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure USART2 Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
  
  /* Configure USART2 Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void UART_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Enable the USARTy Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void UART_INIT(void)
{
	USART_InitTypeDef USART_InitStructure;
	/* System Clocks Configuration */
  UART_RCC_Configuration();
       
  /* NVIC configuration */
  UART_NVIC_Configuration();

  /* Configure the GPIO ports */
  UART_GPIO_Configuration();

	/* USART2 configuration ------------------------------------------------------*/
  /* USARTy and USARTz configured as follow:
        - BaudRate = 9600 baud  
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
	/* nRTS：请求以发送(Request To Send)， n 表示低电平有效。如果使能 RTS 流控制，当
		 USART 接收器准备好接收新数据时就会将 nRTS 变成低电平；当接收寄存器已满时，
		 nRTS 将被设置为高电平。该引脚只适用于硬件流控制。
	   nCTS：清除以发送(Clear To Send)， n 表示低电平有效。如果使能 CTS 流控制，发送
		 器在发送下一帧数据之前会检测 nCTS 引脚，如果为低电平，表示可以发送数据，如果为
		 高电平则在发送完当前数据帧之后停止发送。该引脚只适用于硬件流控制。*/
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* Configure USART2 */
  USART_Init(USART2, &USART_InitStructure);
  
  /* Enable USART2 Receive and Transmit interrupts */
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  // USART_ITConfig(USART2, USART_IT_TXE, ENABLE);

  /* Enable the USART2 */
  USART_Cmd(USART2, ENABLE);
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART2, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
  {}

  return ch;
}
void USART2_IRQHandler(void)
{
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
		Rx_flag = 1;
		uart_cnt = 0;
    /* Read one byte from the receive data register */
    RxBuffer[RxCounter++] = USART_ReceiveData(USART2);
		if(RxCounter > 20)
		{
			RxCounter = 0;
			memset(RxBuffer,0,sizeof(RxBuffer));
		}
  }
}
