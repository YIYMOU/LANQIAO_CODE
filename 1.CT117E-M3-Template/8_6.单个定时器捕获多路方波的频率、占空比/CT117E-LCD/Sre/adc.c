/*
 * ��飺ADC1 Channel8 ������س���
 * Copyright (c) 2018 ������ƹ��� dianshe.taobao.com
 * All rights reserved
 */
#include "adc.h"

void ADC_Channel8_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1|RCC_APB2Periph_GPIOB,ENABLE);

	/* Configure PB.0 (ADC Channel8) as analog input -------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;						/*ADC����ģʽ */
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;							/*����ģʽ */
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;						/*����ת�� */
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;		/*ADC��ʹ���ⲿ���� */
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;					/*�Ҷ��� */
	ADC_InitStructure.ADC_NbrOfChannel = 1;									/*ɨ��ͨ������1 */
	ADC_Init(ADC1, &ADC_InitStructure);										/*ͨ���ṹ���ʼ��ADC1 */

	/* ADC1 regular channel8 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_55Cycles5);	/*����ADC1ͨ��8��ɨ��˳������*/

	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);												/*ADC1 DMAʹ�ܣ���ʵ����û��DMA������ע������*/

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);													/*ADC1ʹ��*/	

	/* Enable ADC1 reset calibration register */   
	ADC_ResetCalibration(ADC1);												/*��λADC1У׼�Ĵ���*/
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));								/*�ȴ�ADC1У׼�Ĵ�����λ���*/

	/* Start ADC1 calibration */
	ADC_StartCalibration(ADC1);												/*ADC1У׼*/
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));									/*�ȴ�ADC1У׼���*/
	 
	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);									/*��ʼADC1����*/
}
