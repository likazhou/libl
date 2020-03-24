#ifndef __SPIBUS_H__
#define __SPIBUS_H__



#ifdef __cplusplus
extern "C" {
#endif








//External Functions
sys_res spibus_Init(spi_t *p);
sys_res spibus_Config(spi_t *p);
void spibus_Start(spi_t *p);
void spibus_SendChar(spi_t *p, u8 nData);
void spibus_End(spi_t *p);
sys_res spibus_Send(spi_t *p, const void *pData, size_t nLen);
sys_res spibus_Recv(spi_t *p, void *pRec, size_t nLen);
sys_res spibus_Transce(spi_t *p, const void *pCmd, size_t nCmdLen, void *pRec, size_t nRecLen);
sys_res spibus_TransChar(spi_t *p, u8 nSend, void *pRec);


#ifdef __cplusplus
}
#endif

#endif


