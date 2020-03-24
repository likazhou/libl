
#if 0
//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void spi_Initialize(BSPI_TypeDef *pBSPI)
{

	BSPI_BSPI0Conf(ENABLE);
	BSPI_Init(pBSPI);
	if (pBSPI == BSPI0)
	{
		GPIO_Config(GPIO0, BITMASK(0) | BITMASK(1) | BITMASK(2), GPIO_AF_PP);
		BSPI_ClockDividerConfig(pBSPI, 24);
	}
	else
	{
		GPIO_Config(GPIO0, BITMASK(4) | BITMASK(5) | BITMASK(6), GPIO_AF_PP);
		BSPI_ClockDividerConfig(pBSPI, 8);
	}
	BSPI_Enable(pBSPI, ENABLE);
	BSPI_MasterEnable(pBSPI, ENABLE);
	BSPI_ClkActiveHigh(pBSPI, DISABLE);
	BSPI_ClkFEdge(pBSPI, DISABLE);
	BSPI_8bLEn(pBSPI, ENABLE);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
static int _spi_Transmit(BSPI_TypeDef *pBSPI, u32 nData)
{
	int nResult;
	u32 i = spi_ErrorThreshold;

	while (pBSPI->CSR2 & BSPI_RFNE)
	{
		nResult = pBSPI->RXR;
		SPI_ERR_RETURN(i, -1);
	}
	while (!(pBSPI->CSR2 & BSPI_TFE))
	{
		SPI_ERR_RETURN(i, -1);
	}
	pBSPI->TXR = nData << 8;
	while (!(pBSPI->CSR2 & BSPI_RFF))
	{
		SPI_ERR_RETURN(i, -1);
	}
	nResult = pBSPI->RXR >> 8;
	return nResult;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
int spi_Transmit(BSPI_TypeDef *pBSPI, u32 nData)
{
	int nResult;

	if ((nResult = _spi_Transmit(pBSPI, nData)) == (-1))
	{
		spi_Initialize(pBSPI);
	}
	return nResult;
}
#endif

