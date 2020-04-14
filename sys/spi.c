

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
spi_t *spi_Open(int id, size_t tmo)
{
	spi_t *p = &dev_Spi[id];

#if OS_TYPE
	if (dev_Open(&p->parent, tmo) != SYS_R_OK)
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
sys_res spi_Config(spi_t *p, int mode, int speed)
{

#if SPI_SOFTWARE
	p->sckmode = mode & 1 ? 1 : 0;
	p->latchmode = mode & 1 ? 1 : 0;
	p->speed = speed;
	spibus_Config(p);
#else
	arch_SpiConfig(p, mode, speed);
#endif
	return SYS_R_OK;
}

#if SPI_SEL_ENABLE
void spi_CsSel(spi_t *p, int id)
{

	if (p->csid != id)
	{
		p->csid = id;
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
#endif

	return SYS_R_OK;
}

sys_res spi_SendChar(spi_t *p, u8 data)
{

#if SPI_SOFTWARE
	spibus_SendChar(p, data);
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
sys_res spi_Send(spi_t *p, const void *send, size_t len)
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
	res = spibus_Send(p, send, len);
#else
	res = arch_SpiSend(p, send, len);
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
sys_res spi_Recv(spi_t *p, void *rec, size_t len)
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
	res = spibus_Recv(p, rec, len);
#else
	res = arch_SpiRecv(p, rec, len);
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
	res = spibus_TransThenRecv(p, pCmd, nCmdLen, pRec, nRecLen);
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


