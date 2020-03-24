



//Private Const
static LPC_TIM_TypeDef * const lpc176x_tblTimerBase[] = {
	LPC_TIM0,
	LPC_TIM1,
	LPC_TIM2,
	LPC_TIM3
};




void arch_TimerInit(uint nId)
{

	switch (nId) {
	case 0:
		SETBIT(LPC_SC->PCONP, 1);
		NVIC_EnableIRQ(TIMER0_IRQn);
		break;
	case 1:
		SETBIT(LPC_SC->PCONP, 2);
		NVIC_EnableIRQ(TIMER1_IRQn);
		break;
	case 2:
		SETBIT(LPC_SC->PCONP, 22);
		NVIC_EnableIRQ(TIMER2_IRQn);
		break;
	case 3:
		SETBIT(LPC_SC->PCONP, 23);
		NVIC_EnableIRQ(TIMER3_IRQn);
		break;
	default:
		break;
	}
}

void arch_TimerIntClear(uint nId)
{
	LPC_TIM_TypeDef *pTimer = lpc176x_tblTimerBase[nId];

	pTimer->IR = BITMASK(0);
}

void arch_TimerStart(uint nId, uint nValue)
{
	LPC_TIM_TypeDef *pTimer = lpc176x_tblTimerBase[nId];

	pTimer->TCR = BITMASK(1);
	pTimer->TC = 0;
	pTimer->IR = BITMASK(0);
	pTimer->CTCR = 0;
	pTimer->PR = 0;
	pTimer->PC = 0;
	pTimer->MR0 = nValue;
	pTimer->MCR = BITMASK(0) | BITMASK(1);
	pTimer->TCR = BITMASK(0);
}

void arch_TimerStop(uint nId)
{
	LPC_TIM_TypeDef *pTimer = lpc176x_tblTimerBase[nId];

	pTimer->TCR = 0;
}



