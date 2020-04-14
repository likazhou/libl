#if SPI_ENABLE && SPI_SOFTWARE == 0

//Private Variables
static SPI_TypeDef * const stm32_tblSpiId[] = {SPI1, SPI2, SPI3};



//Private Functions
static void stm32_SpiApbClockCmd(SPI_TypeDef *pSpi, FunctionalState ste)
{

	switch ((u32)pSpi) {
	case SPI1_BASE:
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ste);
		break;
	case SPI2_BASE:
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ste);
		break;
	case SPI3_BASE:
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ste);
		break;
	default:
		break;
	}
}





//External Functions
void arch_SpiInit(spi_t *p)
{
	t_spi_def *pDef = &tbl_bspSpiDef[p->parent.id];
	SPI_TypeDef *pSpi = stm32_tbl_spiid[pDef->id];
	GPIO_InitTypeDef xGpio;

	//Enable SPI Clock
	stm32_SpiApbClockCmd(pSpi, ENABLE);

	xGpio.GPIO_Speed = GPIO_Speed_50MHz;
	xGpio.GPIO_Mode = GPIO_Mode_AF;
	if (pDef->outmode == DEV_PIN_OD)
		xGpio.GPIO_OType = GPIO_OType_OD;
	else
		xGpio.GPIO_OType = GPIO_OType_PP;
	
	//SCK
	xGpio.GPIO_Pin = BITMASK(pDef->sckpin);
	arch_GpioClockEnable(pDef->sckport);
	GPIO_SetBits(arch_GpioPortBase(pDef->sckport), BITMASK(pDef->sckpin));
	GPIO_Init(arch_GpioPortBase(pDef->sckport), &xGpio);
	//MOSI
	xGpio.GPIO_Pin = BITMASK(pDef->mosipin);
	arch_GpioClockEnable(pDef->mosiport);
	GPIO_SetBits(arch_GpioPortBase(pDef->mosiport), BITMASK(pDef->mosipin));
	GPIO_Init(arch_GpioPortBase(pDef->mosiport), &xGpio);
	//MISO
	xGpio.GPIO_Pin = BITMASK(pDef->misopin);
	arch_GpioClockEnable(pDef->misoport);
	GPIO_SetBits(arch_GpioPortBase(pDef->misoport), BITMASK(pDef->misopin));
	GPIO_Init(arch_GpioPortBase(pDef->misoport), &xGpio);

	GPIO_PinAFConfig(arch_GpioPortBase(pDef->sckport), pDef->sckpin, GPIO_AF_SPI2);
	GPIO_PinAFConfig(arch_GpioPortBase(pDef->mosiport), pDef->mosipin, GPIO_AF_SPI2);
	GPIO_PinAFConfig(arch_GpioPortBase(pDef->misoport), pDef->misopin, GPIO_AF_SPI2);
	
}

sys_res arch_SpiConfig(spi_t *p, int mode, int latch, int speed)
{
	SPI_TypeDef *pSpi = stm32_tbl_spiid[p->def->id];
 	SPI_InitTypeDef xSpi;

	xSpi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	xSpi.SPI_Mode = SPI_Mode_Master;
	xSpi.SPI_DataSize = SPI_DataSize_8b;
	
	if (mode & 1)
		xSpi.SPI_CPOL = SPI_CPOL_High;
	else
		xSpi.SPI_CPOL = SPI_CPOL_Low;
	
	if (mode & 2)
		xSpi.SPI_CPHA = SPI_CPHA_2Edge;
	else
		xSpi.SPI_CPHA = SPI_CPHA_1Edge;

	switch(speed)
	{
	case SPI_SPEED_HIGH:
		xSpi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
		break;
	case SPI_SPEED_NORMAL:
		xSpi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
		break;
	case SPI_SPEED_LOW:
		xSpi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
		break;
	default:
		xSpi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
		break;
	}
	
	xSpi.SPI_NSS = SPI_NSS_Soft;
	xSpi.SPI_FirstBit = SPI_FirstBit_MSB;
	xSpi.SPI_CRCPolynomial = 7;	

	SPI_Init(pSpi, &xSpi);
	SPI_Cmd(pSpi, ENABLE);
	
	return SYS_R_OK;
}

sys_res arch_SpiSend(spi_t *p, const void *send, size_t len)
{
	SPI_TypeDef *pSpi = stm32_tbl_spiid[p->def->id];
	u8 *pData = (u8 *)p;

	for (; len; len--)
	{
		while (SPI_I2S_GetFlagStatus(pSpi, SPI_I2S_FLAG_TXE) == RESET);
		pSpi->DR = *pData++;
	}
	
	return SYS_R_OK;
}

sys_res arch_SpiRecv(spi_t *p, u8 *rec, size_t len)
{
	SPI_TypeDef *pSpi = stm32_tbl_spiid[p->def->id];

	for (; len; len--)
	{
		while (SPI_I2S_GetFlagStatus(pSpi, SPI_I2S_FLAG_RXNE) == RESET);
		*rec++ = pSpi->DR;
	}
	
	return SYS_R_OK;
}

sys_res arch_SpiTransce(spi_t *p, const void *send, size_t slen, u8 *rec, size_t rlen)
{
	SPI_TypeDef *pSpi = stm32_tbl_spiid[tbl_bspSpiDef[p->parent.id].chipid];
	u8 *tp = (u8 *)send;

	for (; slen; slen--)
	{
		while (SPI_I2S_GetFlagStatus(pSpi, SPI_I2S_FLAG_TXE) == RESET);
		pSpi->DR = *tp++;
	}
	
	for (; rlen; rlen--)
	{
		while (SPI_I2S_GetFlagStatus(pSpi, SPI_I2S_FLAG_RXNE) == RESET);
		*rec++ = pSpi->DR;
	}
	
	return SYS_R_OK;
}


#endif



