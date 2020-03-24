

//Public Variables
spi_t dev_Spi[BSP_SPI_QTY];




//-------------------------------------------------------------------------
//Function Name  : 
//Description    : 
//Input          : - GPIOx:
//                - GPIO_InitStruct: 
//Output         : None
//Return         : None
//-------------------------------------------------------------------------
spi_t *spi_Open(int nId, size_t nTmo)
{
	spi_t *p = &dev_Spi[nId];

#if OS_TYPE
	if (dev_Open(&p->parent, nTmo) != SYS_R_OK)
		return NULL;
#endif

	return p;
}

//-------------------------------------------------------------------------
//Function Name  : 
//Description    : 
//Input          : - GPIOx:
//                - GPIO_InitStruct: 
//Output         : None
//Return         : None
//-------------------------------------------------------------------------
sys_res spi_Close(spi_t *p)
{

#if OS_TYPE

	return dev_Close(&p->parent);
#else

	return SYS_R_OK;
#endif
}

//-------------------------------------------------------------------------
//Function Name  : 
//Description    : 
//Input          : - GPIOx:
//                - GPIO_InitStruct: 
//Output         : None
//Return         : None
//-------------------------------------------------------------------------
sys_res spi_Config(spi_t *p, int nSckMode, int nLatch, int nSpeed)
{

	p->sckmode = nSckMode;
	p->latchmode = nLatch;
	p->speed = nSpeed;
#if SPI_SOFTWARE
	spibus_Config(p);
#else
	arch_SpiConfig(p);
#endif
	return SYS_R_OK;
}

#if SPI_SEL_ENABLE
void spi_CsSel(spi_t *p, int nId)
{

	if (p->csid != nId)
	{
		p->csid = nId;
#if HC138_ENABLE
		hc138_Set(p->csid);
#endif
	}
}
#endif


sys_res spi_Start(spi_t *p)
{

#if OS_TYPE
	while (p->ste != SPI_S_IDLE)
	{
		os_thd_slp1tick();
	}
#endif
	p->ste = SPI_S_BUSY;
#if SPI_SOFTWARE
	spibus_Start(p);
#else
#endif

	return SYS_R_OK;
}

sys_res spi_SendChar(spi_t *p, u8 nData)
{

#if SPI_SOFTWARE
	spibus_SendChar(p, nData);
#else
#endif

	return SYS_R_OK;
}

sys_res spi_End(spi_t *p)
{

	if (p->ste == SPI_S_BUSY)
	{
		p->ste = SPI_S_IDLE;
#if SPI_SOFTWARE
		spibus_End(p);
#endif

		return SYS_R_OK;
	}

	return SYS_R_ERR;
}



//-------------------------------------------------------------------------
//Function Name  : 
//Description    : 
//Input          : - GPIOx:
//                - GPIO_InitStruct: 
//Output         : None
//Return         : None
//-------------------------------------------------------------------------
sys_res spi_Send(spi_t *p, const void *pData, size_t nLen)
{
	sys_res res;

#if OS_TYPE
	while (p->ste != SPI_S_IDLE)
	{
		os_thd_slp1tick();
	}
#endif
	p->ste = SPI_S_BUSY;
#if SPI_SOFTWARE
	res = spibus_Send(p, pData, nLen);
#else
	res = arch_SpiSend(p, pData, nLen);
#endif
	p->ste = SPI_S_IDLE;

	return res;
}

//-------------------------------------------------------------------------
//Function Name  : 
//Description    : 
//Input          : - GPIOx:
//                - GPIO_InitStruct: 
//Output         : None
//Return         : None
//-------------------------------------------------------------------------
sys_res spi_Recv(spi_t *p, void *pRec, size_t nLen)
{
 	sys_res res;

#if OS_TYPE
	while (p->ste != SPI_S_IDLE)
	{
		os_thd_slp1tick();
	}
#endif
	p->ste = SPI_S_BUSY;
#if SPI_SOFTWARE
	res = spibus_Recv(p, pRec, nLen);
#else
	res = arch_SpiRecv(p, pRec, nLen);
#endif
	p->ste = SPI_S_IDLE;

	return res;
}

//-------------------------------------------------------------------------
//Function Name  : 
//Description    : 
//Input          : - GPIOx:
//                - GPIO_InitStruct: 
//Output         : None
//Return         : None
//-------------------------------------------------------------------------
sys_res spi_Transce(spi_t *p, const void *pCmd, size_t nCmdLen, void *pRec, size_t nRecLen)
{
 	sys_res res;

#if OS_TYPE
	while (p->ste != SPI_S_IDLE)
	{
		os_thd_slp1tick();
	}
#endif
	p->ste = SPI_S_BUSY;
#if SPI_SOFTWARE
	res = spibus_Transce(p, pCmd, nCmdLen, pRec, nRecLen);
#else
	res = arch_SpiTransce(p, pCmd, nCmdLen, pRec, nRecLen);
#endif
	p->ste = SPI_S_IDLE;

	return res;
}

//-------------------------------------------------------------------------
//Function Name  : 
//Description    : 
//Input          : - GPIOx:
//                - GPIO_InitStruct: 
//Output         : None
//Return         : None
//-------------------------------------------------------------------------
sys_res spi_TransChar(spi_t *p, u8 nSend, void *pRec)
{
 	sys_res res;

#if OS_TYPE
	while (p->ste != SPI_S_IDLE)
	{
		os_thd_slp1tick();
	}
#endif
	p->ste = SPI_S_BUSY;
#if SPI_SOFTWARE
	res = spibus_TransChar(p, nSend, pRec);
#else
#endif
	p->ste = SPI_S_IDLE;

	return res;
}


