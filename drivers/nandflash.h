#ifndef __NAND_FLASH_H__
#define __NAND_FLASH_H__



#ifdef __cplusplus
extern "C" {
#endif






//-------------------------------------------------------------------------
//External Functions
//-------------------------------------------------------------------------
void nand_Init(void);
sys_res nand_Identify(void);
sys_res nand_ReadPage(u_byte4 *pPage, void *pBuf, u32 *pDataEcc);
sys_res nand_ProgData(u_byte4 *pPage, const void *pBuf, u32 *pDataEcc);
sys_res nand_ProgHalfDouble(u_byte4 *pPage, const void *pBuf0, const void *pBuf1, u32 *pDataEcc);
sys_res nand_ProgSpare(u_byte4 *pPage, const void *pBuf);
sys_res nand_EraseBlock(u_byte4 *pPage);


#ifdef __cplusplus
}
#endif


#endif

