






//Internal Functions
#if OS_TYPE

static void stm32_RccInit()
{

#if MCU_HSE_ENABLE == 0
	//启用内部高速时钟
	RCC_HSICmd(ENABLE);

	/* Flash wait state */
#if MCU_FREQUENCY == MCU_SPEED_LOW
	FLASH_SetLatency(FLASH_Latency_0);
#elif MCU_FREQUENCY == MCU_SPEED_HALF
	FLASH_SetLatency(FLASH_Latency_1);
#else
	FLASH_SetLatency(FLASH_Latency_2);
#endif
	/* PLLCLK */
#if MCU_FREQUENCY == MCU_SPEED_LOW
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_2);
#elif MCU_FREQUENCY == MCU_SPEED_HALF
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_9);
#else
//	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_16);
#endif
	/* Enable PLL */ 
	RCC_PLLCmd(ENABLE);
	/* Wait till PLL is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
	/* Select PLL as system clock source */
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	/* Wait till PLL is used as system clock source */
	while (RCC_GetSYSCLKSource() != 0x08);
#endif
#if MCU_FREQUENCY == MCU_SPEED_LOW
	RCC_HCLKConfig(RCC_SYSCLK_Div4);
#elif MCU_FREQUENCY == MCU_SPEED_HALF
	RCC_HCLKConfig(RCC_SYSCLK_Div2);
#endif
	
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
	RCC_AHB1PeriphClockCmd(	RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD |
							RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG | 
							RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOI, ENABLE);
	
	xGpio.GPIO_Pin = GPIO_Pin_All;
	xGpio.GPIO_Mode = GPIO_Mode_AIN;
	xGpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &xGpio);
	GPIO_Init(GPIOC, &xGpio);
	GPIO_Init(GPIOD, &xGpio);
	GPIO_Init(GPIOE, &xGpio);
	GPIO_Init(GPIOF, &xGpio);
	GPIO_Init(GPIOG, &xGpio);
	GPIO_Init(GPIOH, &xGpio);
	GPIO_Init(GPIOI, &xGpio);
	
	RCC_AHB1PeriphClockCmd(	RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD |
							RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG | 
							RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOI, DISABLE);
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

