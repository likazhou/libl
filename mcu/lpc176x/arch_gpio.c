


//Private Defines
#define LPC_PINCON_SEL_BASE				LPC_PINCON_BASE
#define LPC_PINCON_MODE_BASE			(LPC_PINCON_SEL_BASE + 0x40)
#define LPC_PINCON_MODEOD_BASE			(LPC_PINCON_MODE_BASE + 40)

//Private Variables
static LPC_GPIO_TypeDef * const lpc176x_tblGpioBase[] = {
	LPC_GPIO0,
	LPC_GPIO1,
	LPC_GPIO2,
	LPC_GPIO3,
	LPC_GPIO4,
};



//Internal Functions




//External Functions

void arch_GpioClockEnable(uint nPort)
{

	SETBIT(LPC_SC->PCONP, 15);
}

void arch_GpioSel(uint nPort, uint nPin, uint nSel)
{
	uint nMove;
	u32 nBase, nValue;

	if (nPin < 16)
	{
		nBase = LPC_PINCON_SEL_BASE + nPort * 2 * 4;
		nMove = nPin << 1;
	}
	else
	{
		nBase = LPC_PINCON_SEL_BASE + (nPort * 2 + 1) * 4;
		nMove = (nPin - 16) << 1;
	}
	
	//Set GPIO function
	nValue = __raw_readl(nBase) & ~(3 << nMove);
	nValue |= (nSel << nMove);
	
	__raw_writel(nValue, nBase);
}

void arch_GpioConf(uint nPort, uint nPin, uint nMode, uint nInit)
{
	LPC_GPIO_TypeDef *p = lpc176x_tblGpioBase[nPort];
	uint nMove;
	u32 nBase, nValue;

	arch_GpioClockEnable(nPort);
	if (nMode & GPIO_M_OUT_MASK)
	{
		switch (nInit)
		{
		case GPIO_INIT_HIGH:
			SETBIT(p->FIOSET, nPin);
			break;
		case GPIO_INIT_LOW:
			SETBIT(p->FIOCLR, nPin);
			break;
		default:
			break;
		}
		SETBIT(p->FIODIR, nPin);
	}
	else
	{
		CLRBIT(p->FIODIR, nPin);
	}
	
	//Set as OD Mode
	switch (nMode)
	{
	case GPIO_M_OUT_OD:
	case GPIO_M_AF_OD:
		nBase = LPC_PINCON_MODEOD_BASE + nPort * 4;
		__raw_writel(__raw_readl(nBase) | BITMASK(nPin), nBase);
		break;
	default:
		break;
	}
	
	//Set GPIO Mode
	if (nPin < 16)
	{
		nBase = LPC_PINCON_MODE_BASE + nPort * 2 * 4;
		nMove = nPin << 1;
	}
	else
	{
		nBase = LPC_PINCON_MODE_BASE + (nPort * 2 + 1) * 4;
		nMove = (nPin - 16) << 1;
	}
	nValue = __raw_readl(nBase) & ~(3 << nMove);
	
	switch (nMode)
	{
	case GPIO_M_IN_ANALOG:
		break;
	case GPIO_M_IN_PD:
	case GPIO_M_OUT_OD:
	case GPIO_M_AF_OD:
		nValue |= (3 << nMove);
		break;
	case GPIO_M_IN_PU:
		break;
	case GPIO_M_IN_FLOAT:
		nValue |= (2 << nMove);
		break;
	case GPIO_M_OUT_PP:
	case GPIO_M_AF_PP:
		nValue |= (1 << nMove);
		break;
	default:
		break;
	}
	
	__raw_writel(nValue, nBase);
	//Set as GPIO function
	arch_GpioSel(nPort, nPin, 0);
}

void arch_GpioSet(uint nPort, uint nPin, uint nHL)
{
	LPC_GPIO_TypeDef *p = lpc176x_tblGpioBase[nPort];

	if (nHL)
		p->FIOSET = BITMASK(nPin);
	else
		p->FIOCLR = BITMASK(nPin);
}

int arch_GpioRead(uint nPort, uint nPin)
{
	LPC_GPIO_TypeDef *p = lpc176x_tblGpioBase[nPort];

	return GETBIT(p->FIOPIN, nPin);
}


