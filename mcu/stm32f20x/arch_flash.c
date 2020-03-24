#if INTFLASH_ENABLE


//Private Defines
#define STM32_INTF_LOCK_ENABLE		0

#define ADDR_FLASH_SECTOR_0     ((u32)0x08000000) /* Base @ of Sector 0, 16 Kbyte */
#define ADDR_FLASH_SECTOR_1     ((u32)0x08004000) /* Base @ of Sector 1, 16 Kbyte */
#define ADDR_FLASH_SECTOR_2     ((u32)0x08008000) /* Base @ of Sector 2, 16 Kbyte */
#define ADDR_FLASH_SECTOR_3     ((u32)0x0800C000) /* Base @ of Sector 3, 16 Kbyte */
#define ADDR_FLASH_SECTOR_4     ((u32)0x08010000) /* Base @ of Sector 4, 64 Kbyte */
#define ADDR_FLASH_SECTOR_5     ((u32)0x08020000) /* Base @ of Sector 5, 128 Kbyte */
#define ADDR_FLASH_SECTOR_6     ((u32)0x08040000) /* Base @ of Sector 6, 128 Kbyte */
#define ADDR_FLASH_SECTOR_7     ((u32)0x08060000) /* Base @ of Sector 7, 128 Kbyte */
#define ADDR_FLASH_SECTOR_8     ((u32)0x08080000) /* Base @ of Sector 8, 128 Kbyte */
#define ADDR_FLASH_SECTOR_9     ((u32)0x080A0000) /* Base @ of Sector 9, 128 Kbyte */
#define ADDR_FLASH_SECTOR_10    ((u32)0x080C0000) /* Base @ of Sector 10, 128 Kbyte */
#define ADDR_FLASH_SECTOR_11    ((u32)0x080E0000) /* Base @ of Sector 11, 128 Kbyte */


//Private Macros
#if STM32_INTF_LOCK_ENABLE
#define stm32_intf_Lock()			os_thd_lock()
#define stm32_intf_Unlock()			os_thd_unlock()
#else
#define stm32_intf_Lock()
#define stm32_intf_Unlock()
#endif




//Internal Functions
static u32 arch_flashGetSector(adr_t Address)
{
	u32 sector = 0;

	if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
	{
		sector = FLASH_Sector_0;
	}
	else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
	{
		sector = FLASH_Sector_1;
	}
	else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
	{
		sector = FLASH_Sector_2;
	}
	else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
	{
		sector = FLASH_Sector_3;
	}
	else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
	{
		sector = FLASH_Sector_4;
	}
	else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
	{
		sector = FLASH_Sector_5;
	}
	else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
	{
		sector = FLASH_Sector_6;
	}
	else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
	{
		sector = FLASH_Sector_7;
	}
	else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
	{
		sector = FLASH_Sector_8;
	}
	else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
	{
		sector = FLASH_Sector_9;
	}
	else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
	{
		sector = FLASH_Sector_10;
	}
	else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
	{
		sector = FLASH_Sector_11;
	}
	return sector;
}


//External Functions
void arch_IntfInit()
{

	FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
    				FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR| FLASH_FLAG_PGSERR);
}

sys_res arch_IntfErase(adr_t nAdr)
{
	FLASH_Status res = FLASH_COMPLETE;

	stm32_intf_Lock();
	
    res = FLASH_EraseSector(arch_flashGetSector(nAdr), VoltageRange_3);
	
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

