#ifndef __SYS_FLASH_H__
#define __SYS_FLASH_H__


#ifdef __cplusplus
extern "C" {
#endif

//Header Files
#if INTFLASH_ENABLE
#include <drivers/intflash.h>
#endif
#if NANDFLASH_ENABLE
#include <drivers/nandflash.h>
#endif	  
#if NORFLASH_ENABLE
#include <drivers/norflash.h>
#endif	  
#if SPIFLASH_ENABLE
#include <drivers/spiflash.h>
#endif



//Public Defines
#define FLASH_DEV_NULL			0
#define FLASH_DEV_INT			1
#define FLASH_DEV_EXTNOR		2
#define FLASH_DEV_SPINOR		3



//Public Typedefs
struct _flash_dev
{
	u16	dev;
	u16	blk;
	adr_t	start;
};
typedef const struct _flash_dev			flash_dev_t;


//External Functions
size_t flash_BlkSize(int nDev);
sys_res flash_NolockErase(int nDev, adr_t nAdr);
sys_res flash_NolockProgram(int nDev, adr_t nAdr, const void *pData, size_t nLen);

sys_res flash_Erase(int nDev, adr_t nAdr);
sys_res flash_Program(int nDev, adr_t nAdr,const void *pData, size_t nLen);

void flash_Init(void);

void intf_Read(adr_t nAdr, void *pBuf, size_t nLen);
void intf_Write(adr_t nAdr, const void *pBuf, size_t nLen);

void spif_SecRead(int nSec, void *pData);
void spif_SecWrite(int nSec, const void *pBuf);
void spif_Read(adr_t nAdr, void *pData, size_t nLen);
void spif_Write(adr_t nAdr, const void *pBuf, size_t nLen);
void spif_Fill(adr_t nAdr, int nVal, size_t nLen);

void flash_Flush(int nDelay);




#ifdef __cplusplus
}
#endif

#endif

