


//Internal Functions
static void lpc176x_RccInit()
{

	SystemInit();

	/* Set the Vector Table base location */
	SCB->VTOR  = (BOOTLOADER_SIZE & 0x3FFFFF80);

	/* set pend exception priority */
	NVIC_SetPriority(PendSV_IRQn, (1<<__NVIC_PRIO_BITS) - 1);

	/* init systick */
	SysTick_Config(__CORE_CLK / RT_TICK_PER_SECOND - 1);
}

static void lpc176x_IrqInit()
{

}

static void lpc176x_GpioIdleInit()
{

	//将所有没有使用的IO置为模拟输入
	//可以降低功耗和改善EMC/EMI性能
}



//External Functions
void arch_Init()
{

	//时钟系统初始化
	lpc176x_RccInit();
	//中断初始化
	lpc176x_IrqInit();
	//GPIO初始化
	lpc176x_GpioIdleInit();
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


