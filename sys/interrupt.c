

//Private Variables
#if IRQ_HALF_ENABLE
static os_sem_t irq_sem;
#endif

#if IRQ_EXT_ENABLE
static t_irq_vector irq_aExt[ARCH_EXTIRQ_QTY];
#endif
#if IRQ_TIMER_ENABLE
static t_irq_vector irq_aTimer[ARCH_TIMER_QTY];
#endif




void irq_Init()
{

#if IRQ_HALF_ENABLE
	os_sem_init(&irq_sem, 0);
#endif
}

void irq_VectorInit()
{
	t_irq_vector *p;

#if IRQ_EXT_ENABLE
	for (p = irq_aExt; p < ARR_ENDADR(irq_aExt); p++)
	{
#if IRQ_HALF_ENABLE
		p->bottom = 0;
#endif
		p->handler = NULL;
	}
#endif
	
#if IRQ_TIMER_ENABLE
	for (p = irq_aTimer; p < ARR_ENDADR(irq_aTimer); p++)
	{
#if IRQ_HALF_ENABLE
		p->bottom = 0;
#endif
		p->handler = NULL;
	}
#endif
}

#if IRQ_HALF_ENABLE
sys_res irq_Wait()
{

	if (os_sem_waittmo(&irq_sem, 0) == RT_EOK)
		return SYS_R_OK;
	
	return SYS_R_TMO;
}

void irq_BottomHandler()
{
	t_irq_vector *p;

	for (p = irq_aExt; p < ARR_ENDADR(irq_aExt); p++)
	{
		if (p->bottom)
		{
			p->bottom = 0;
			(p->handler)(p->args);
		}
	}
	
	for (p = irq_aTimer; p < ARR_ENDADR(irq_aTimer); p++)
	{
		if (p->bottom)
		{
			p->bottom = 0;
			(p->handler)(p->args);
		}
	}
}
#endif


#if IRQ_EXT_ENABLE
int irq_ExtRegister(int nPort, int nPin, int nTriggerMode, void (*pHandler)(void *), 
							void *args, int nHandlerMode)
{
	int nId;
	t_irq_vector *p;

	nId = arch_ExtIrqRegister(nPort, nPin, nTriggerMode);
	if (nId == -1)
		return -1;
	
	p = &irq_aExt[nId];
	
	p->mode = nHandlerMode;
	p->port = nPort;
	p->pin = nPin;
	p->handler = pHandler;
	p->args = args;
	
	return nId;
}

void irq_ExtEnable(int nId)
{
	t_irq_vector *p = &irq_aExt[nId];

	arch_ExtIrqEnable(p->port, p->pin, p->mode);
}

void irq_ExtDisable(int nId)
{
	t_irq_vector *p = &irq_aExt[nId];

	arch_ExtIrqDisable(p->port, p->pin);
}

void irq_ExtISR(int nId)
{
	t_irq_vector *p = &irq_aExt[nId];

#if IRQ_HALF_ENABLE
	if (p->mode == IRQ_MODE_NORMAL)
	{
		(p->handler)(p->args);
	}
	else
	{
		p->bottom = 1;
		os_sem_signal(&irq_sem);
	}
#else
	(p->handler)(p->args);
#endif
}
#endif


#if IRQ_TIMER_ENABLE
sys_res irq_TimerRegister(int nId, void (*pHandler)(void *), void *args)
{
	t_irq_vector *p = &irq_aTimer[nId];

	arch_TimerInit(nId);

	p->handler = pHandler;
	p->args = args;

	return SYS_R_OK;
}

void irq_TimerHandler(int nId, void (*pHandler)(void *), void *args)
{
	t_irq_vector *p = &irq_aTimer[nId];

	p->handler = pHandler;
	p->args = args;
}


void irq_TimerStart(int nId, u32 nValue)
{

	arch_TimerStart(nId, nValue);
}

void irq_TimerStop(int nId)
{

	arch_TimerStop(nId);
}

void irq_TimerISR(int nId)
{
	t_irq_vector *p = &irq_aTimer[nId];

	(p->handler)(p->args);
}
#endif


