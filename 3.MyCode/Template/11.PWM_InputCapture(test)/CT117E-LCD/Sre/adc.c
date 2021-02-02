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
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);	// ��Ƶ����Ҫȷ�� ADC1 ��ʱ�ӣ�ADCCLK����Ҫ���� 14Mhz�� 
																		// ����������÷�Ƶ����λ 6�� ʱ��Ϊ 72/6=12MHz������������
	/* Enable GPIOs and ADC1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_ADC1, ENABLE);
	
	/* GPIO configuration ------------------------------------------------------*/
  GPIO_Configuration();
	
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	// ADC_Mode������ ADC ��ģʽ����ʹ��һ�� ADC 
																											// ʱ�Ƕ���ģʽ��ʹ������ ADC ʱ��˫ģʽ����
																											// ˫ģʽ�»��кܶ�ϸ��ģʽ��ѡ�� �������� ADC_CR1:DUALMOD λ��
																											// ��������ֻʹ����һ��ADC,��������Ϊ����ģʽ
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;	// ScanConvMode����ѡ����Ϊ ENABLE �� DISABLE�������Ƿ�ʹ��ɨ�衣���
																								// �ǵ�ͨ�� AD ת��ʹ�� DISABLE ������Ƕ�ͨ��ADת��ʹ��ENABLE��������
																								// ��ADC_CR1:SCAN λ��
																								// ���ж��ͨ����Ҫ�ɼ��ź�ʱ�����԰� ADC ����Ϊ��һ����˳�����Ը���ͨ
																								// ������ɨ��ת�����������ɼ���ͨ����ֵ��
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	// ����ת��ģʽ����ģʽ�뵥��ת��ģʽ�෴������ת��ģʽ ADC ֻ��
																											// ��һ�����ݾ�ֹͣת����������ת��ģʽ������һ�� ADC ת����ɺ�
																											// ����������һ��ת����
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	// ADC ��Ҫ�ڽ��յ������źź�ſ�ʼ����ģ��ת����
																																			// ���ⲿ�жϴ�����EXTI �ߣ�����ʱ��������������
																																			// Ϊ�ⲿ�����źţ������ʹ���ⲿ�����źſ���ʹ
																																			// ��������ƴ��� ��
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	// ���ݶ��뷽ʽ��align:����
  ADC_InitStructure.ADC_NbrOfChannel = 1;	// �����Ա������Ҫ���� ADC ����ת����ͨ����������Ϊ1 �� 16 ����������������
																					// �ε�ʵ���У�ֻ�õ�һ��ͨ������������Ϊ1
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channels configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_28Cycles5);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);  

  /* Enable ADC1 reset calibration register */   
  ADC_ResetCalibration(ADC1);	// Calibration:У׼
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));  
	
  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);		/*��ʼADC1����*/
}

uint16_t Get_Adc_Value(void)
{
	return ADC_GetConversionValue(ADC1);
}
