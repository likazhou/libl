#ifndef __SPIFLASH_H__
#define __SPIFLASH_H__



#ifdef __cplusplus
extern "C" {
#endif





//Public Defines
#define SPIF_SEC_SIZE			0x1000
#define SPIF_SEC_QTY			1024


//Public Typedefs



//External Functions
void spif_Init(void);
void spif_ReadLen(int nSec, int nOffset, void *pData, size_t nLen);
void spif_Program(int nSec, const void *pData);
int spif_GetSize(void);




#ifdef __cplusplus
}
#endif

#endif



