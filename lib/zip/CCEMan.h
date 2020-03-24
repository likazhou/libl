

#ifndef _CCEMAN_H_
#define _CCEMAN_H_


	
#define MAXSBUFLEN			4096

#define EXE_COMPRESS_NEW	0x01//新的压缩 
#define EXE_SHA				0x02//完整性验证 
#define EXE_ENCRYPT			0x04//加密

#include <lib/zip/CrypFun.h>
#include <lib/zip/CompressFun.h>
#include <lib/zip/CompressFunNew.h>


extern unsigned char SendBuf[MAXSBUFLEN];
extern unsigned char RecvBuf[MAXSBUFLEN];



void zip_Init(void);
void zip_Lock(void);
void zip_Unlock(void);

#if ZIP_ENCRYPT_ENABLE
void CCEManInit(void);
int SetKey(BYTE MainKey[4][4], int keylen);
#endif

int EnData(BYTE * DataBuf, int DataLen, unsigned char Oper);
int DeData(BYTE * DataBuf, int DataLen);






#endif //(_CCEMAN_H_)

