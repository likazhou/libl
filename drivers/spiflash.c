#include <drivers/spiflash.h>


//Private Defines
#define SPIF_LOCK_ENABLE		0

#define SPIF_PAGE_SIZE			256

#define SPIF_JEDEC_MX			0x20C2
#define SPIF_JEDEC_SST			0x25BF
#define SPIF_JEDEC_AT			0x271F

#define SPIF_JEDEC_MX25L16		0x1520C2
#define SPIF_JEDEC_MX25L32		0x1620C2
#define SPIF_JEDEC_MX25L64		0x1720C2
#define SPIF_JEDEC_SST25VF16	0x4125BF
#define SPIF_JEDEC_SST25VF32	0x4A25BF
#define SPIF_JEDEC_AT45DB32		0x01271F

#define SPIF_T_NULL				0
#define SPIF_T_PROG_PP			1
#define SPIF_T_PROG_AAI			2


//Command Defines
#define SPIF_CMD_READ_LS		0x03
#define SPIF_CMD_READ_HS		0x0B

#define SPIF_CMD_PAGE_PROG		0x02

#define SPIF_CMD_ERASE_4KB		0x20
#define SPIF_CMD_ERASE_32KB		0x52
#define SPIF_CMD_ERASE_64KB		0xD8

#define SPIF_CMD_WRITE			0xAD

#define SPIF_CMD_READ_STATUS	0x05
#define SPIF_CMD_WRITE_STATUS	0x01
#define SPIF_CMD_WRSR_ENABLE	0x50

#define SPIF_CMD_WRITE_EN		0x06
#define SPIF_CMD_WRITE_DIS		0x04

#define SPIF_CMD_JEDECID		0x9F


//Status Register Bits Defines
#define SPIF_SR_BUSY			0x01
#define SPIF_SR_WRITE_EN		0x02





//Private Typedefs
typedef struct {
	int	size;
	u8	type[BSP_SPIF_QTY];
#if SPI_SEL_ENABLE
	u8	csid[BSP_SPIF_QTY];
#endif
} t_spif_info, *p_spif_info;



//Private Variables
#if SPIF_LOCK_ENABLE
static os_sem_t spif_sem;
#endif
static t_spif_info spif_info;
#if SPI_SEL_ENABLE
static u32 spif_dbgJedec[BSP_SPIF_QTY];
#endif

//Private Macros
#if SPIF_LOCK_ENABLE && OS_TYPE
#define spif_Lock()				os_sem_wait(&spif_sem)
#define spif_Unlock()			os_sem_signal(&spif_sem)
#else
#define spif_Lock()
#define spif_Unlock()
#endif




//Internal Functions
static spi_t *_spif_SpiGet()
{
	spi_t *p;
	
	p = spi_Open(SPIF_COMID, OS_TMO_FOREVER);
	spi_Config(p, SPI_MODE_0, SPI_ORDER_MSB, SPI_SPEED_HIGH);
	return p;
}

static void _spif_WaitIdle(spi_t *p)
{
	size_t nTmo, nSte = SPIF_SR_BUSY, nCmd = SPIF_CMD_READ_STATUS;

	for (nTmo = 50000; (nSte & SPIF_SR_BUSY) && nTmo; nTmo--)
	{
		spi_TransThenRecv(p, &nCmd, 1, &nSte, 1);
	}
}

static void _spif_WriteEnable(spi_t *p)
{
	int nCmd = SPIF_CMD_WRITE_EN;

	spi_Send(p, &nCmd, 1);
}

static void _spif_WriteDisable(spi_t *p)
{
	int nCmd = SPIF_CMD_WRITE_DIS;

	spi_Send(p, &nCmd, 1);
}


static u32 _spif_Probe(spi_t *p)
{
	int nCmd = SPIF_CMD_JEDECID;
	u32 nId = 0;

	_spif_WriteDisable(p);
	
	if (spi_TransThenRecv(p, &nCmd, 1, &nId, 3) == SYS_R_OK)
	{
		nCmd = SPIF_CMD_WRSR_ENABLE;
		spi_Send(p, &nCmd, 1);
		
		nCmd = SPIF_CMD_WRITE_STATUS;
		spi_Send(p, &nCmd, 2);
		
		return nId;
	}
	
	return 0;
}

static sys_res _spif_Read(spi_t *p, adr_t adr, void *pData, size_t nLen)
{
	sys_res res;
	u8 aBuf[5];

	aBuf[0] = SPIF_CMD_READ_HS;
	aBuf[1] = adr >> 16;
	aBuf[2] = adr >> 8;
	aBuf[3] = adr;
	
	spi_TransThenRecv(p, aBuf, sizeof(aBuf), pData, nLen);
	
	return res;
}

static sys_res _spif_Erase(spi_t *p, adr_t adr)
{
	sys_res res;
	u8 aBuf[4];

	_spif_WriteEnable(p);
	
	aBuf[0] = SPIF_CMD_ERASE_4KB;
	aBuf[1] = adr >> 16;
	aBuf[2] = adr >> 8;
	aBuf[3] = adr;
	spi_Send(p, aBuf, sizeof(aBuf));
	
	_spif_WaitIdle(p);
	
	return res;
}

static sys_res _spif_Program_AAI(spi_t *p, adr_t adr, const void *pData, size_t nLen)
{
	u8 aBuf[6], *pBuf, *pEnd;

	_spif_WriteEnable(p);
	
	pBuf = (u8 *)pData;
	pEnd = pBuf + nLen;
	
	aBuf[0] = SPIF_CMD_WRITE;
	aBuf[1] = adr >> 16;
	aBuf[2] = adr >> 8;
	aBuf[3] = adr;
	aBuf[4] = pBuf[0];
	aBuf[5] = pBuf[1];
	spi_Send(p, aBuf, sizeof(aBuf));
	_spif_WaitIdle(p);
	
	pBuf += 2;
	for (; pBuf < pEnd; pBuf += 2)
	{
		aBuf[1] = pBuf[0];
		aBuf[2] = pBuf[1];
		spi_Send(p, aBuf, 3);
		_spif_WaitIdle(p);
	}
	
	_spif_WriteDisable(p);
	
	return SYS_R_OK;
}

static sys_res _spif_Program_PP(spi_t *p, adr_t adr, const void *pData, size_t nLen)
{
	u8 *pBuf, *pPageEnd, *pSecEnd;

	pBuf = (u8 *)pData;
	pSecEnd = pBuf + nLen;
	
	for (; pBuf < pSecEnd; adr += SPIF_PAGE_SIZE)
	{
		_spif_WriteEnable(p);
		
		spi_Start(p);
		
		spi_SendChar(p, SPIF_CMD_PAGE_PROG);
		spi_SendChar(p, adr >> 16);
		spi_SendChar(p, adr >> 8);
		spi_SendChar(p, adr);
		for (pPageEnd = pBuf + 256; pBuf < pPageEnd; )
		{
			spi_SendChar(p, *pBuf++);
		}
		
		spi_End(p);
		
		_spif_WaitIdle(p);
	}
	
	return SYS_R_OK;
}



//External Functions
void spif_Init()
{
	p_spif_info p = &spif_info;
	spi_t *pSpi;
	int i, j;
	u32 jedec;

#if SPIF_LOCK_ENABLE
	os_sem_init(&spif_sem, 1);
#endif
	spif_Lock();

	pSpi = _spif_SpiGet();
	p->size = 0;
#if SPI_SEL_ENABLE
	for (i = 0, j = 0; i < BSP_SPIF_QTY; i++)
	{
		spi_CsSel(pSpi, tbl_bspSpifCsid[i]);
#endif
		jedec = _spif_Probe(pSpi);
		if ((jedec & 0xFFFF) == SPIF_JEDEC_MX)
		{
			switch (jedec)
			{
			case SPIF_JEDEC_MX25L64:
				p->size += 2048;
				break;
			case SPIF_JEDEC_MX25L32:
				p->size += 1024;
				break;
			default:
				p->size += 512;
				break;
			}
#if SPI_SEL_ENABLE
			p->csid[j] = tbl_bspSpifCsid[i];
#endif
			p->type[j] = SPIF_T_PROG_PP;
			j += 1;
		}
		if ((jedec & 0xFFFF) == SPIF_JEDEC_SST)
		{
			switch (jedec)
			{
			case SPIF_JEDEC_SST25VF32:
				p->size += 1024;
				break;
			default:
				p->size += 512;
				break;
			}
#if SPI_SEL_ENABLE
			p->csid[j] = tbl_bspSpifCsid[i];
#endif
			p->type[j] = SPIF_T_PROG_AAI;
			j += 1;
		}
#if OS_TYPE
		os_thd_slp1tick();
#else
		sys_Delay(10000);
#endif
#if SPI_SEL_ENABLE
		spif_dbgJedec[i] = jedec;
	}
#endif
	
	spi_Close(pSpi);
	
	spif_Unlock();
}

void spif_ReadLen(int nSec, int nOffset, void *pData, size_t nLen)
{
	spi_t *pSpi;

	spif_Lock();
	
	pSpi = _spif_SpiGet();

#if SPI_SEL_ENABLE
	spi_CsSel(pSpi, spif_info.csid[nSec / SPIF_SEC_QTY]);
#endif

	_spif_Read(pSpi, (nSec & (SPIF_SEC_QTY - 1)) * SPIF_SEC_SIZE + nOffset, pData, nLen);

	spi_Close(pSpi);
	
	spif_Unlock();
}

void spif_SecErase(int nSec)
{
	spi_t *pSpi;

	spif_Lock();
	
	pSpi = _spif_SpiGet();
	
#if SPI_SEL_ENABLE
	spi_CsSel(pSpi, spif_info.csid[nSec / SPIF_SEC_QTY]);
#endif

	_spif_Erase(pSpi, (nSec & (SPIF_SEC_QTY - 1)) * SPIF_SEC_SIZE);

	spi_Close(pSpi);
	
	spif_Unlock();
}

void spif_Program(int nSec, const void *pData)
{
	p_spif_info p = &spif_info;
	spi_t *pSpi;

	spif_Lock();
	
	pSpi = _spif_SpiGet();
	
#if SPI_SEL_ENABLE
	spi_CsSel(pSpi, p->csid[nSec / SPIF_SEC_QTY]);
#endif

	switch (p->type[nSec / SPIF_SEC_QTY])
	{
	case SPIF_T_PROG_PP:
		_spif_Program_PP(pSpi, (nSec & (SPIF_SEC_QTY - 1)) * SPIF_SEC_SIZE, pData, SPIF_SEC_SIZE);
		break;
	default:
		_spif_Program_AAI(pSpi, (nSec & (SPIF_SEC_QTY - 1)) * SPIF_SEC_SIZE, pData, SPIF_SEC_SIZE);
		break;
	}
	
	spi_Close(pSpi);
	
	spif_Unlock();
}

int spif_GetSize()
{

	return spif_info.size;
}

int dbg_SpifInfo(u8 *pBuf)
{
	p_spif_info p = &spif_info;
	size_t nLen;
#if SPI_SEL_ENABLE
	int i;
#endif

	nLen = sprintf((char *)pBuf, "\r\nSize: %d", p->size * SPIF_SEC_SIZE);
#if SPI_SEL_ENABLE
	for (i = 0; i < BSP_SPIF_QTY; i++)
	{
		nLen += sprintf((char *)(pBuf + nLen), "\r\n%d %d %08X", p->csid[i], p->type[i], spif_dbgJedec[i]);
	}
#endif

	return nLen;
}

