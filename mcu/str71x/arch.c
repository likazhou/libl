#if ARCH_TYPE == ARCH_T_STR71X

static void str7_RccInit()
{
	//系统基本初始化
	RCCU_MCLKConfig(RCCU_DEFAULT);							//MCLK = RCLK
	RCCU_FCLKConfig(RCCU_RCLK_2);							//FCLK = RCLK/2
	RCCU_PCLKConfig(RCCU_RCLK_4);							//PCLK = RCLK/4
	RCCU_PLL1Config(RCCU_PLL1_Mul_12, RCCU_Div_2);			//48MHz PLL @ 16MHz XTAL
	
	while (RCCU_FlagStatus(RCCU_PLL1_LOCK) == RESET);		//Wait for PLL to Lock
	RCCU_RCLKSourceConfig(RCCU_PLL1_Output);				//Select PLL for RCLK	
	
}

static void str7_IrqInit()
{

	//设置外部中断
	EIC_IRQConfig(ENABLE);
	XTI_Init();
	XTI_ModeConfig(XTI_Interrupt, ENABLE );
	EIC->SIR[XTI_IRQChannel] = ((int)XTI_IRQHandler << 16) | 8;
	EIC_IRQChannelConfig(XTI_IRQChannel, ENABLE);
	
}


void arch_Init()
{

	//时钟系统初始化
	str7_RccInit();
	//中断初始化
	str7_IrqInit();
	//GPIO初始化
	str7_GpioIdleInit();
#if EPI_ENABLE
	str7_EmiInit();
#endif
}





void arch_IdleEntry()
{

	RCCU->SMR &= ~1;
}

void arch_Reset()
{

	
}


#endif

