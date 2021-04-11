#include "rtc.h"

uint32_t THH = 0,TMM = 0,TSS = 0;

void rtc_init(void)
{
	/* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  BKP_DeInit();

  /* Enable the LSI OSC */
  RCC_LSICmd(ENABLE);
  /* Wait till LSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {}
  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Second */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec */
  RTC_SetPrescaler(40000);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* To output second signal on Tamper pin, the tamper functionality
       must be disabled (by default this functionality is disabled) */
  BKP_TamperPinCmd(DISABLE);

  /* Enable the RTC Second Output on Tamper Pin */
  // BKP_RTCOutputConfig(BKP_RTCOutputSource_Second);
}


/**
  * @brief  Adjusts time.
  * @param  None
  * @retval None
  */
void Time_Adjust(uint32_t Tmp_HH,uint32_t Tmp_MM,uint32_t Tmp_SS)
{
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  /* Change the current time */
  RTC_SetCounter((Tmp_HH*3600 + Tmp_MM*60 + Tmp_SS));
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}

/**
  * @brief  Displays the current time.
  * @param  TimeVar: RTC counter value.
  * @retval None
  */
void Time_Display(void)
{
  uint32_t TimeVar = RTC_GetCounter();
  
  /* Reset RTC Counter when Time is 23:59:59 */
  if (TimeVar == 0x00015180)
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
}
