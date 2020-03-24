#if INTFLASH_ENABLE


//Private Defines
#define STM32_INTF_LOCK_ENABLE		0

 
//Private Macros
#if STM32_INTF_LOCK_ENABLE
#define stm32_intf_Lock()			os_thd_lock()
#define stm32_intf_Unlock()			os_thd_unlock()
#else
#define stm32_intf_Lock()
#define stm32_intf_Unlock()
#endif



void arch_IntfInit()
{

	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPTERR | FLASH_FLAG_WRPRTERR | 
					FLASH_FLAG_PGERR | FLASH_FLAG_BSY);
}

sys_res arch_IntfErase(adr_t adr)
{
	FLASH_Status res = FLASH_COMPLETE;
	adr_t cur, end;

	stm32_intf_Lock();

	cur = adr;
	end = adr + INTFLASH_BLK_SIZE;
	for (; cur < end; cur += 4)
	{
		if (*(volatile u32 *)cur != 0xFFFFFFFF)
			break;
	}
	if (cur < end)
		res = FLASH_ErasePage(adr);

	stm32_intf_Unlock();

	if (res == FLASH_COMPLETE)
		return SYS_R_OK;

	return SYS_R_TMO;
}

sys_res arch_IntfProgram(adr_t adr, const void *pd, size_t len)
{
	adr_t end;
	u16 data;
	u8 *p = (u8 *)pd

	stm32_intf_Lock();
	
	for (end = adr + len; adr < end; )
	{
		memcpy(&data, p, 2);
		if (*(volatile u16 *)adr != data)
		{
			if (FLASH_ProgramHalfWord(adr, data) != FLASH_COMPLETE)
				break;
		}
		adr += 2;
		p += 2;
	}
	
	stm32_intf_Unlock();
	
	if (adr < end)
		return SYS_R_TMO;
	
	return SYS_R_OK;
}


#endif

