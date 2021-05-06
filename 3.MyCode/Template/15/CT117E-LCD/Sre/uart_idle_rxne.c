#include "uart_idle_rxne.h"
#include "stdio.h"
#include "string.h"

uint8_t RxBuffer[20];
_Bool uart2_idle_flag = 0;
uint8_t RxCnt = 0;
#define countof(a)   (sizeof(a) / sizeof(*(a)))
#define RxBufferSize   (countof(RxBuffer) - 1)

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

void UART_IDLE_RXNE_INIT(void)
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

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  /* Configure USART2 */
  USART_Init(USART2, &USART_InitStructure);

  /* Enable the USART2 Receive Interrupt */
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  
  /* Enable USART2 */
  USART_Cmd(USART2, ENABLE);
}
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
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

/**
  * @brief  This function handles USART2 global interrupt request.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		RxBuffer[RxCnt++] = USART_ReceiveData(USART2);
//		USART_SendData(USART2, (uint8_t) RxBuffer[RxCnt - 1]);
//		/* Loop until the end of transmission */
//		while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
//		{}
	}
  else if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
  {
    USART_ReceiveData(USART2);// 读取数据注意：这句必须要，否则不能够清除中断标志位。
		//***********帧数据处理函数************//
		printf ("Thelenght:%d\r\n",RxCnt);
		printf ("The data:%s\r\n",RxBuffer);
		printf ("Over! \r\n");
		memset(RxBuffer,0,sizeof(RxBuffer));
		RxCnt = 0;
		USART_ClearITPendingBit(USART2,USART_IT_IDLE);         	//清除中断标志
  }
}
