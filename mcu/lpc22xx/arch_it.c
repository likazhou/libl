#if IRQ_ENABLE



//Private Functions
static uint lpc22xx_ExtIrqId(uint nPin)
{

	return 0;
}


//External Functions
int arch_ExtIrqRegister(uint nPort, uint nPin, uint nTriggerMode)
{

	rt_hw_interrupt_install(EINT0_INT, EINT0_IRQHandler, RT_NULL);
	return 0;
}

void arch_ExtIrqEnable(uint nPort, uint nPin, uint nMode)
{
	uint nId;
	
	nId = lpc22xx_ExtIrqId(nPin);
	SETBIT(EXTMODE, nId);
	if (nMode == IRQ_TRIGGER_FALLING)
		CLRBIT(EXTPOLAR, nId);
	else
		SETBIT(EXTPOLAR, nId);
	SETBIT(EXTINT, nId);
	rt_hw_interrupt_umask(EINT0_INT);
}

void arch_ExtIrqDisable(uint nPort, uint nPin)
{

	rt_hw_interrupt_mask(EINT0_INT);
}

void arch_ExtIrqRxConf(uint nPort, uint nPin)
{
	
	arch_GpioSel(nPort, nPin, 1);
}

//Interrupt Functions
void WDT_IRQHandler(int vector)
{
}

void TIMER0_IRQHandler(int vector)
{

	irq_TimerISR(0);
	arch_TimerIntClear(0);
}

void TIMER1_IRQHandler(int vector)
{

	irq_TimerISR(1);
	arch_TimerIntClear(1);
}

void UART0_IRQHandler(int vector)
{
	
	arch_UartISR(0);
}

void UART1_IRQHandler(int vector)
{

	arch_UartISR(1);
}

void EINT0_IRQHandler(int vector)
{

#if IRQ_EXT_ENABLE
	irq_ExtISR(0);
	SETBIT(EXTINT, 0);
#endif
}

void EINT1_IRQHandler(int vector)
{

#if IRQ_EXT_ENABLE
	irq_ExtISR(1);
	SETBIT(EXTINT, 1);
#endif
}

void EINT2_IRQHandler(int vector)
{

#if IRQ_EXT_ENABLE
	irq_ExtISR(2);
	SETBIT(EXTINT, 2);
#endif
}

void EINT3_IRQHandler(int vector)
{

#if IRQ_EXT_ENABLE
	irq_ExtISR(3);
	SETBIT(EXTINT, 3);
#endif
}

#endif

