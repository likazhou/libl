#ifndef __RC522_H__
#define __RC522_H__




#ifdef __cplusplus
extern "C" {
#endif






#define MF_S_IDEL			0
#define MF_S_ACTIVE			1
#define MF_S_BUSY			2
typedef struct {
	spi_t *	p;
	u8		ste : 7,
			block : 1;
	u16		ctype;
	u32		cid;
}mifare[1];




//External Functions
void mf_InitGpio(void);
void mf_Init(mifare mf);
void mf_Release(mifare mf);
sys_res mf_Get(mifare mf, int nId, size_t nTmo);
sys_res mf_FindCard(mifare mf, int nOp);
sys_res mf_SelectCard(mifare mf);
sys_res mf_PcdReset(mifare mf);
sys_res mf_SelectApp(mifare mf, int nParam);
sys_res mf_Send(mifare mf, int nIns, int nP1, int nP2, void *pBuf, size_t nIn, size_t nOut);




#ifdef __cplusplus
}
#endif

#endif

