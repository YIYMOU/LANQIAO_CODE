/*
 * ��飺RTCʵʱʱ����س���
 * Copyright (c) 2018 ������ƹ��� dianshe.taobao.com
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
  /* Enable PWR and BKP clocks ʹ�� PWR �� Backup ʱ�� */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain ������� Backup ����. */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain ��λ Backup ���� */
  BKP_DeInit();

  /* Enable the LSI OSC ʹ�� LSI */
  RCC_LSICmd(ENABLE);
  /* Wait till LSI is ready �ȴ� LSI ׼���� */
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {}
  /* Select the RTC Clock Source ѡ�� LSI ��Ϊ RTC ʱ��Դ */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

  /* Enable RTC Clock ʹ�� RTC ʱ�� */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization �ȴ�ʱ��ͬ�����ڶ� RTC �Ĵ������κβ���ǰ��������ñ�����
   * �ȴ� RTC �Ĵ��� ͬ��
	 * ��Ϊ RTC ʱ���ǵ��ٵģ��ڻ�ʱ���Ǹ��ٵģ�����Ҫͬ��
	 */
		RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished ȷ����һ�� RTC �Ĳ������ */
  RTC_WaitForLastTask();

  /* Enable the RTC Second ʹ�� RTC ���ж� */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished ȷ����һ�� RTC �Ĳ������ */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec ���� RTC ��Ƶ����*/
	/* ���� RTC ��Ƶ: ʹ RTC ����Ϊ 1s ,LSI ԼΪ 40KHz */
	/* RTC period = RTCCLK/RTC_PR = (40 KHz)/(40000-1+1) = 1HZ */
  RTC_SetPrescaler(40000 - 1);

  /* Wait until last write operation on RTC registers has finished ȷ����һ�� RTC �Ĳ������ */
  RTC_WaitForLastTask();

  /* To output second signal on Tamper pin, the tamper functionality
       must be disabled (by default this functionality is disabled) */
	/* ��ֹ Tamper ���� tamper pin(PC.13)*/
	/* Ҫ��� RTCCLK/64 �� Tamper ����, tamper ���ܱ����ֹ */	 
	// ��TAMPER�����ϵ��źŴ�0���1���ߴ�1���0(ȡ���ڱ��ݿ��ƼĴ���BKP_CR��TPALλ)��
	// �����һ���������¼����������¼����������ݱ��ݼĴ������������
  BKP_TamperPinCmd(DISABLE);

  /* Enable the RTC Second Output on Tamper Pin */
	// BKP_RTCOutputConfig(BKP_RTCOutputSource_Second);
  
  NVIC_Configuration();
}

/**
  * @brief  Adjusts time.ʱ�����
  * @param  None
  * @retval None
  */
void Time_Adjust(u8 Tmp_HH,u8 Tmp_MM,u8 Tmp_SS)
{
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  /* Change the current time ���� RTC ��������ֵ*/
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
  if (RTC_GetCounter() == 0x0001517F)	// ��ȡ RTC ��������ֵ .һ�����3600*24�룬3600*24-1ת��Ϊʮ������0x0001517F
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
