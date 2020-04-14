

//Private Consts
static const u16 _tbl_stm32_rcc_periph_gpio[] =
{
	RCC_APB2Periph_GPIOA,
	RCC_APB2Periph_GPIOB,
	RCC_APB2Periph_GPIOC,
	RCC_APB2Periph_GPIOD,
	RCC_APB2Periph_GPIOE,
	RCC_APB2Periph_GPIOF,
	RCC_APB2Periph_GPIOG
};

static GPIO_TypeDef * const _tbl_stm32_gpio_base[] =
{
	GPIOA,
	GPIOB,
	GPIOC,
	GPIOD,
	GPIOE,
	GPIOF,
	GPIOG
};



//Internal Functions
static GPIOMode_TypeDef stm32_GpioFunDef(int mode)
{

	switch (mode)
	{
	case GPIO_M_IN_ANALOG:
		return GPIO_Mode_AIN;
	case GPIO_M_IN_PD:
		return GPIO_Mode_IPD;
	case GPIO_M_IN_PU:
		return GPIO_Mode_IPU;
	case GPIO_M_OUT_OD:
		return GPIO_Mode_Out_OD;
	case GPIO_M_OUT_PP:
		return GPIO_Mode_Out_PP;
	case GPIO_M_AF_OD:
		return GPIO_Mode_AF_OD;
	case GPIO_M_AF_PP:
		return GPIO_Mode_AF_PP;
	default:
		return GPIO_Mode_IN_FLOATING;
	}
}


//External Functions
GPIO_TypeDef *arch_GpioPortBase(int port)
{
	
	return _tbl_stm32_gpio_base[port];
}

void arch_GpioClockEnable(int port)
{

	RCC_APB2PeriphClockCmd(_tbl_stm32_rcc_periph_gpio[port], ENABLE);
}

void arch_GpioConf(int port, int pin, int mode, int init)
{
	GPIO_InitTypeDef xGpio;

	xGpio.GPIO_Pin = BITMASK(pin);
	xGpio.GPIO_Mode = stm32_GpioFunDef(mode);
	xGpio.GPIO_Speed = GPIO_Speed_50MHz;
	arch_GpioClockEnable(port);
	
	if (mode & GPIO_M_OUT_MASK)
	{
		switch (init)
		{
		case GPIO_INIT_HIGH:
			arch_GpioSet(port, pin, 1);
			break;
		case GPIO_INIT_LOW:
			arch_GpioSet(port, pin, 0);
			break;
		default:
			break;
		}
	}
	
	GPIO_Init(arch_GpioPortBase(port), &xGpio);
}

void arch_GpioSet(int port, int pin, int HL)
{

	if (HL)
		GPIO_SetBits(arch_GpioPortBase(port), BITMASK(pin));
	else
		GPIO_ResetBits(arch_GpioPortBase(port), BITMASK(pin));
}

int arch_GpioRead(int port, int pin)
{

	return GPIO_ReadInputDataBit(arch_GpioPortBase(port), BITMASK(pin));
}



