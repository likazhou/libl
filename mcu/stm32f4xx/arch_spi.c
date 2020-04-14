#if SPI_ENABLE && SPI_SOFTWARE == 0



//Private Variables
static SPI_TypeDef * const stm32_tbl_spiid[] = 
{
	SPI1,
	SPI2,
	SPI3,
#if defined(STM32F427xx) || defined(STM32F429xx)
	SPI4,
	SPI5,
	SPI6,
#endif
};

static u8 spiid_mcu2dev[] =
{
	0xFF,
	0xFF,
	0xFF,
#if defined(STM32F427xx) || defined(STM32F429xx)
	0xFF,
	0xFF,
	0xFF,
#endif
};


//Private Functions




//External Functions
void arch_SpiInit(spi_t *p)
{
	t_spi_def *def = p->def;
	int nirq;
	LL_GPIO_InitTypeDef gpioinit;

	spiid_mcu2dev[def->id] = p - &dev_Spi[0];

	switch (def->id)
	{
	case 0:
		/* Enalbe the SPI1  Clock*/
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
		/* Enable the SPI1 Interrupt */
		nirq = SPI1_IRQn;
		gpioinit.Alternate = LL_GPIO_AF_5;
		break;
		
	case 1:
		/* Enalbe the SPI2  Clock*/
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
		/* Enable the SPI2 Interrupt */
		nirq = SPI2_IRQn;
		gpioinit.Alternate = LL_GPIO_AF_5;
		break;
		
	case 2:
		/* Enalbe the SPI3  Clock*/
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI3);
		/* Enable the SPI3 Interrupt */
		nirq = SPI3_IRQn;
		gpioinit.Alternate = LL_GPIO_AF_6;
		break;
		
#if defined(STM32F427xx) || defined(STM32F429xx)
	case 3:
		/* Enalbe the SPI4  Clock*/
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI4);
		/* Enable the SPI4 Interrupt */
		nirq = SPI4_IRQn;
		gpioinit.Alternate = LL_GPIO_AF_5;
		break;
		
	case 4:
		/* Enalbe the SPI5  Clock*/
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI5);
		/* Enable the SPI5 Interrupt */
		nirq = SPI5_IRQn;
		gpioinit.Alternate = LL_GPIO_AF_5;
		break;
		
	case 5:
		/* Enalbe the SPI6  Clock*/
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI6);
		/* Enable the SPI6 Interrupt */
		nirq = SPI6_IRQn;
		gpioinit.Alternate = LL_GPIO_AF_5;
		break;
#endif

	default:
		return;
	}
	arch_IrqPri(nirq, 0);

	gpioinit.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	gpioinit.Pull = LL_GPIO_PULL_UP;
	gpioinit.Mode = LL_GPIO_MODE_ALTERNATE;
	gpioinit.OutputType = def->outmode == DEV_PIN_OD ? LL_GPIO_OUTPUT_OPENDRAIN : LL_GPIO_OUTPUT_PUSHPULL;

	//SCK
	gpioinit.Pin = BITMASK(def->sckpin);
	arch_GpioClockEnable(def->sckport);
	LL_GPIO_Init(arch_GpioPortBase(def->sckport), &gpioinit);
	
	//MOSI
	gpioinit.Pin = BITMASK(def->mosipin);
	arch_GpioClockEnable(def->mosiport);
	LL_GPIO_Init(arch_GpioPortBase(def->mosiport), &gpioinit);
	
	//MISO
	gpioinit.Pin = BITMASK(def->misopin);
	arch_GpioClockEnable(def->misoport);
	LL_GPIO_Init(arch_GpioPortBase(def->misoport), &gpioinit);
	
	//NSS
	if (def->nssvalid)
	{
		gpioinit.Pin = BITMASK(def->nsspin);
		arch_GpioClockEnable(def->nssport);
		LL_GPIO_Init(arch_GpioPortBase(def->nssport), &gpioinit);
	}
}

sys_res arch_SpiConfig(spi_t *p, int mode, int speed)
{
	SPI_TypeDef *spi;
	LL_SPI_InitTypeDef spiinit;

	spi = stm32_tbl_spiid[p->def->id];

	spiinit.TransferDirection = LL_SPI_FULL_DUPLEX;
	spiinit.Mode = LL_SPI_MODE_MASTER;
	spiinit.DataWidth = LL_SPI_DATAWIDTH_8BIT;
	
	spiinit.ClockPolarity = mode & 1 ? LL_SPI_POLARITY_HIGH : LL_SPI_POLARITY_LOW;
	spiinit.ClockPhase = mode & 2 ? LL_SPI_PHASE_2EDGE : LL_SPI_PHASE_1EDGE;
	switch(speed)
	{
	case SPI_SPEED_HIGH:
		spiinit.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV8;
		break;
	case SPI_SPEED_NORMAL:
		spiinit.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV16;
		break;
	case SPI_SPEED_LOW:
		spiinit.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV64;
		break;
	default:
		spiinit.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
		break;
	}
	spiinit.NSS = p->def->nssvalid ? LL_SPI_NSS_HARD_OUTPUT : LL_SPI_NSS_SOFT;
	spiinit.BitOrder = LL_SPI_MSB_FIRST;
	spiinit.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
	spiinit.CRCPoly = 10;
	LL_SPI_DeInit(spi);
	LL_SPI_Init(spi, &spiinit);

//	LL_SPI_EnableIT_RXNE(spi);

	LL_SPI_Enable(spi);
	
	return SYS_R_OK;
}

sys_res arch_SpiSend(spi_t *p, const void *send, size_t len)
{
	SPI_TypeDef *spi = stm32_tbl_spiid[p->def->id];
	u8 *data = (u8 *)send;
	
	for (; len; len--)
	{
		while (LL_SPI_IsActiveFlag_TXE(spi) == 0);
		spi->DR = *data++;
	}

	LL_SPI_ClearFlag_OVR(spi);
	
	return SYS_R_OK;
}

sys_res arch_SpiRecv(spi_t *p, u8 *rec, size_t len)
{
	SPI_TypeDef *spi = stm32_tbl_spiid[p->def->id];

	for (; len; len--)
	{
		while (LL_SPI_IsActiveFlag_RXNE(spi) == 0);
		*rec++ = spi->DR;
	}
	
	return SYS_R_OK;
}

sys_res arch_SpiTransce(spi_t *p, const void *send, u8 *rec, size_t len)
{
	SPI_TypeDef *spi = stm32_tbl_spiid[p->def->id];
	u8 *tp = (u8 *)&send;

	for (; len; len--)
	{
		while (LL_SPI_IsActiveFlag_TXE(spi) == 0);
		spi->DR = *tp++;

		while (LL_SPI_IsActiveFlag_RXNE(spi) == 0);
		*rec++ = spi->DR;
	}

	LL_SPI_ClearFlag_OVR(spi);
	
	return SYS_R_OK;
}


#endif




