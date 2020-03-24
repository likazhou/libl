#include <drivers/spibus.h>



//Private Definds
#define SPIBUS_LOCK_ENABLE		0


//Private Macros
#if SPIBUS_LOCK_ENABLE && OS_TYPE
#define spibus_Lock()			os_thd_lock()
#define spibus_Unlock()			os_thd_unlock()
#else
#define spibus_Lock()
#define spibus_Unlock()
#endif




static void spibus_Delay(spi_t *p)
{

	if (p->speed)
		sys_Delay(MCU_CLOCK / p->speed / 2);
}

static void spibus_Sck(spi_t *p, int nOut)
{
	t_gpio_def *pIO = tbl_bspSpiDef[p->parent.id];

	if (p->sckmode == SPI_SCKIDLE_HIGH)
		nOut = nOut ? 0 : 1;

	arch_GpioSet(pIO->port, pIO->pin, nOut);
	spibus_Delay(p);
}

static void spibus_Mosi(spi_t *p, int nHL)
{
	t_gpio_def *pIO = tbl_bspSpiDef[p->parent.id] + 1;

	arch_GpioSet(pIO->port, pIO->pin, nHL);
}

static int spibus_Miso(spi_t *p)
{
	t_gpio_def *pIO = tbl_bspSpiDef[p->parent.id] + 2;

	return arch_GpioRead(pIO->port, pIO->pin);
}

static void spibus_CeLow(spi_t *p)
{
	t_gpio_def *pIO = tbl_bspSpiDef[p->parent.id] + 3;

	arch_GpioSet(pIO->port, pIO->pin, 0);
}

static void spibus_CeHigh(spi_t *p)
{
	t_gpio_def *pIO = tbl_bspSpiDef[p->parent.id] + 3;

	arch_GpioSet(pIO->port, pIO->pin, 1);
}


static void _spibus_Recv(spi_t *p, u8 *pRec, size_t nLen)
{
	int i, nData;

	for (; nLen; nLen--)
	{
		nData = 0;
		for (i = 0; i < 8; i++)
		{
			nData <<= 1;
			if (p->latchmode == SPI_LATCH_1EDGE)
			{
				if (spibus_Miso(p))
					SETBIT(nData, 0);
			}
			
			spibus_Sck(p, 1);
			
			if (p->latchmode == SPI_LATCH_2EDGE)
			{
				if (spibus_Miso(p))
					SETBIT(nData, 0);
			}
			
			spibus_Sck(p, 0);
		}
		*pRec++ = nData;
	}
}










//-------------------------------------------------------------------
//External Functions
//-------------------------------------------------------------------

sys_res spibus_Init(spi_t *p)
{
	tbl_gpio_def pSck = tbl_bspSpiDef[p->parent.id];

	sys_GpioConf(pSck);
	sys_GpioConf(pSck + 1);
	sys_GpioConf(pSck + 2);
#if SPI_CE_ENABLE
	sys_GpioConf(pSck + 3);
#endif
	return SYS_R_OK;
}

sys_res spibus_Config(spi_t *p)
{

	spibus_Sck(p, 0);
	return SYS_R_OK;
}


void spibus_Start(spi_t *p)
{

	spibus_Lock();
#if SPI_CE_ENABLE
	spibus_CeLow(p);
#endif
}

void spibus_SendChar(spi_t *p, u8 nData)
{
	int i;

	for (i = 0; i < 8; i++)
	{
		if (p->latchmode == SPI_LATCH_1EDGE)
			spibus_Mosi(p, nData & 0x80);
		
		spibus_Sck(p, 1);
		
		if (p->latchmode == SPI_LATCH_2EDGE)
			spibus_Mosi(p, nData & 0x80);
		
		spibus_Sck(p, 0);

		nData <<= 1;
	}
}

void spibus_End(spi_t *p)
{

#if SPI_CE_ENABLE
	spibus_CeHigh(p);
#endif
	spibus_Unlock();
}


sys_res spibus_Send(spi_t *p, const void *pData, size_t nLen)
{
	u8 *pBuf = (u8 *)pData;

	spibus_Start(p);
	
	for (; nLen; nLen--)
	{
		spibus_SendChar(p, *pBuf++);
	}
	
	spibus_End(p);
	
	return SYS_R_OK;
}

sys_res spibus_Recv(spi_t *p, void *pRec, size_t nLen)
{

	spibus_Start(p);
	
	_spibus_Recv(p, pRec, nLen);
	
	spibus_End(p);
	return SYS_R_OK;
	
}

sys_res spibus_Transce(spi_t *p, const void *pCmd, size_t nCmdLen, void *pRec, size_t nRecLen)
{
	u8 *pBuf = (u8 *)pCmd;

	spibus_Start(p);
	
	for (; nCmdLen; nCmdLen--)
	{
		spibus_SendChar(p, *pBuf++);
	}
	
	_spibus_Recv(p, pRec, nRecLen);
	
	spibus_End(p);
	
	return SYS_R_OK;
}

sys_res spibus_TransChar(spi_t *p, u8 nSend, void *pRec)
{
	u8 *pBuf = (u8 *)pRec;
	int i, nData = 0;

	spibus_Start(p);
	
	for (i = 0; i < 8; i++)
	{
		nData <<= 1;
		if (p->latchmode == SPI_LATCH_1EDGE)
		{
			spibus_Mosi(p, nSend & 0x80);
			if (spibus_Miso(p))
				SETBIT(nData, 0);
		}
		
		spibus_Sck(p, 1);
		
		if (p->latchmode == SPI_LATCH_2EDGE)
		{
			spibus_Mosi(p, nSend & 0x80);
			if (spibus_Miso(p))
				SETBIT(nData, 0);
		}
		
		spibus_Sck(p, 0);

		nSend <<= 1;
	}
	*pBuf = nData;
	
	spibus_End(p);
	
	return SYS_R_OK;
}




