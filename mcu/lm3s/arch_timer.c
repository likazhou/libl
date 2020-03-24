



//Private Const
static u32 const lm3s_tblTimerSysCtl[] = {
	SYSCTL_PERIPH_TIMER0,
	SYSCTL_PERIPH_TIMER1,
	SYSCTL_PERIPH_TIMER2,
	SYSCTL_PERIPH_TIMER3
};
static u32 const lm3s_tblTimerBase[] = {
	TIMER0_BASE,
	TIMER1_BASE,
	TIMER2_BASE,
	TIMER3_BASE
};
static uint const lm3s_tblTimerInt[] = {
	INT_TIMER0A,
	INT_TIMER0B,
	INT_TIMER1A,
	INT_TIMER1B,
	INT_TIMER2A,
	INT_TIMER2B,
	INT_TIMER3A,
	INT_TIMER3B,
};




void arch_TimerInit(uint nId)
{
	uint nBase = lm3s_tblTimerBase[nId >> 1];

	MAP_SysCtlPeripheralEnable(lm3s_tblTimerSysCtl[nId >> 1]);
	MAP_TimerConfigure(nBase, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC | TIMER_CFG_B_PERIODIC);
	MAP_IntEnable(lm3s_tblTimerInt[nId]);
	MAP_TimerEnable(nBase, TIMER_A | TIMER_B);
}

void arch_TimerIntClear(uint nId)
{
	uint nTimer;

	if (nId & 1)
		nTimer = TIMER_TIMB_TIMEOUT;
	else
		nTimer = TIMER_TIMA_TIMEOUT;
	
	MAP_TimerIntClear(lm3s_tblTimerBase[nId >> 1], nTimer);
}

void arch_TimerStart(uint nId, uint nValue)
{
	uint nBase, nTimer;

	nBase = lm3s_tblTimerBase[nId >> 1];
	
	if (nId & 1)
	{
		__raw_writel(nValue, nBase + TIMER_O_TBILR);
		__raw_writel(nValue, nBase + TIMER_O_TBV);
	}
	else
	{
		__raw_writel(nValue, nBase + TIMER_O_TAILR);
		__raw_writel(nValue, nBase + TIMER_O_TAV);
	}
	
	if (nId & 1)
		nTimer = TIMER_TIMB_TIMEOUT;
	else
		nTimer = TIMER_TIMA_TIMEOUT;
	
	MAP_TimerIntClear(nBase, nTimer);
	MAP_TimerIntEnable(nBase, nTimer);
}

void arch_TimerStop(uint nId)
{
	uint nTimer;

	if (nId & 1)
		nTimer = TIMER_TIMB_TIMEOUT;
	else
		nTimer = TIMER_TIMA_TIMEOUT;
	
	MAP_TimerIntDisable(lm3s_tblTimerBase[nId >> 1], nTimer);
}

uint arch_TimerClockGet()
{

	return MAP_SysCtlClockGet();
}

