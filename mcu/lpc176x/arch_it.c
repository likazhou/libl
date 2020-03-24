



//Private Functions
static void lpc176x_ExtIrqISR()
{
	uint i;

	if (LPC_GPIOINT->IntStatus & BITMASK(0)) {
		for (i = 0; i < ARCH_EXTIRQ_QTY / 2; i++)
			if ((LPC_GPIOINT->IO0IntStatR & BITMASK(i)) || (LPC_GPIOINT->IO0IntStatF & BITMASK(i))) {
				irq_ExtISR(i);
				SETBIT(LPC_GPIOINT->IO0IntClr, i);
			}
	}
	if (LPC_GPIOINT->IntStatus & BITMASK(2)) {
		for (i = 0; i < ARCH_EXTIRQ_QTY / 2; i++)
			if ((LPC_GPIOINT->IO2IntStatR & BITMASK(i)) || (LPC_GPIOINT->IO2IntStatF & BITMASK(i))) {
				irq_ExtISR(ARCH_EXTIRQ_QTY / 2 + i);
				SETBIT(LPC_GPIOINT->IO2IntClr, i);
			}
	}
}

static void lpc176x_TimerISR(uint nId)
{

	irq_TimerISR(nId);
	arch_TimerIntClear(nId);
}



//External Functions
int arch_ExtIrqRegister(uint nPort, uint nPin, uint nTriggerMode)
{

	NVIC_EnableIRQ(EINT3_IRQn);

	if (nPort)
		return (ARCH_EXTIRQ_QTY / 2 + nPin);
	return nPin;
}

void arch_ExtIrqEnable(uint nPort, uint nPin, uint nMode)
{

	if (nPort)
	{
		SETBIT(LPC_GPIOINT->IO2IntClr, nPin);
		if (nMode == IRQ_TRIGGER_FALLING)
			SETBIT(LPC_GPIOINT->IO2IntEnF, nPin);
		else
			SETBIT(LPC_GPIOINT->IO2IntEnR, nPin);
	}
	else
	{
		SETBIT(LPC_GPIOINT->IO0IntClr, nPin);
		if (nMode == IRQ_TRIGGER_FALLING)
			SETBIT(LPC_GPIOINT->IO0IntEnF, nPin);
		else
			SETBIT(LPC_GPIOINT->IO0IntEnR, nPin);
	}
}

void arch_ExtIrqDisable(uint nPort, uint nPin)
{

	if (nPort)
	{
		CLRBIT(LPC_GPIOINT->IO2IntEnF, nPin);
		CLRBIT(LPC_GPIOINT->IO2IntEnR, nPin);
	}
	else
	{
		CLRBIT(LPC_GPIOINT->IO0IntEnF, nPin);
		CLRBIT(LPC_GPIOINT->IO0IntEnR, nPin);
	}
}


//Interrupt Functions
void WDT_IRQHandler()
{
}

void TIMER0_IRQHandler()
{

	os_irq_Enter();

	lpc176x_TimerISR(0);

	os_irq_Leave();
}

void TIMER1_IRQHandler()
{

	os_irq_Enter();

	lpc176x_TimerISR(1);

	os_irq_Leave();
}

void TIMER2_IRQHandler()
{

	os_irq_Enter();

	lpc176x_TimerISR(2);

	os_irq_Leave();
}

void TIMER3_IRQHandler()
{

	os_irq_Enter();

	lpc176x_TimerISR(3);

	os_irq_Leave();
}

void UART0_IRQHandler()
{
	
	os_irq_Enter();

#ifdef RT_USING_FINSH
	arch_Uart0Handler();
#else
	arch_UartISR(0);
#endif

	os_irq_Leave();
}

void UART1_IRQHandler()
{

	os_irq_Enter();

	arch_UartISR(1);

	os_irq_Leave();
}

void UART2_IRQHandler()
{

	os_irq_Enter();

	arch_UartISR(2);

	os_irq_Leave();
}

void UART3_IRQHandler()
{

	os_irq_Enter();

	arch_UartISR(3);

	os_irq_Leave();
}

void EINT0_IRQHandler()
{
}

void EINT1_IRQHandler()
{
}

void EINT2_IRQHandler()
{
}

void EINT3_IRQHandler()
{

#if IRQ_ENABLE
	os_irq_Enter();

	lpc176x_ExtIrqISR();
	
	os_irq_Leave();
#endif
}


void USB_IRQHandler()
{
}


void ENET_IRQHandler()
{
	extern void arch_EmacIsr(void);

#if ETH_INT_ENABLE
	os_irq_Enter();

	arch_EmacIsr();

	os_irq_Leave();
#endif
}



