#if OS_TYPE

void arch_AdcInit()
{

 	/* Enable ADC3 clock                                                        */
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
}


u32 arch_AdcData(int nPort, int nPin)
{
	u32 nVol;
	ADC_InitTypeDef ADC_InitStructure;

	ADC_StructInit(&ADC_InitStructure);

	ADC_Init(ADC3, &ADC_InitStructure);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_4, 1, ADC_SampleTime_239Cycles5);

	/* Enable ADC1 ------------------------------------------------------*/
	ADC_Cmd(ADC3, ENABLE);
	/* Enable ADC1 reset calibration register */
	ADC_ResetCalibration(ADC3);
	/* Check the end of ADC1 reset calibration register */
	while (ADC_GetResetCalibrationStatus(ADC3));
	/* Start ADC1 calibration */
	ADC_StartCalibration(ADC3);
	/* Check the end of ADC1 calibration */
	while (ADC_GetCalibrationStatus(ADC3));
	
	nVol = ADC3->DR;
	
 	/* Start ADC1 Software Conversion */ 
 	ADC_SoftwareStartConvCmd(ADC3, ENABLE);
	
	return nVol;
}

#endif

