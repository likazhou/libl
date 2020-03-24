
static GpioPinMode_TypeDef str7_GpioFunDef(int nMode)
{

	switch (nMode)
	{
	case GPIO_M_IN_ANALOG:
		return GPIO_HI_AIN_TRI;
	case GPIO_M_IN_PD:
		return GPIO_IPUPD_WP;
	case GPIO_M_IN_PU:
		return GPIO_IPUPD_WP;
	case GPIO_M_OUT_OD:
		return GPIO_OUT_OD;
	case GPIO_M_OUT_PP:
		return GPIO_OUT_PP;
	case GPIO_M_AF_OD:
		return GPIO_AF_OD;
	case GPIO_M_AF_PP:
		return GPIO_AF_PP;
	case GPIO_M_IN_FLOAT:
	default:
		return GPIO_IN_TRI_CMOS;
	}
}

GPIO_TypeDef *str7_GpioPortDef(int nPort)
{

	switch (nPort)
	{
	case GPIO_P0:
		return GPIO0;
	case GPIO_P1:
		return GPIO1;
	case GPIO_P2:
		return GPIO2;
	default:
		return NULL;
	}
}

void arch_GpioConf(p_gpio_def p)
{

	if (p->mode & GPIO_M_OUT_MASK)
	{
		switch (p->init)
		{
		case GPIO_INIT_HIGH:
			arch_GpioSet(p, 1);
			break;
		case GPIO_INIT_LOW:
			arch_GpioSet(p, 0);
			break;
		default:
			break;
		}
	}
	GPIO_Config(str7_GpioPortDef(p->port), BITMASK(p->pin), str7_GpioFunDef(p->mode));
}

void str7_GpioIdleInit()
{
/*	GPIO_InitTypeDef xGpio;

	//将所有没有使用的IO置为模拟输入
	//可以降低功耗和改善EMC/EMI性能
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
						   	RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
							RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF |
							RCC_APB2Periph_GPIOG, ENABLE);
	xGpio.GPIO_Pin = GPIO_Pin_All;
	xGpio.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &xGpio);
	GPIO_Init(GPIOB, &xGpio);
	GPIO_Init(GPIOC, &xGpio);
	GPIO_Init(GPIOD, &xGpio);	
	GPIO_Init(GPIOE, &xGpio);
	GPIO_Init(GPIOF, &xGpio);
	GPIO_Init(GPIOG, &xGpio);
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
						   	RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
							RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF |
							RCC_APB2Periph_GPIOG, DISABLE);
*/
}

void arch_GpioSet(p_gpio_def p, uint nHL)
{

	if (nHL)
		SETBIT(str7_GpioPortDef(p->port)->PD, p->pin);
	else
		CLRBIT(str7_GpioPortDef(p->port)->PD, p->pin);
}

int arch_GpioRead(p_gpio_def p)
{
	int nR;

	if ((p->mode & GPIO_M_OUT_MASK) == 0)
		return GPIO_BitRead(str7_GpioPortDef(p->port), p->pin);
	
	GPIO_Config(str7_GpioPortDef(p->port), BITMASK(p->pin), GPIO_IN_TRI_CMOS);
	nR = GPIO_BitRead(str7_GpioPortDef(p->port), p->pin);
	GPIO_Config(str7_GpioPortDef(p->port), BITMASK(p->pin), str7_GpioFunDef(p->mode));
	
	return nR;
}


