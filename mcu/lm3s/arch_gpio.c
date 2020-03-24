


//Private Variables
static uint const lm3s_tblGpioBase[] = {
	GPIO_PORTA_BASE,
	GPIO_PORTB_BASE,
	GPIO_PORTC_BASE,
	GPIO_PORTD_BASE,
	GPIO_PORTE_BASE,
	GPIO_PORTF_BASE,
	GPIO_PORTG_BASE,
	GPIO_PORTH_BASE,
	NULL,
	GPIO_PORTJ_BASE,
};

static uint const lm3s_tblSysctlGpio[] = {
	SYSCTL_PERIPH_GPIOA,
	SYSCTL_PERIPH_GPIOB,
	SYSCTL_PERIPH_GPIOC,
	SYSCTL_PERIPH_GPIOD,
	SYSCTL_PERIPH_GPIOE,
	SYSCTL_PERIPH_GPIOF,
	SYSCTL_PERIPH_GPIOG,
	SYSCTL_PERIPH_GPIOH,
	NULL,
	SYSCTL_PERIPH_GPIOJ,
};





//Internal Functions




//External Functions
void arch_GpioClockEnable(uint nPort)
{

	MAP_SysCtlPeripheralEnable(lm3s_tblSysctlGpio[nPort]);
}

u32 arch_GpioPortBase(uint nPort)
{

#if 0
	switch (nPort)
	{
	case GPIO_P0:
		if (__raw_readl(SYSCTL_GPIOHBCTL) & BITMASK(0))
			return GPIO_PORTA_AHB_BASE;
		else
			return GPIO_PORTA_BASE;
		
	case GPIO_P1:
		if (__raw_readl(SYSCTL_GPIOHBCTL) & BITMASK(1))
			return GPIO_PORTB_AHB_BASE;
		else
			return GPIO_PORTB_BASE;
		
	case GPIO_P2:
		if (__raw_readl(SYSCTL_GPIOHBCTL) & BITMASK(2))
			return GPIO_PORTC_AHB_BASE;
		else
			return GPIO_PORTC_BASE;
		
	case GPIO_P3:
		if (__raw_readl(SYSCTL_GPIOHBCTL) & BITMASK(3))
			return GPIO_PORTD_AHB_BASE;
		else
			return GPIO_PORTD_BASE;
		
	case GPIO_P4:
		if (__raw_readl(SYSCTL_GPIOHBCTL) & BITMASK(4))
			return GPIO_PORTE_AHB_BASE;
		else
			return GPIO_PORTE_BASE;
		
	case GPIO_P5:
		if (__raw_readl(SYSCTL_GPIOHBCTL) & BITMASK(5))
			return GPIO_PORTF_AHB_BASE;
		else
			return GPIO_PORTF_BASE;
		
	case GPIO_P6:
		if (__raw_readl(SYSCTL_GPIOHBCTL) & BITMASK(6))
			return GPIO_PORTG_AHB_BASE;
		else
			return GPIO_PORTG_BASE;
		
	case GPIO_P7:
		if (__raw_readl(SYSCTL_GPIOHBCTL) & BITMASK(7))
			return GPIO_PORTH_AHB_BASE;
		else
			return GPIO_PORTH_BASE;
		
	case GPIO_P9:
		if (__raw_readl(SYSCTL_GPIOHBCTL) & BITMASK(8))
			return GPIO_PORTJ_AHB_BASE;
		else
			return GPIO_PORTJ_BASE;
		
	default:
		return NULL;
	}
#else

	return lm3s_tblGpioBase[nPort];
#endif
}

void arch_GpioConf(uint nPort, uint nPin, uint nMode, uint nInit)
{
	uint nBase;

	nBase = arch_GpioPortBase(nPort);
	nPin = BITMASK(nPin);
	arch_GpioClockEnable(nPort);
    //PB7(NMI)
    if ((nPort == GPIO_P1) && (nPin == 0x80))
	{
        __raw_writel(GPIO_LOCK_KEY_DD, GPIO_PORTB_BASE + GPIO_O_LOCK);
        __raw_writel(0x80, GPIO_PORTB_BASE + GPIO_O_CR);
    }
	
	switch (nMode)
	{
	case GPIO_M_IN_ANALOG:
		MAP_GPIODirModeSet(nBase, nPin, GPIO_DIR_MODE_IN);
		MAP_GPIOPadConfigSet(nBase, nPin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_ANALOG);
		break;
		
	case GPIO_M_IN_PD:
		MAP_GPIODirModeSet(nBase, nPin, GPIO_DIR_MODE_IN);
		MAP_GPIOPadConfigSet(nBase, nPin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
		break;
		
	case GPIO_M_IN_PU:
		MAP_GPIODirModeSet(nBase, nPin, GPIO_DIR_MODE_IN);
		MAP_GPIOPadConfigSet(nBase, nPin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
		break;
		
	case GPIO_M_IN_FLOAT:
		MAP_GPIODirModeSet(nBase, nPin, GPIO_DIR_MODE_IN);
		MAP_GPIOPadConfigSet(nBase, nPin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
		break;
		
	case GPIO_M_OUT_OD:
		MAP_GPIODirModeSet(nBase, nPin, GPIO_DIR_MODE_OUT);
		MAP_GPIOPadConfigSet(nBase, nPin, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_OD);
		break;
		
	case GPIO_M_AF_OD:
		MAP_GPIODirModeSet(nBase, nPin, GPIO_DIR_MODE_HW);
		MAP_GPIOPadConfigSet(nBase, nPin, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_OD);
		break;
		
	case GPIO_M_OUT_PP:
		MAP_GPIODirModeSet(nBase, nPin, GPIO_DIR_MODE_OUT);
		MAP_GPIOPadConfigSet(nBase, nPin, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
		break;
		
	case GPIO_M_AF_PP:
		MAP_GPIODirModeSet(nBase, nPin, GPIO_DIR_MODE_HW);
		MAP_GPIOPadConfigSet(nBase, nPin, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
		break;
		
	default:
		break;
	}
	
	if (nMode & GPIO_M_OUT_MASK)
	{
		switch (nInit)
		{
		case GPIO_INIT_HIGH:
			MAP_GPIOPinWrite(nBase, nPin, 0xFF);
			break;
		case GPIO_INIT_LOW:
			MAP_GPIOPinWrite(nBase, nPin, 0);
			break;
		default:
			break;
		}
	}
	
	//PB7(NMI)
    if ((nPort == GPIO_P1) && (nPin == 0x80))
	{
        __raw_writel(GPIO_LOCK_KEY_DD, GPIO_PORTB_BASE + GPIO_O_LOCK);
        __raw_writel(0x00, GPIO_PORTB_BASE + GPIO_O_CR);
	}
}

void arch_GpioSet(uint nPort, uint nPin, uint nHL)
{

	if (nHL)
		MAP_GPIOPinWrite(arch_GpioPortBase(nPort), BITMASK(nPin), 0xFF);
	else
		MAP_GPIOPinWrite(arch_GpioPortBase(nPort), BITMASK(nPin), 0);
}

int arch_GpioRead(uint nPort, uint nPin)
{

	return MAP_GPIOPinRead(arch_GpioPortBase(nPort), BITMASK(nPin)) ? 1 : 0;
}



