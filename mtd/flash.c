

#if INTFLASH_ENABLE
#include <drivers/intflash.c>
#endif
#if NANDFLASH_ENABLE
#include <drivers/nandflash.c>
#endif
#if NORFLASH_ENABLE
#include <drivers/norflash.c>
#endif
#if SPIFLASH_ENABLE
#include <drivers/spiflash.c>
#endif



size_t flash_BlkSize(int nDev)
{

	switch (nDev)
	{
#if INTFLASH_ENABLE
	case FLASH_DEV_INT:
		return INTFLASH_BLK_SIZE;
#endif
#if NORFLASH_ENABLE
	case FLASH_DEV_EXTNOR:
		return NORFLASH_BLK_SIZE;
#endif
	default:
		return 0;
	}
}

sys_res flash_NolockErase(int nDev, adr_t nAdr)
{
	sys_res res;

	switch (nDev)
	{
#if INTFLASH_ENABLE
	case FLASH_DEV_INT:
		res = intf_nolockErase(nAdr);
		break;
#endif
#if NORFLASH_ENABLE
	case FLASH_DEV_EXTNOR:
		res = norf_nolockErase(nAdr);
		break;
#endif
	default:
		res = SYS_R_EMPTY;
		break;
	}
	return res;
}

sys_res flash_NolockProgram(int nDev, adr_t nAdr, const void *pData, size_t nLen)
{
	sys_res res;

	switch (nDev)
	{
#if INTFLASH_ENABLE
	case FLASH_DEV_INT:
		res = intf_nolockProgram(nAdr, pData, nLen);
		break;
#endif
#if NORFLASH_ENABLE
	case FLASH_DEV_EXTNOR:
		res = norf_nolockProgram(nAdr, pData, nLen);
		break;
#endif
	default:
		res = SYS_R_EMPTY;
		break;
	}
	return res;
}


#if FLASH_ENABLE

sys_res flash_Erase(int nDev, adr_t nAdr)
{
	sys_res res;

	switch (nDev)
	{
#if INTFLASH_ENABLE
	case FLASH_DEV_INT:
		res = intf_Erase(nAdr);
		break;
#endif
#if NORFLASH_ENABLE
	case FLASH_DEV_EXTNOR:
		res = norf_Erase(nAdr);
		break;
#endif
	default:
		res = SYS_R_EMPTY;
		break;
	}
	return res;
}

sys_res flash_Program(int nDev, adr_t nAdr, const void *pData, size_t nLen)
{
	sys_res res;

	switch (nDev)
	{
#if INTFLASH_ENABLE
	case FLASH_DEV_INT:
		res = intf_Program(nAdr, pData, nLen);
		break;
#endif
#if NORFLASH_ENABLE
	case FLASH_DEV_EXTNOR:
		res = norf_Program(nAdr, pData, nLen);
		break;
#endif
	default:
		res = SYS_R_EMPTY;
		break;
	}
	return res;
}




//Private Defines
#define FLASH_LOCK_ENABLE		1

#define FLASH_BLOCK_INVALID		(-1)


//Private Typedefs
struct flash_buffer
{
	int		type;
	int		sec;
	time_t	dirty;
	u8		fbuf[SPIF_SEC_SIZE];
};
typedef struct flash_buffer flash_buf_t;


//Private Variables
#if FLASH_LOCK_ENABLE
static os_sem_t flash_sem;
#endif
static flash_buf_t flash_buf;



//Private Macros
#if FLASH_LOCK_ENABLE && OS_TYPE
#define flash_Lock()				os_sem_wait(&flash_sem)
#define flash_Unlock()				os_sem_signal(&flash_sem)
#else
#define flash_Lock()
#define flash_Unlock()
#endif


//Internal Functions
static void _flash_Flush(int nDelay)
{
	flash_buf_t *p = &flash_buf;
	adr_t nAdr;
	time_t tTime;

	if (p->sec == FLASH_BLOCK_INVALID)
		return;
	
	if (p->dirty == 0)
		return;
	
	tTime = rtc_GetTimet();
	if (p->dirty > tTime)
	{
		nDelay = 0;
	}
	else
	{
		if ((tTime - p->dirty) >= nDelay)
			nDelay = 0;
	}
	
	if (nDelay == 0)
	{
		switch (p->type)
		{
#if INTFLASH_ENABLE
		case FLASH_DEV_INT:
			nAdr = p->sec * INTFLASH_BLK_SIZE;
			if ((nAdr >= INTFLASH_BASE_ADR) && (nAdr < (INTFLASH_BASE_ADR + INTFLASH_SIZE)))
			{
				intf_Erase(nAdr);
				intf_Program(nAdr, p->fbuf, INTFLASH_BLK_SIZE);
			}
			break;
#endif

#if NORFLASH_ENABLE
		case FLASH_DEV_EXTNOR:
			nAdr = p->sec * NORFLASH_BLK_SIZE;
			norf_Erase(nAdr);
			norf_Program(nAdr, p->fbuf, NORFLASH_BLK_SIZE);
			break;
#endif

#if SPIFLASH_ENABLE
		case FLASH_DEV_SPINOR:
#if SPIF_PROTECT_ENABLE
			spif_SecErase(SPIF_PROTECT_SEC);
			spif_Program(SPIF_PROTECT_SEC, p->fbuf);
			sfs_Write(&spif_IdxDev, 1, &p->sec, sizeof(p->sec));
#endif

			spif_SecErase(p->sec);
			spif_Program(p->sec, p->fbuf);
			
#if SPIF_PROTECT_ENABLE
			sfs_Delete(&spif_IdxDev, 1);
#endif
			break;
#endif

		default:
			break;
		}
		
		p->dirty = 0;
	}
}








void flash_Init()
{
	flash_buf_t *p = &flash_buf;

#if FLASH_LOCK_ENABLE
	os_sem_init(&flash_sem, 1);
#endif

#if SPIF_PROTECT_ENABLE
	if (sfs_Read(&spif_IdxDev, 0, NULL) < 0)
	{
		sfs_Init(&spif_IdxDev);
		sfs_Write(&spif_IdxDev, 0, NULL, 0);
	}
	
	if (sfs_Read(&spif_IdxDev, 1, &p->sec) < 0)
	{
		spif_ReadLen(SPIF_PROTECT_SEC, 0, p->fbuf, SPIF_SEC_SIZE);
		spif_SecErase(p->sec);
		spif_Program(p->sec, p->fbuf);
		
		sfs_Delete(&spif_IdxDev, 1);
	}
#endif

	p->type = FLASH_DEV_NULL;
	p->sec = FLASH_BLOCK_INVALID;
	p->dirty = 0;
}

#if INTFLASH_ENABLE
void intf_Read(adr_t nAdr, void *pBuf, size_t nLen)
{
	flash_buf_t *p = &flash_buf;
	u8 *pData, *pTemp;
	int nSec, nOffset;
	size_t nRead;

	flash_Lock();
	
	for (pData = (u8 *)pBuf; nLen; nLen -= nRead, pData += nRead, nAdr += nRead)
	{
		nSec = nAdr / INTFLASH_BLK_SIZE;
		nOffset = nAdr % INTFLASH_BLK_SIZE;
		nRead = MIN(INTFLASH_BLK_SIZE - nOffset, nLen);
		
		if ((p->type == FLASH_DEV_INT) && (nSec == p->sec))
		{
			pTemp = p->fbuf + nOffset;
			memcpy(pData, pTemp, nRead);
		}
		else
		{
			memcpy(pData, (void *)nAdr, nRead);
		}
	}
	
	flash_Unlock();
}

void intf_Write(adr_t nAdr, const void *pBuf, size_t nLen)
{
	flash_buf_t *p = &flash_buf;
	u8 *pData, *pTemp;
	int nSec, nOffset;
	size_t nWrite;

	flash_Lock();
	
	for (pData = (u8 *)pBuf; nLen; nLen -= nWrite, pData += nWrite, nAdr += nWrite)
	{
		nSec = nAdr / INTFLASH_BLK_SIZE;
		nOffset = nAdr % INTFLASH_BLK_SIZE;
		nWrite = MIN(INTFLASH_BLK_SIZE - nOffset, nLen);
		
		if ((p->type != FLASH_DEV_INT) || (nSec != p->sec))
		{
			_flash_Flush(0);
			
			memcpy(p->fbuf, (void *)(nSec * INTFLASH_BLK_SIZE), INTFLASH_BLK_SIZE);
			
			p->sec = nSec;
			p->type = FLASH_DEV_INT;
		}
		
		pTemp = p->fbuf + nOffset;
		memcpy(pTemp, pData, nWrite);
		
		p->dirty = rtc_GetTimet();
	}
	
	flash_Unlock();
}
#endif



#if SPIFLASH_ENABLE
#if SPIFS_ENABLE
void spif_SecRead(int nSec, void *pBuf)
{
	flash_buf_t *p = &flash_buf;

	flash_Lock();
	
	if ((p->type == FLASH_DEV_SPINOR) && (p->sec == nSec))
		memcpy(pBuf, p->fbuf, SPIF_SEC_SIZE);
	else
		spif_ReadLen(nSec, 0, pBuf, SPIF_SEC_SIZE);
	
	flash_Unlock();
}

void spif_SecWrite(int nSec, const void *pBuf)
{
	flash_buf_t *p = &flash_buf;
	u32 *pLast, *pData;

	flash_Lock();
	
	if ((p->type == FLASH_DEV_SPINOR) && (p->sec == nSec))
	{
		memcpy(p->fbuf, pBuf, SPIF_SEC_SIZE);
		p->dirty = rtc_GetTimet();
	}
	else
	{
		pData = (u32 *)pBuf;
		pLast = pData + (SPIF_SEC_SIZE / 4);
		while (pData < pLast)
		{
			if (*pData++ != 0xFFFFFFFF)
				break;
		}
		
		if (pData < pLast)
		{
			_flash_Flush(0);
			
			memcpy(p->fbuf, pBuf, SPIF_SEC_SIZE);

			p->sec = nSec;
			p->type = FLASH_DEV_SPINOR;
			p->dirty = rtc_GetTimet();
		}
		else
		{
			spif_SecErase(nSec);
		}
	}

	flash_Unlock();
}
#endif

void spif_Read(adr_t nAdr, void *pBuf, size_t nLen)
{
	flash_buf_t *p = &flash_buf;
	u8 *pData, *pTemp;
	int nSec, nOffset;
	size_t nRead;

	flash_Lock();

	for (pData = (u8 *)pBuf; nLen; nLen -= nRead, pData += nRead, nAdr += nRead)
	{
		nSec = nAdr / SPIF_SEC_SIZE;
		nOffset = nAdr % SPIF_SEC_SIZE;
		nRead = MIN(SPIF_SEC_SIZE - nOffset, nLen);
		
		if ((p->type == FLASH_DEV_SPINOR) && (nSec == p->sec))
		{
			pTemp = p->fbuf + nOffset;
			memcpy(pData, pTemp, nRead);
		}
		else
		{
			spif_ReadLen(nSec, nOffset, pData, nRead);
		}
	}
	
	flash_Unlock();
}

void spif_Write(adr_t nAdr, const void *pBuf, size_t nLen)
{
	flash_buf_t *p = &flash_buf;
	u8 *pData, *pTemp;
	int nSec, nOffset;
	size_t nWrite;

	flash_Lock();
	
	for (pData = (u8 *)pBuf; nLen; nLen -= nWrite, pData += nWrite, nAdr += nWrite)
	{
		nSec = nAdr / SPIF_SEC_SIZE;
		nOffset = nAdr % SPIF_SEC_SIZE;
		nWrite = MIN(SPIF_SEC_SIZE - nOffset, nLen);
		if ((p->type != FLASH_DEV_SPINOR) || (nSec != p->sec))
		{
			_flash_Flush(0);
			
			spif_ReadLen(nSec, 0, p->fbuf, SPIF_SEC_SIZE);
			
			p->sec = nSec;
			p->type = FLASH_DEV_SPINOR;
		}
		
		pTemp = p->fbuf + nOffset;
		memcpy(pTemp, pData, nWrite);
		p->dirty = rtc_GetTimet();
	}
	
	flash_Unlock();
}

void spif_Fill(adr_t nAdr, int nVal, size_t nLen)
{
	size_t nFill;
	u8 aBuf[64];

	memset(aBuf, nVal, sizeof(aBuf));
	
	for (; nLen; nLen -= nFill, nAdr += nFill)
	{
		nFill = MIN(sizeof(aBuf), nLen);
		spif_Write(nAdr, aBuf, nFill);
	}
}
#endif


void flash_Flush(int nDelay)
{

	flash_Lock();
	
	_flash_Flush(nDelay);
	
	flash_Unlock();
}


#endif

