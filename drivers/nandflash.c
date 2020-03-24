#if NANDFLASH_ENABLE
#include <drivers/nandflash.h>


//Parivate Defines
#define NAND_RWL_TMO				0xFFFF


//Nand Device ID Defines
#if NAND_BLK_QTY == 1024
#define NAND_DEVICEID				0x34
#elif NAND_BLK_QTY == 2048
#define NAND_DEVICEID				0x35
#elif NAND_BLK_QTY == 4096
#define NAND_DEVICEID				0x36
#elif NAND_BLK_QTY == 8192
#define NAND_DEVICEID				0x37
#endif


//FSMC NAND memory command
#define NAND_CMD_AREA_A				0x00
#define NAND_CMD_AREA_B				0x01
#define NAND_CMD_AREA_C				0x50
#define NAND_CMD_AREA_TRUE1			0x30

#define NAND_CMD_WRITE0				0x80
#define NAND_CMD_WRITE_TRUE1		0x10
	
#define NAND_CMD_ERASE0				0x60
#define NAND_CMD_ERASE1				0xD0

#define NAND_CMD_READID				0x90
#define NAND_CMD_STATUS				0x70
#define NAND_CMD_LOCK_STATUS		0x7A
#define NAND_CMD_RESET				0xFF


//NAND memory status
#define NAND_VALID_ADDRESS			0x00000100
#define NAND_INVALID_ADDRESS		0x00000200
#define NAND_TIMEOUT_ERROR			0x00000400
#define NAND_BUSY					0x00000000
#define NAND_ERROR					0x00000001
#define NAND_READY					0x00000040




#if NAND_ECC_SOFT
static u32 nand_GetEcc(void *pBuf, size_t nLen)
{

	return 0;
}
#endif


static u32 nand_GetStatus()
{

	__raw_nand_cmd(NAND_CMD_STATUS);
	return __raw_nand_read;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void nand_Init()
{

}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res nand_Identify()
{
	u_byte4 xId;
	int i;
#if NAND_DATA_WIDTH == 16
	u16 *p = (u16 *)&xId;
#else
	u8 *p = (u8 *)&xId;
#endif

	__raw_nand_cmd(NAND_CMD_RESET);
	__raw_nand_cmd(NAND_CMD_READID);
	__raw_nand_addr(0x00);

	for (i = 0; i < (4 / (NAND_DATA_WIDTH / 8)); i++)
	{
		*p++ = __raw_nand_read;
	}
	
	return ((xId.byte[1] & 0x3F) != NAND_DEVICEID) ? SYS_R_ERR : SYS_R_OK;
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res nand_ReadPage(u_byte4 *pPage, void *pBuf, u32 *pDataEcc)
{
	int i;
#if NAND_DATA_WIDTH == 16
	u16 *p;
#else
	u8 *p;
#endif

	__raw_nand_cmd(NAND_CMD_AREA_A);
	__raw_nand_addr(0);
	__raw_nand_addr(pPage->byte[0]);
	__raw_nand_addr(pPage->byte[1]);
	__raw_nand_addr(pPage->byte[2]);
#if NAND_ECC_SOFT == 0
	arch_NandEccStart();
#endif	
#if NAND_BUSY_SOFT
	sys_Delay(100);//Threshold 80
#endif
	for (p = pBuf, i = 0; i < (NAND_PAGE_DATA / (NAND_DATA_WIDTH / 8)); i++)
	{
		*p++ = __raw_nand_read;
	}
#if NAND_ECC_SOFT == 0
	*pDataEcc = arch_NandGetEcc();
#endif
	for (i = 0; i < (NAND_PAGE_SPARE / (NAND_DATA_WIDTH / 8)); i++)
	{
		*p++ = __raw_nand_read;
	}
	
	return SYS_R_OK;
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res nand_ProgData(u_byte4 *pPage, const void *pBuf, u32 *pDataEcc)
{
	int i;
#if NAND_DATA_WIDTH == 16
	const u16 *p;
#else
	const u8 *p;
#endif

	__raw_nand_cmd(NAND_CMD_AREA_A);
	__raw_nand_cmd(NAND_CMD_WRITE0);
	__raw_nand_addr(0);
	__raw_nand_addr(pPage->byte[0]);
	__raw_nand_addr(pPage->byte[1]);
	__raw_nand_addr(pPage->byte[2]);
#if NAND_ECC_SOFT == 0
	arch_NandEccStart();
#endif	
	for (p = pBuf, i = 0; i < (NAND_PAGE_DATA / (NAND_DATA_WIDTH / 8)); i++)
	{
		__raw_nand_read = *p++;
	}
#if NAND_ECC_SOFT == 0
	*pDataEcc = arch_NandGetEcc();
#endif
	__raw_nand_cmd(NAND_CMD_WRITE_TRUE1);

#if NAND_BUSY_SOFT
	while ((nand_GetStatus() & BITMASK(6)) == 0);
#endif

	return (nand_GetStatus() & BITMASK(0)) ? SYS_R_ERR : SYS_R_OK;
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res nand_ProgHalfDouble(u_byte4 *pPage, const void *pBuf0, const void *pBuf1, u32 *pDataEcc)
{
	int i;
#if NAND_DATA_WIDTH == 16
	const u16 *p;
#else
	const u8 *p;
#endif

	__raw_nand_cmd(NAND_CMD_AREA_A);
	__raw_nand_cmd(NAND_CMD_WRITE0);
	__raw_nand_addr(0);
	__raw_nand_addr(pPage->byte[0]);
	__raw_nand_addr(pPage->byte[1]);
	__raw_nand_addr(pPage->byte[2]);
#if NAND_ECC_SOFT == 0
	arch_NandEccStart();
#endif
	for (p = pBuf0, i = 0; i < (NAND_PAGE_DATA / 2 / (NAND_DATA_WIDTH / 8)); i++)
	{
		__raw_nand_read = *p++;
	}
	for (p = pBuf1, i = 0; i < (NAND_PAGE_DATA / 2 / (NAND_DATA_WIDTH / 8)); i++)
	{
		__raw_nand_read = *p++;
	}
#if NAND_ECC_SOFT == 0
	*pDataEcc = arch_NandGetEcc();
#endif
	__raw_nand_cmd(NAND_CMD_WRITE_TRUE1);

#if NAND_BUSY_SOFT
	while ((nand_GetStatus() & BITMASK(6)) == 0);
#endif

	return (nand_GetStatus() & BITMASK(0)) ? SYS_R_ERR : SYS_R_OK;
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res nand_ProgSpare(u_byte4 *pPage, const void *pBuf)
{
	int i;
#if NAND_DATA_WIDTH == 16
	const u16 *p;
#else
	const u8 *p;
#endif

	__raw_nand_cmd(NAND_CMD_AREA_C);
	__raw_nand_cmd(NAND_CMD_WRITE0);
	__raw_nand_addr(0);
	__raw_nand_addr(pPage->byte[0]);
	__raw_nand_addr(pPage->byte[1]);
	__raw_nand_addr(pPage->byte[2]);
	for (p = pBuf, i = 0; i < (NAND_PAGE_SPARE / (NAND_DATA_WIDTH / 8)); i++)
	{
		__raw_nand_read = *p++;
	}
	__raw_nand_cmd(NAND_CMD_WRITE_TRUE1);

#if NAND_BUSY_SOFT
	while ((nand_GetStatus() & BITMASK(6)) == 0);
#endif

	return (nand_GetStatus() & BITMASK(0)) ? SYS_R_ERR : SYS_R_OK;
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res nand_EraseBlock(u_byte4 *pPage)
{

	__raw_nand_cmd(NAND_CMD_ERASE0);
	__raw_nand_addr(pPage->byte[0]);
	__raw_nand_addr(pPage->byte[1]);
	__raw_nand_addr(pPage->byte[2]);
	__raw_nand_cmd(NAND_CMD_ERASE1);
	
#if NAND_BUSY_SOFT
	while ((nand_GetStatus() & BITMASK(6)) == 0);
#endif

	return (nand_GetStatus() & BITMASK(0)) ? SYS_R_ERR : SYS_R_OK;
}



#endif


