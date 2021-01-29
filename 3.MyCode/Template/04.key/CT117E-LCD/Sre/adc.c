#include "adc.h"

void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure PB.00(ADC Channel8)
   as analog input -----------------------------------------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void ADC_Channel18_Init(void)
{
	ADC_InitTypeDef   ADC_InitStructure;
	
	/* ADCCLK = PCLK2/4 */
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);	// 分频因子要确保 ADC1 的时钟（ADCCLK）不要超过 14Mhz。 
																		// 这个我们设置分频因子位 6， 时钟为 72/6=12MHz，否则会有误差
	/* Enable GPIOs and ADC1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_ADC1, ENABLE);
	
	/* GPIO configuration ------------------------------------------------------*/
  GPIO_Configuration();
	
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	// ADC_Mode：配置 ADC 的模式，当使用一个 ADC 
																											// 时是独立模式，使用两个 ADC 时是双模式，在
																											// 双模式下还有很多细分模式可选， 具体配置 ADC_CR1:DUALMOD 位。
																											// 由于这里只使用了一个ADC,所以配置为独立模式
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;	// ScanConvMode：可选参数为 ENABLE 和 DISABLE，配置是否使用扫描。如果
																								// 是单通道 AD 转换使用 DISABLE ，如果是多通道AD转换使用ENABLE，具体配
																								// 置ADC_CR1:SCAN 位。
																								// 当有多个通道需要采集信号时，可以把 ADC 配置为按一定的顺序来对各个通
																								// 道进行扫描转换，即轮流采集各通道的值。
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	// 连续转换模式，此模式与单次转换模式相反，单次转换模式 ADC 只采
																											// 集一次数据就停止转换。而连续转换模式则在上一次 ADC 转换完成后，
																											// 立即开启下一次转换。
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	// ADC 需要在接收到触发信号后才开始进行模数转换，
																																			// 如外部中断触发（EXTI 线）、定时器触发，这两个
																																			// 为外部触发信号，如果不使用外部触发信号可以使
																																			// 用软件控制触发 。
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	// 数据对齐方式。align:对齐
  ADC_InitStructure.ADC_NbrOfChannel = 1;	// 这个成员保存了要进行 ADC 数据转换的通道数，可以为1 ～ 16 个。由于我们在这
																					// 次的实验中，只用到一个通道，所以配置为1
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channels configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_28Cycles5);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);  

  /* Enable ADC1 reset calibration register */   
  ADC_ResetCalibration(ADC1);	// Calibration:校准
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));  
	
  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);		/*开始ADC1采样*/
}

uint16_t Get_Adc_Value(void)
{
	return ADC_GetConversionValue(ADC1);
}
