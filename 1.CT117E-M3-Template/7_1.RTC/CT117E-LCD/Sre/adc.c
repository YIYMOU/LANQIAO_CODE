/*
 * 简介：ADC1 Channel8 采样相关程序
 * Copyright (c) 2018 电子设计工坊 dianshe.taobao.com
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
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;						/*ADC独立模式 */
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;							/*连续模式 */
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;						/*连续转换 */
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;		/*ADC不使用外部触发 */
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;					/*右对齐 */
	ADC_InitStructure.ADC_NbrOfChannel = 1;									/*扫描通道数：1 */
	ADC_Init(ADC1, &ADC_InitStructure);										/*通过结构体初始化ADC1 */

	/* ADC1 regular channel8 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_55Cycles5);	/*配置ADC1通道8、扫描顺序、周期*/

	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);												/*ADC1 DMA使能，其实我们没用DMA，可以注释这行*/

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);													/*ADC1使能*/	

	/* Enable ADC1 reset calibration register */   
	ADC_ResetCalibration(ADC1);												/*复位ADC1校准寄存器*/
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));								/*等待ADC1校准寄存器复位完成*/

	/* Start ADC1 calibration */
	ADC_StartCalibration(ADC1);												/*ADC1校准*/
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));									/*等待ADC1校准完成*/
	 
	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);									/*开始ADC1采样*/
}
