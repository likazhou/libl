

//Private Consts
static const u16 stm32_tbl_rcc_periph_gpio[] =
{
	LL_AHB1_GRP1_PERIPH_GPIOA,
	LL_AHB1_GRP1_PERIPH_GPIOB,
	LL_AHB1_GRP1_PERIPH_GPIOC,
	LL_AHB1_GRP1_PERIPH_GPIOD,
	LL_AHB1_GRP1_PERIPH_GPIOE,
	LL_AHB1_GRP1_PERIPH_GPIOF,
	LL_AHB1_GRP1_PERIPH_GPIOG,
	LL_AHB1_GRP1_PERIPH_GPIOH,
	LL_AHB1_GRP1_PERIPH_GPIOI,
#if defined(STM32F427xx) || defined(STM32F429xx)
	LL_AHB1_GRP1_PERIPH_GPIOJ,
	LL_AHB1_GRP1_PERIPH_GPIOK,
#endif
};

static GPIO_TypeDef * const stm32_tbl_gpio_base[] =
{
	GPIOA,
	GPIOB,
	GPIOC,
	GPIOD,
	GPIOE,
	GPIOF,
	GPIOG,
	GPIOH,
	GPIOI,
#if defined(STM32F427xx) || defined(STM32F429xx)
	GPIOJ,
	GPIOK,
#endif
};



//Internal Functions
static u32 stm32_GpioModeDef(int mode)
{

	switch (mode)
	{
	case GPIO_M_IN_ANALOG:
	case GPIO_M_IN_PD:
	case GPIO_M_IN_PU:
		return LL_GPIO_MODE_INPUT;
	case GPIO_M_OUT_OD:
	case GPIO_M_OUT_PP:
		return LL_GPIO_MODE_OUTPUT;
	case GPIO_M_AF_OD:
	case GPIO_M_AF_PP:
		return LL_GPIO_MODE_ALTERNATE;
	default:
		return LL_GPIO_MODE_ANALOG;
	}
}

static u32 stm32_GpioOutTypeDef(int mode)
{

	switch (mode)
	{
	case GPIO_M_OUT_PP:
	case GPIO_M_AF_PP:
		return LL_GPIO_OUTPUT_PUSHPULL;
	default:
		return LL_GPIO_OUTPUT_OPENDRAIN;
	}
}

static u32 stm32_GpioPullDef(int mode)
{

	switch (mode)
	{
	case GPIO_M_IN_PD:
		return LL_GPIO_PULL_DOWN;
	case GPIO_M_IN_PU:
		return LL_GPIO_PULL_UP;
	default:
		return LL_GPIO_PULL_NO;
	}
}




//External Functions
GPIO_TypeDef *arch_GpioPortBase(int port)
{
	
	return stm32_tbl_gpio_base[port];
}

void arch_GpioClockEnable(int port)
{

	LL_AHB1_GRP1_EnableClock(stm32_tbl_rcc_periph_gpio[port]);
}

void arch_GpioConf(int port, int pin, int mode, int init)
{
	LL_GPIO_InitTypeDef xGpio;

	xGpio.Pin = BITMASK(pin);
	xGpio.Mode = stm32_GpioModeDef(mode);
	xGpio.OutputType = stm32_GpioOutTypeDef(mode);
	xGpio.Pull = stm32_GpioPullDef(mode);
	xGpio.Speed = LL_GPIO_SPEED_FREQ_LOW;
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
	
	LL_GPIO_Init(arch_GpioPortBase(port), &xGpio);
}

void arch_GpioSet(int port, int pin, int HL)
{

	if (HL)
		LL_GPIO_SetOutputPin(arch_GpioPortBase(port), BITMASK(pin));
	else
		LL_GPIO_ResetOutputPin(arch_GpioPortBase(port), BITMASK(pin));
}

int arch_GpioRead(int port, int pin)
{

	return LL_GPIO_IsInputPinSet(arch_GpioPortBase(port), BITMASK(pin));
}




