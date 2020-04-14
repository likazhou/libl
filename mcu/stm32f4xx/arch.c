



//Internal Functions





/* ==============	BOARD SPECIFIC CONFIGURATION CODE BEGIN    ============== */

/**
  * @brief	System Clock Configuration
  * 		The system Clock is configured as follow :
  * 		   System Clock source			  = PLL (HSE)
  * 		   SYSCLK(Hz)					  = 168000000
  * 		   HCLK(Hz) 					  = 168000000
  * 		   AHB Prescaler				  = 1
  * 		   APB1 Prescaler				  = 4
  * 		   APB2 Prescaler				  = 2
  * 		   HSE Frequency(Hz)			  = 25000000
  * 		   PLL_M						  = 25
  * 		   PLL_N						  = 336
  * 		   PLL_P						  = 2
  * 		   VDD(V)						  = 3.3
  * 		   Main regulator output voltage  = Scale1 mode
  * 		   Flash Latency(WS)			  = 5
  * @param	None
  * @retval None
  */
static void stm32_RccInit()
{
	/* Enable HSE oscillator */
	LL_RCC_HSE_Enable();
	while(LL_RCC_HSE_IsReady() != 1)
	{
	};

	/* Set FLASH latency */
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);

	/* Main PLL configuration and activation */
#if HSE_VALUE == 8000000
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_8, 336, LL_RCC_PLLP_DIV_2);
#else
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_25, 336, LL_RCC_PLLP_DIV_2);
#endif
	LL_RCC_PLL_Enable();
	while(LL_RCC_PLL_IsReady() != 1)
	{
	};

	/* Sysclk activation on the main PLL */
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
	while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
	{
	};

	/* Set APB1 & APB2 prescaler */
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);

	/* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
	//SystemCoreClockUpdate();	// In system_stm32f4xx.c inited
	
#if OS_TYPE
	//SysTick Initialize
	SysTick_Config(MCU_CLOCK / (1000 / OS_TICK_MS));
#endif
}

static void stm32_IrqInit()
{

#if IRQ_ENABLE
	arch_IrqPri(EXTI0_IRQn, 0);
	arch_IrqPri(EXTI1_IRQn, 0);
	arch_IrqPri(EXTI2_IRQn, 0);
	arch_IrqPri(EXTI3_IRQn, 0);
	arch_IrqPri(EXTI4_IRQn, 0);
	arch_IrqPri(EXTI9_5_IRQn, 0);
	arch_IrqPri(EXTI15_10_IRQn, 0);
#endif
#if ETH_INT_ENABLE
	arch_IrqPri(ETH_IRQn, 0);
#endif
}

static void stm32_GpioInit()
{

	LL_AHB1_GRP1_DisableClock(	LL_AHB1_GRP1_PERIPH_GPIOA | LL_AHB1_GRP1_PERIPH_GPIOB | LL_AHB1_GRP1_PERIPH_GPIOC |
								LL_AHB1_GRP1_PERIPH_GPIOD | LL_AHB1_GRP1_PERIPH_GPIOE | LL_AHB1_GRP1_PERIPH_GPIOF | 
								LL_AHB1_GRP1_PERIPH_GPIOG | LL_AHB1_GRP1_PERIPH_GPIOH | LL_AHB1_GRP1_PERIPH_GPIOI);
}


//External Functions
void arch_Init()
{

	//时钟系统初始化
	stm32_RccInit();

	//中断初始化
	stm32_IrqInit();

	//GPIO初始化
	stm32_GpioInit();
	
#if EPI_ENABLE && EPI_SOFTWARE == 0
	stm32_FsmcInit();
#endif
}


void arch_IrqPri(int chl, int pri)
{

	NVIC->IP[chl] = pri << 4;

	/* Enable the Selected IRQ Channels --------------------------------------*/
	SETBIT(NVIC->ISER[chl >> 0x05], chl & 0x1F);
}


void arch_IdleEntry()
{

	__WFI();
}


void arch_Reset()
{

	__set_FAULTMASK(1);
	NVIC_SystemReset();
}

