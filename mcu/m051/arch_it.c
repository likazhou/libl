



//Private functions




//External Functions
int arch_ExtIrqRegister(uint nPort, uint nPin, uint nTriggerMode)
{

}

void arch_ExtIrqEnable(uint nPort, uint nPin, uint nMode)
{

}

void arch_ExtIrqDisable(uint nPort, uint nPin)
{

}


//Interrupt Functions
void WDT_IRQHandler()
{
}

void EINT0_IRQHandler()
{
}

void EINT1_IRQHandler()
{
}

void TMR0_IRQHandler()
{

	os_irq_Enter();

	os_irq_Leave();
}

void TMR1_IRQHandler()
{

	os_irq_Enter();

	os_irq_Leave();
}

void TMR2_IRQHandler()
{

	os_irq_Enter();

	os_irq_Leave();
}

void TMR3_IRQHandler()
{

	os_irq_Enter();

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



