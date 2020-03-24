#ifndef __BKP_H__
#define __BKP_H__


#ifdef __cplusplus
extern "C" {
#endif



//Public Defines
#define BKP_T_NULL			0
#define BKP_T_INT			1
#define BKP_T_EEPROM		2
#define BKP_T_SFS			3




//External Functions
void bkp_Init(void);
sys_res bkp_Write(adr_t nAdr, const void *pBuf, size_t nLen);
sys_res bkp_WriteData(adr_t nAdr, const u64 nData, size_t nLen);
void bkp_Fill(adr_t nAdr, adr_t nEnd, int nVal);
sys_res bkp_Read(adr_t nAdr, void *pBuf, size_t nLen);


#ifdef __cplusplus
}
#endif

#endif

