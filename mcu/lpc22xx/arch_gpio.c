


//Private Defines
#define LPC_PINCON_SEL_BASE				0xE002C000


//Private Variables
static LPC_GPIO_TypeDef * const lpc22xx_tblGpioBase[] = {
	LPC_GPIO0,
	LPC_GPIO1,
	LPC_GPIO2,
	LPC_GPIO3,
};



//Internal Functions




//External Functions
void arch_GpioSel(uint nPort, uint nPin, uint nSel)
{
	uint nMove;
	u32 nBase, nValue;

	if (nPort != GPIO_P0)
		return;
	
	if (nPin < 16)
	{
		nBase = LPC_PINCON_SEL_BASE;
		nMove = nPin << 1;
	}
	else
	{
		nBase = LPC_PINCON_SEL_BASE + 4;
		nMove = (nPin - 16) << 1;
	}
	
	//Set GPIO function
	nValue = __raw_readl(nBase) & ~(3 << nMove);
	nValue |= (nSel << nMove);
	__raw_writel(nValue, nBase);
}

void arch_GpioConf(uint nPort, uint nPin, uint nMode, uint nInit)
{
	LPC_GPIO_TypeDef *p = lpc22xx_tblGpioBase[nPort];

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

	//Set as GPIO function
	arch_GpioSel(nPort, nPin, 0);
}

void arch_GpioSet(uint nPort, uint nPin, uint nHL)
{
	LPC_GPIO_TypeDef *p = lpc22xx_tblGpioBase[nPort];

	if (nHL)
		p->FIOSET = BITMASK(nPin);
	else
		p->FIOCLR = BITMASK(nPin);
}

int arch_GpioRead(uint nPort, uint nPin)
{
	LPC_GPIO_TypeDef *p = lpc22xx_tblGpioBase[nPort];

	return GETBIT(p->FIOPIN, nPin);
}


