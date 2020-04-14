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

static void spibus_Sck(spi_t *p, int out)
{
	t_gpio_def *pIO = tbl_bspSpiDef[p->parent.id];

	if (p->sckmode)
		out = out ? 0 : 1;

	arch_GpioSet(pIO->port, pIO->pin, out);
	spibus_Delay(p);
}

static void spibus_Mosi(spi_t *p, int HL)
{
	t_gpio_def *pIO = tbl_bspSpiDef[p->parent.id] + 1;

	arch_GpioSet(pIO->port, pIO->pin, HL);
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


static void _spibus_Recv(spi_t *p, u8 *rec, size_t len)
{
	int i, nData;

	for (; len; len--)
	{
		nData = 0;
		for (i = 0; i < 8; i++)
		{
			nData <<= 1;
			if (p->latchmode == 0)
			{
				if (spibus_Miso(p))
					SETBIT(nData, 0);
			}
			
			spibus_Sck(p, 1);
			
			if (p->latchmode)
			{
				if (spibus_Miso(p))
					SETBIT(nData, 0);
			}
			
			spibus_Sck(p, 0);
		}
		*rec++ = nData;
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

void spibus_SendChar(spi_t *p, u8 data)
{
	int i;

	for (i = 0; i < 8; i++)
	{
		if (p->latchmode == 0)
			spibus_Mosi(p, data & 0x80);
		
		spibus_Sck(p, 1);
		
		if (p->latchmode)
			spibus_Mosi(p, data & 0x80);
		
		spibus_Sck(p, 0);

		data <<= 1;
	}
}

void spibus_End(spi_t *p)
{

#if SPI_CE_ENABLE
	spibus_CeHigh(p);
#endif
	spibus_Unlock();
}


sys_res spibus_Send(spi_t *p, const void *send, size_t len)
{
	u8 *data = (u8 *)send;

	spibus_Start(p);
	
	for (; len; len--)
	{
		spibus_SendChar(p, *data++);
	}
	
	spibus_End(p);
	
	return SYS_R_OK;
}

sys_res spibus_Recv(spi_t *p, void *rec, size_t len)
{

	spibus_Start(p);
	
	_spibus_Recv(p, rec, len);
	
	spibus_End(p);
	return SYS_R_OK;
	
}

sys_res spibus_TransThenRecv(spi_t *p, const void *send, size_t slen, void *rec, size_t rlen)
{
	const u8 *data = (const u8 *)send;

	spibus_Start(p);
	
	for (; slen; slen--)
	{
		spibus_SendChar(p, *data++);
	}
	
	_spibus_Recv(p, rec, rlen);
	
	spibus_End(p);
	
	return SYS_R_OK;
}

sys_res spibus_Transmit(spi_t *p, const void *send, void *rec, size_t len)
{
	const u8 *psend = (const u8 *)send;
	u8 *prec = (u8 *)rec;
	int i, sd, rd;

	spibus_Start(p);

	for (; len; len--)
	{
		sd = *psend++;
		rd = 0;
		for (i = 0; i < 8; i++)
		{
			rd <<= 1;
			if (p->latchmode == 0)
			{
				spibus_Mosi(p, sd & 0x80);
				if (spibus_Miso(p))
					SETBIT(rd, 0);
			}
			
			spibus_Sck(p, 1);
			
			if (p->latchmode)
			{
				spibus_Mosi(p, sd & 0x80);
				if (spibus_Miso(p))
					SETBIT(rd, 0);
			}
			
			spibus_Sck(p, 0);

			sd <<= 1;
		}
		*prec++ = rd;
	}
	
	spibus_End(p);
	
	return SYS_R_OK;
}




