#if SC16IS7X_ENABLE
#include <drivers/sc16is7x.h>
#endif


sys_res arch_ExtirqInit(p_gpio_def p, int nTriggerMode)
{
	uint nLine;

	if (p->port != GPIO_P2)
		return SYS_R_ERR;
	
	nLine = BITMASK(p->pin - 6);
	switch (nTriggerMode)
	{
	case IRQ_MODE_LOWEDGE:
		XTI_LineModeConfig(nLine, XTI_FallingEdge);
		break;
	case IRQ_MODE_HIGHEDGE:
		XTI_LineModeConfig(nLine, XTI_RisingEdge);
		break;
	default:
		return SYS_R_ERR;
	}
	XTI_LineConfig(nLine, ENABLE);
	
	return SYS_R_OK;
}


__irq void XTI_IRQHandler()
{
	uint i, nLine, nIrq;

	os_irq_Enter();

	nIrq = XTI_InterruptLineValue();
	for (i = 0, nLine = BITMASK(2); i < ARCH_EXTIRQ_QTY; i++, nLine << 1)
	{
		if (nIrq & nLine)
		{
			irq_FlagSet(i);
			XTI_PendingBitClear(nLine);
		}
	}
	SETBIT(EIC->IPR, XTI_IRQChannel);

	os_irq_Leave();
}

__irq void UART0_IRQHandler()
{

	os_irq_Enter();

	arch_UartIoHandler(0);
	SETBIT(EIC->IPR, UART0_IRQChannel);

	os_irq_Leave();
}

__irq void UART1_IRQHandler()
{

	os_irq_Enter();

	arch_UartIoHandler(1);
	SETBIT(EIC->IPR, UART1_IRQChannel);

	os_irq_Leave();
}

__irq void UART2_IRQHandler()
{

	os_irq_Enter();

	arch_UartIoHandler(2);
	SETBIT(EIC->IPR, UART2_IRQChannel);

	os_irq_Leave();
}

__irq void UART3_IRQHandler()
{

	os_irq_Enter();

	arch_UartIoHandler(3);
	SETBIT(EIC->IPR, UART3_IRQChannel);

	os_irq_Leave();
}

