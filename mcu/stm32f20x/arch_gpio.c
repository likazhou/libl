





//Internal Functions
static GPIOMode_TypeDef stm32_GpioMode(int mode)
{

	switch (mode) {
	case GPIO_M_IN_ANALOG:
		return GPIO_Mode_AIN;
	case GPIO_M_OUT_OD:
	case GPIO_M_OUT_PP:
		return GPIO_Mode_OUT;
	case GPIO_M_AF_OD:
	case GPIO_M_AF_PP:
		return GPIO_Mode_AF;
	default :
		return GPIO_Mode_IN;
	}
}

static GPIOOType_TypeDef stm32_GpioOType(int mode)
{

	switch (mode) {
	case GPIO_M_AF_PP:
	case GPIO_M_OUT_PP:
		return GPIO_OType_PP;
	default :
		return GPIO_OType_OD;
	}
}

static GPIOPuPd_TypeDef stm32_GpioPuPd(int mode)
{
	switch (mode) {
	case GPIO_M_IN_PD:
		return GPIO_PuPd_DOWN;
	case GPIO_M_IN_PU:
		return GPIO_PuPd_UP;
	default :
		return GPIO_PuPd_NOPULL;
	}
}

//External Functions
GPIO_TypeDef *arch_GpioPortBase(int port)
{

	switch (port) {
	case GPIO_P0:
		return GPIOA;
	case GPIO_P1:
		return GPIOB;
	case GPIO_P2:
		return GPIOC;
	case GPIO_P3:
		return GPIOD;
	case GPIO_P4:
		return GPIOE;
	case GPIO_P5:
		return GPIOF;
	case GPIO_P6:
		return GPIOG;
	case GPIO_P7:
		return GPIOH;
	case GPIO_P8:
		return GPIOI;
	default:
		return NULL;
	}
}

void arch_GpioClockEnable(int port)
{

	switch (port) {
	case GPIO_P0:
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		break;
	case GPIO_P1:
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		break;
	case GPIO_P2:
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		break;
	case GPIO_P3:
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		break;
	case GPIO_P4:
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
		break;
	case GPIO_P5:
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
		break;
	case GPIO_P6:
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
		break;
	case GPIO_P7:
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
		break;
	case GPIO_P8:
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);
		break;
	default:
		break;
	}
}

void arch_GpioSel(int port, int pin, int GPIO_AF)
{
	
	GPIO_PinAFConfig(arch_GpioPortBase(port),  pin, GPIO_AF);
}

void arch_GpioConf(int port, int pin, int mode, int init)
{
	GPIO_InitTypeDef xGpio;

	xGpio.GPIO_Pin = BITMASK(pin);
	xGpio.GPIO_Mode = stm32_GpioMode(mode);
	xGpio.GPIO_OType = stm32_GpioOType(mode);
	xGpio.GPIO_PuPd = stm32_GpioPuPd(mode);
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



