

//Private Defines
#define LM3S_INTF_LOCK_ENABLE		0


//Private Macros
#if LM3S_INTF_LOCK_ENABLE
#define lm3s_intf_Lock()			os_thd_Lock()
#define lm3s_intf_Unlock()			os_thd_Unlock()
#else
#define lm3s_intf_Lock()
#define lm3s_intf_Unlock()
#endif



void arch_IntfInit()
{

	MAP_FlashUsecSet(MAP_SysCtlClockGet() / 1000000);
}


sys_res arch_IntfErase(adr_t nAdr)
{
	int res = 0;
	adr_t nCur, nEndAdr;

	lm3s_intf_Lock();
	
	nCur = nAdr;
	nEndAdr = nCur + INTFLASH_BLK_SIZE;
	for (; nCur < nEndAdr; nCur += 4)
	{
		if (*(volatile u32 *)nCur != 0xFFFFFFFF)
			break;
	}
	
	if (nCur < nEndAdr)
		res = MAP_FlashErase(nAdr);
	
	lm3s_intf_Unlock();

	if (res)
		return SYS_R_TMO;

	return SYS_R_OK;
}


#if 1
sys_res arch_IntfProgram(adr_t nAdr, const void *pData, uint nLen)
{
	adr_t aEnd;
	__packed u32 *p = (u32 *)pData;

	lm3s_intf_Lock();

	aEnd = nAdr + nLen;
	for (; nAdr < aEnd; nAdr += 4)
	{
		while (__raw_readl(FLASH_FMC) & FLASH_FMC_WRITE);
		__raw_writel(*p++, FLASH_FMD);
		__raw_writel(nAdr, FLASH_FMA);
		__raw_writel(FLASH_FMC_WRKEY | FLASH_FMC_WRITE, FLASH_FMC);
	}

	lm3s_intf_Unlock();

	return SYS_R_OK;
}
#else
sys_res arch_IntfProgram(adr_t adr, const void *pData, uint nLen)
{
	int res;
	
	lm3s_intf_Lock();
	
	res = MAP_FlashProgram((unsigned long *)pData, adr, nLen);
	
	lm3s_intf_Unlock();
	
	if (res)
		return SYS_R_TMO;
	
	return SYS_R_OK;
}
#endif



