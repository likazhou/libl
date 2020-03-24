/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "diskio.h"

/*-----------------------------------------------------------------------*/
/* Correspondence between physical drive number and physical drive.      */

#define NAND	0
#define ATA		1
#define MMC		2
#define USB		3



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */

DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{

	if (ml_init())
		if (ml_format())
			return RES_ERROR;
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{

	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	DRESULT res = RES_OK;
	uint_t i;
	unsigned char *p = (unsigned char *)buff;

	if (ml_open(sector, count, ML_READ))
		return RES_ERROR;
	for (i = count; i; i--, p += _MAX_SS)
		if (ml_read(p)) {
			res = RES_ERROR;
			break;
		}
	if (ml_close())
		return RES_ERROR;
	return res;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
	DRESULT res = RES_OK;
	uint_t i;
	unsigned char *p = (unsigned char *)buff;

	if (ml_open(sector, count, ML_WRITE))
		return RES_ERROR;
	for (i = count; i; i--, p += _MAX_SS)
		if (ml_write(p)) {
			res = RES_ERROR;
			break;
		}
	if (ml_close())
		return RES_ERROR;
	return res;
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	

	switch (ctrl) {
	case GET_SECTOR_COUNT:
		*(DWORD *)buff = ml_getmaxsector() * MAX_PAGE_PER_BLOCK;
		break;
	case GET_SECTOR_SIZE:
		*(WORD *)buff = MAX_DATA_SIZE;
		break;
	case GET_BLOCK_SIZE:
		*(DWORD *)buff = 1;
		break;
	case CTRL_SYNC:
	default:
		break;
	}
	return RES_OK;
}

DWORD get_fattime (void)
{

	return 0;
}
