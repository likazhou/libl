



//Internal Functions



void SystemInit(void)
{

	//初始化系统时钟
	RCC_DeInit();

#if MCU_HSE_ENABLE
	//启用外部高速晶振
	RCC_HSEConfig(RCC_HSE_ON);
	if (RCC_WaitForHSEStartUp() == SUCCESS) {
#endif
		/* HCLK = 72M Max */
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		/* PCLK2 72M Max */
		RCC_PCLK2Config(RCC_HCLK_Div1);
		/* PCLK1 36M Max */
#if MCU_FREQUENCY == MCU_SPEED_HALF
		RCC_PCLK1Config(RCC_HCLK_Div1);
#else
		RCC_PCLK1Config(RCC_HCLK_Div2);
#endif
		/* ADCCLK = PCLK2/6 */
		RCC_ADCCLKConfig(RCC_PCLK2_Div6);
		/* Enable Prefetch Buffer */
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		/* Flash wait state */
#if MCU_FREQUENCY == MCU_SPEED_LOW
		FLASH_SetLatency(FLASH_Latency_0);
#elif MCU_FREQUENCY == MCU_SPEED_HALF
		FLASH_SetLatency(FLASH_Latency_1);
#else
		FLASH_SetLatency(FLASH_Latency_2);
#endif
		/* PLLCLK */
#if MCU_HSE_ENABLE
#if MCU_FREQUENCY == MCU_SPEED_LOW
		RCC_PLLConfig(RCC_PLLSource_HSE_Div2, RCC_PLLMul_2);
#elif MCU_FREQUENCY == MCU_SPEED_HALF
		RCC_PLLConfig(RCC_PLLSource_HSE_Div2, RCC_PLLMul_9);
#else
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
#endif
#else
#if MCU_FREQUENCY == MCU_SPEED_LOW
		RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_2);
#elif MCU_FREQUENCY == MCU_SPEED_HALF
		RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_9);
#else
		RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_16);
#endif
#endif
		/* Enable PLL */ 
		RCC_PLLCmd(ENABLE);
		/* Wait till PLL is ready */
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
		/* Select PLL as system clock source */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		/* Wait till PLL is used as system clock source */
		while (RCC_GetSYSCLKSource() != 0x08);
#if MCU_HSE_ENABLE
	}	
#endif
}


static void stm32_RccInit()
{

	//SystemInit(); //on startup_xxx.s

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

#if OS_TYPE
	//SysTick Initialize
	SysTick_Config(MCU_CLOCK / (1000 / OS_TICK_MS));
	SysTick->CTRL |= SysTick_CLKSource_HCLK;
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
#if (ARCH_TYPE == ARCH_T_STM32F10X_CL) && USB_ENABLE
	/* Disable the Selected IRQ Channels --------------------------------------*/
	SETBIT(NVIC->ICER[OTG_FS_IRQn >> 0x05], OTG_FS_IRQn & 0x1F);
#endif
}

static void stm32_GpioInit()
{
	GPIO_InitTypeDef xGpio;

	//将所有没有使用的IO置为模拟输入
	//可以降低功耗和改善EMC/EMI性能
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
						   	RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
							RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF |
							RCC_APB2Periph_GPIOG, ENABLE);
	xGpio.GPIO_Pin = GPIO_Pin_All;
	xGpio.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &xGpio);
	GPIO_Init(GPIOB, &xGpio);
	GPIO_Init(GPIOC, &xGpio);
	GPIO_Init(GPIOD, &xGpio);
	GPIO_Init(GPIOE, &xGpio);
	GPIO_Init(GPIOF, &xGpio);
	GPIO_Init(GPIOG, &xGpio);
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
						   	RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
							RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF |
							RCC_APB2Periph_GPIOG, DISABLE);
}


static void stm32_DbgInit()
{

#if 0
	DBGMCU_Config(0x00000127, ENABLE);
	*(vu32 *)0xE0000FB0 = 0xC5ACCE55;
	*(vu32 *)0xE0000E80 = 0x00010017;
	*(vu32 *)0xE0000E40 = 0x00000081;
	*(vu32 *)0xE0000E00 = 0x80000001;
	*(vu32 *)0xE00400F0 = 0x00000002;
	*(vu32 *)0xE0040304 = 0x00000102;
#else
	*(vu32 *)0xE0042008 = 0x00001000;
#endif
}

//External Functions
void arch_Init()
{

	//时钟系统初始化
	stm32_RccInit();
//	stm32_DbgInit();

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

