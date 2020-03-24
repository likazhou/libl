#if NORFLASH_ENABLE
#include <drivers/norflash.h>

//Private Defines
#define NORFLASH_LOCK_ENABLE	1


//Private Const
#if NORFLASH_LOCK_ENABLE && OS_TYPE
#define norf_Lock()				os_thd_lock()
#define norf_Unlock()			os_thd_unlock()
#else
#define norf_Lock(...)
#define norf_Unlock(...)
#endif


//Public variables



//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
static sys_res norf_ToggleDone(adr_t adr, int nIsErase)
{
	size_t nTmo;
	u16 nTarget;

	for (nTmo = 0x00FFFFFF; nTmo; nTmo--)
	{
		nTarget = specAddress(adr);
		if ((nTarget & 0x0044) == (specAddress(adr) & 0x0044))
			return SYS_R_OK;
		
		if ((nTarget & BITMASK(5)) == 0)
		{
			if (nIsErase)
			{
#if OS_TYPE
				os_thd_slp1tick();
#else
				wdg_Reload(0);
#endif
			}
		}
		else
		{
			if ((specAddress(adr) & 0x0044) != (specAddress(adr) & 0x0044))
				return SYS_R_ERR;
		}
	}
	
	return SYS_R_TMO;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void norf_Init()
{

}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res norf_EraseChip()
{
	sys_res res;
	
	norf_Lock();
	
	sysAddress(0x0555) = 0x00AA;
	sysAddress(0x02AA) = 0x0055;
	sysAddress(0x0555) = 0x0080;
	sysAddress(0x0555) = 0x00AA;
	sysAddress(0x02AA) = 0x0055;
	sysAddress(0x0555) = 0x0010;
	
	res = norf_ToggleDone(NORFLASH_BASE_ADR, 1);
	
	norf_Unlock();
	return res;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res norf_nolockErase(adr_t adr)
{

	sysAddress(0x0555) = 0x00AA;
	sysAddress(0x02AA) = 0x0055;
	sysAddress(0x0555) = 0x0080;
	sysAddress(0x0555) = 0x00AA;
	sysAddress(0x02AA) = 0x0055;
	
	specAddress(adr) = 0x0030;
	
	return norf_ToggleDone(adr, 1);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res norf_Erase(adr_t adr)
{
	sys_res res;
	
	norf_Lock();
	
	res = norf_nolockErase(adr);
	
	norf_Unlock();
	
	return res;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
#if 0
sys_res norf_nolockProgram_seq(adr_t adr, const void *pBuf, size_t nLen)
{
	int nData;
	u8 *pData = (u8 *)pBuf;

	if (nLen)
	{
		//进入编程模式
		sysAddress(0x0555) = 0x00AA;
		sysAddress(0x02AA) = 0x0055;
		sysAddress(0x0555) = 0x0020;
		for (nLen += adr; adr < nLen; adr += 2, pData += 2)
		{
			nData = (*(pData + 1) << 8) | *pData;
			if (*(volatile u16 *)adr == nData)
				continue;
			specAddress(NORFLASH_BASE_ADR) = 0x00A0;
			specAddress(adr) = nData;
			if (norf_ToggleDone(adr, 0) != SYS_R_OK)
			{
				//失败, 复位Flash
				specAddress(NORFLASH_BASE_ADR) = 0x00F0;
				return SYS_R_TMO;
			}
		}
		//正常退出编程模式
		specAddress(NORFLASH_BASE_ADR) = 0x0090;
		specAddress(NORFLASH_BASE_ADR) = 0x0000;
	}
	
	return SYS_R_OK;
}
#endif

sys_res norf_nolockProgram(adr_t adr, const void *pBuf, size_t nLen)
{
	int i, nData;
	u8 *pData = (u8 *)pBuf;

	for (nLen += adr; adr < nLen; adr += 2, pData += 2)
	{
		nData = (*(pData + 1) << 8) | *pData;
		for (i = 0; i < 50; i++)
		{
			if (*(volatile u16 *)adr == nData)
				break;
			
			sysAddress(0x0555) = 0x00AA;
			sysAddress(0x02AA) = 0x0055;
			sysAddress(0x0555) = 0x00A0;
			specAddress(adr) = nData;
			
			if (norf_ToggleDone(adr, 0) != SYS_R_OK)
			{
				//失败, 复位Flash
				specAddress(NORFLASH_BASE_ADR) = 0x00F0;
				return SYS_R_TMO;
			}
		}
	}
	
	return SYS_R_OK;
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res norf_Program(adr_t adr, const void *pBuf, size_t nLen)
{
	sys_res res;
	
	norf_Lock();
	
	res = norf_nolockProgram(adr, pBuf, nLen);
	
	norf_Unlock();
	
	return res;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res norf_Read(adr_t adr, void *pBuf, size_t nLen)
{

	norf_Lock();
	
	memcpy(pBuf, (u8 *)adr, nLen);
	
	norf_Unlock();
	
	return SYS_R_OK;
}

#endif

