


void arch_AdcInit(void)
{

	// 使能AIN4/PD3所在端口
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

	// 配置PD3为AD输入
	GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_3);
  
	// 使能ADC时钟 
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	
	//设置ADC采样速率
	SysCtlADCSpeedSet(SYSCTL_ADCSPEED_125KSPS);
  
	// 配置采样序列发生器3为软件触发，且为最高优先级							
	ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);  
															  
	// 配置采样序列发生器3的通道0，转换完成中断使能，
	// 且是最后一个转换通道
	ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH4 | ADC_CTL_IE | ADC_CTL_END);
										
	// 启动AD的采样序列发生器3
	ADCSequenceEnable(ADC0_BASE, 0); 
  
	// 清除中断标志		
	ADCIntClear(ADC0_BASE, 0);
}

float arch_AdcData()
{
	uint nRes; 

	// 触发ADC转换
	ADCProcessorTrigger(ADC0_BASE, 0);

	// 等待ADC转换完毕
	while(!ADCIntStatus(ADC0_BASE, 0, false));

	// 读取ADC的值
	ADCSequenceDataGet(ADC0_BASE, 0, &nRes);

	return (float)nRes;
}

