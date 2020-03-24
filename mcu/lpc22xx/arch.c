#include <LPC22xx.H>



//Internal Functions
static void lpc22xx_RccInit()
{

}

#if IRQ_ENABLE
static void lpc22xx_IrqInit()
{

	rt_hw_interrupt_init();

	/* prescaler = 0*/
	T1PR = 0;
	T1PC = 0;

	/* reset and enable MR0 interrupt */
	T1MCR = 0x3;
	T1MR0 = PERI_CLOCK / RT_TICK_PER_SECOND;

	/* enable timer 0 */
	T1TCR = 1;

	/* install timer handler */
	rt_hw_interrupt_install(TIMER1_INT, rt_hw_timer_handler, RT_NULL);
	rt_hw_interrupt_umask(TIMER1_INT);
}
#endif

static void lpc22xx_GpioIdleInit()
{

	//将所有没有使用的IO置为模拟输入
	//可以降低功耗和改善EMC/EMI性能
}



//External Functions
void arch_Init()
{

	//时钟系统初始化
	lpc22xx_RccInit();
	//中断初始化
#if IRQ_ENABLE
	lpc22xx_IrqInit();
#endif
	//GPIO初始化
	lpc22xx_GpioIdleInit();
}


void arch_IdleEntry()
{

	__WFI();
}


void arch_Reset()
{

#if OS_TYPE
	os_interrupt_Disable();
#endif
	while(1);
}


