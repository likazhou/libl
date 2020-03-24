#if IRQ_TIMER_ENABLE



//Private Const
static LPC_TIM_TypeDef * const lpc22xx_tblTimerBase[] = {
	LPC_TIM0,
	LPC_TIM1,
};




void arch_TimerInit(uint nId)
{

	rt_hw_interrupt_install(TIMER0_INT, TIMER0_IRQHandler, RT_NULL);
}

void arch_TimerIntClear(uint nId)
{
	LPC_TIM_TypeDef *pTimer = lpc22xx_tblTimerBase[0];

	pTimer->IR = 1;
}

void arch_TimerStart(uint nId, uint nValue)
{
	LPC_TIM_TypeDef *pTimer = lpc22xx_tblTimerBase[0];

	pTimer->TCR = BITMASK(1);
	pTimer->TC = 0;
	pTimer->IR = 0xFF;
	pTimer->PR = 0;
	pTimer->PC = 0;
	pTimer->MR0 = nValue;
	pTimer->MCR = BITMASK(0) | BITMASK(1);
	pTimer->CCR0 = 0x0C30;
	pTimer->TCR = BITMASK(0);
	rt_hw_interrupt_umask(TIMER0_INT);
}

void arch_TimerStop(uint nId)
{

	rt_hw_interrupt_mask(TIMER0_INT);
}

void arch_TimerCapConf(uint nPort, uint nPin)
{

	arch_GpioSel(nPort, nPin, 2);
}

#if 0
void arch_TimerCapStart(uint nId, uint nValue)
{
	LPC_TIM_TypeDef *pTimer = lpc22xx_tblTimerBase[nId];

	pTimer->TCR = BITMASK(1);
	pTimer->TC = 0;
	pTimer->IR = 0xFF;
	pTimer->PR = 0;
	pTimer->PC = 0;
	pTimer->CCR0 = 0x0C30;
	pTimer->MR1 = nValue / 2;
	pTimer->MR2 = nValue;
	pTimer->TCR = BITMASK(0);
}
#else
void arch_TimerCapStart(uint nId, uint nValue)
{
//	LPC_TIM_TypeDef *pTimer = lpc22xx_tblTimerBase[nId];

  	T0TC=0;
  	T0TCR=0x03;
  	T0TCR=0;
  	T0PR=0;
  	T0PC=0;                                 	//不分频
  	T0CCR=0x0C30;                            	//设定捕捉CAP3下降沿并中断
  	T0MR1=nValue / 2;
  	T0MR2=nValue;
  	T0IR=0xff;

  	T0EMR=0;

	rt_hw_interrupt_umask(TIMER0_INT);
}
#endif

#endif
