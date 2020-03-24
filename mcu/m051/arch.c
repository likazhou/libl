



//Internal Functions
static void m051_RccInit()
{

	UNLOCKREG();

    SYSCLK->PWRCON.XTL12M_EN = 1;
    /* Waiting for 12M Xtal stalble */
	while (SYSCLK->CLKSTATUS.XTL12M_STB == 0);

	/* PLL Enable */
	SYSCLK->PLLCON.PD = 0;
	SYSCLK->PLLCON.OE = 0;
	/* Waiting for PLL stalble */
	while (SYSCLK->CLKSTATUS.PLL_STB == 0);

	/* Select PLL as system clock */
	SYSCLK->CLKSEL0.HCLK_S = 2;

	LOCKREG();

	/* init systick */
	SysTick_Config(XTAL / RT_TICK_PER_SECOND - 1);
}

static void m051_IrqInit()
{

	/* set pend exception priority */
	NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
}

static void m051_GpioIdleInit()
{

	//将所有没有使用的IO置为模拟输入
	//可以降低功耗和改善EMC/EMI性能
}


#if DEBUG_ENABLE
static void m051_DbgInit()
{

}
#endif

//External Functions
void arch_Init()
{

	//时钟系统初始化
	m051_RccInit();
#if DEBUG_ENABLE
	m051_DbgInit();
#endif
	//中断初始化
	m051_IrqInit();
	//GPIO初始化
	m051_GpioIdleInit();
}


#if IDLE_ENABLE
void arch_IdleEntry()
{

}
#endif


void arch_Reset()
{

}



