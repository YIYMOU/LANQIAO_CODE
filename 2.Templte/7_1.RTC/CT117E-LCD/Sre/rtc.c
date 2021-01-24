/*
 * 简介：RTC实时时钟相关程序
 * Copyright (c) 2018 电子设计工坊 dianshe.taobao.com
 * All rights reserved
 */
#include "rtc.h"

__IO uint32_t TimeDisplay = 0;

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Configures the RTC.
  * @param  None
  * @retval None
  */
void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks 使能 PWR 和 Backup 时钟 */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain 允许访问 Backup 区域. */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain 复位 Backup 区域 */
  BKP_DeInit();

  /* Enable the LSI OSC 使能 LSI */
  RCC_LSICmd(ENABLE);
  /* Wait till LSI is ready 等待 LSI 准备好 */
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {}
  /* Select the RTC Clock Source 选择 LSI 作为 RTC 时钟源 */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

  /* Enable RTC Clock 使能 RTC 时钟 */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization 等待时钟同步，在对 RTC 寄存器的任何操作前，必须调用本函数
   * 等待 RTC 寄存器 同步
	 * 因为 RTC 时钟是低速的，内环时钟是高速的，所以要同步
	 */
		RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished 确保上一次 RTC 的操作完成 */
  RTC_WaitForLastTask();

  /* Enable the RTC Second 使能 RTC 秒中断 */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished 确保上一次 RTC 的操作完成 */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec 设置 RTC 分频配置*/
	/* 设置 RTC 分频: 使 RTC 周期为 1s ,LSI 约为 40KHz */
	/* RTC period = RTCCLK/RTC_PR = (40 KHz)/(40000-1+1) = 1HZ */
  RTC_SetPrescaler(40000 - 1);

  /* Wait until last write operation on RTC registers has finished 确保上一次 RTC 的操作完成 */
  RTC_WaitForLastTask();

  /* To output second signal on Tamper pin, the tamper functionality
       must be disabled (by default this functionality is disabled) */
	/* 禁止 Tamper 引脚 tamper pin(PC.13)*/
	/* 要输出 RTCCLK/64 到 Tamper 引脚, tamper 功能必须禁止 */	 
	// 当TAMPER引脚上的信号从0变成1或者从1变成0(取决于备份控制寄存器BKP_CR的TPAL位)，
	// 会产生一个侵入检测事件。侵入检测事件将所有数据备份寄存器内容清除。
  BKP_TamperPinCmd(DISABLE);

  /* Enable the RTC Second Output on Tamper Pin */
	// BKP_RTCOutputConfig(BKP_RTCOutputSource_Second);
  
  NVIC_Configuration();
}

/**
  * @brief  Adjusts time.时间调节
  * @param  None
  * @retval None
  */
void Time_Adjust(u8 Tmp_HH,u8 Tmp_MM,u8 Tmp_SS)
{
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  /* Change the current time 设置 RTC 计数器的值*/
  RTC_SetCounter(Tmp_HH*3600 + Tmp_MM*60 + Tmp_SS);
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}

/**
  * @brief  Displays the current time.
  * @param  TimeVar: RTC counter value.
  * @retval None
  */

uint32_t THH = 0, TMM = 0, TSS = 0;
void Time_Display(uint32_t TimeVar)
{
	
  /* Reset RTC Counter when Time is 23:59:59 */
  if (RTC_GetCounter() == 0x0001517F)	// 获取 RTC 计数器的值 .一天等于3600*24秒，3600*24-1转化为十六进制0x0001517F
  {
     RTC_SetCounter(0x0);
     /* Wait until last write operation on RTC registers has finished */
     RTC_WaitForLastTask();
  }
  
  /* Compute  hours */
  THH = TimeVar / 3600;
  /* Compute minutes */
  TMM = (TimeVar % 3600) / 60;
  /* Compute seconds */
  TSS = (TimeVar % 3600) % 60;

  //printf("Time: %0.2d:%0.2d:%0.2d\r", THH, TMM, TSS);
}

/**
  * @brief  This function handles RTC global interrupt request.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
  if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
  {
    /* Clear the RTC Second interrupt */
    RTC_ClearITPendingBit(RTC_IT_SEC);

    /* Enable time update */
    TimeDisplay = 1;

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();  
  }
}
