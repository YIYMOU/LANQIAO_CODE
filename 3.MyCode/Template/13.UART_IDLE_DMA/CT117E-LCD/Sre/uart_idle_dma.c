#include "uart_idle_dma.h"
#include "stdio.h"
#include "string.h"

uint8_t RxBuffer[20];

#define countof(a)   (sizeof(a) / sizeof(*(a)))
#define RxBufferSize   (countof(RxBuffer) - 1)

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void UART_IDLE_DMA_RCC_Configuration(void)
{    
  /* DMA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

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
void UART_IDLE_DMA_GPIO_Configuration(void)
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
void UART_IDLE_DMA_NVIC_Configuration(void)
{
   NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the USARTz Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Configures the DMA.
  * @param  None
  * @retval None
  */
void UART_IDLE_DMA_DMA_Configuration(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  /* USART2_Rx_DMA_Channel (triggered by USART2 Rx event) Config */
  DMA_DeInit(DMA1_Channel6);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RxBuffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = RxBufferSize;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel6, &DMA_InitStructure);
}

void UART_IDLE_DMA_INIT(void)
{
	USART_InitTypeDef USART_InitStructure;
	/* System Clocks Configuration */
  UART_IDLE_DMA_RCC_Configuration();
       
  /* NVIC configuration */
  UART_IDLE_DMA_NVIC_Configuration();

  /* Configure the GPIO ports */
  UART_IDLE_DMA_GPIO_Configuration();

  /* Configure the DMA */
  UART_IDLE_DMA_DMA_Configuration();

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

  /* Enable USART2 DMA TX request */
  USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);

  /* Enable the USART2 Receive Interrupt */
  USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
  
  /* Enable USART2 */
  USART_Cmd(USART2, ENABLE);

  /* Enable USART2 DMA RX Channel */
  DMA_Cmd(DMA1_Channel6, ENABLE);
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
	uint8_t Usart2_Rec_Cnt = 0;
  if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
  {
    USART_ReceiveData(USART2);//读取数据注意：这句必须要，否则不能够清除中断标志位。
		Usart2_Rec_Cnt =RxBufferSize-DMA_GetCurrDataCounter(DMA1_Channel6); //算出接本帧数据长度
		//***********帧数据处理函数************//
		printf ("Thelenght:%d\r\n",Usart2_Rec_Cnt);
		printf ("The data:%s\r\n",RxBuffer);
		printf ("Over! \r\n");
		memset(RxBuffer,0,sizeof(RxBuffer));
		//*************************************//
		USART_ClearITPendingBit(USART2,USART_IT_IDLE);         	//清除中断标志
		DMA_Cmd(DMA1_Channel6, DISABLE );  											//关闭USART2 RX DMA1所指示的通道    
    DMA_SetCurrDataCounter(DMA1_Channel6,RxBufferSize);			//DMA通道的DMA缓存的大小
    DMA_Cmd(DMA1_Channel6, ENABLE);  												//打开USART2 RX DMA1所指示的通道 
  }
}
