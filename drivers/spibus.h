#ifndef __SPIBUS_H__
#define __SPIBUS_H__



#ifdef __cplusplus
extern "C" {
#endif








//External Functions
sys_res spibus_Init(spi_t *p);
sys_res spibus_Config(spi_t *p);
void spibus_Start(spi_t *p);
void spibus_SendChar(spi_t *p, u8 data);
void spibus_End(spi_t *p);
sys_res spibus_Send(spi_t *p, const void *send, size_t len);
sys_res spibus_Recv(spi_t *p, void *rec, size_t len);
sys_res spibus_TransThenRecv(spi_t *p, const void *send, size_t slen, void *rec, size_t rlen);
sys_res spibus_Transmit(spi_t *p, const void *send, void *rec, size_t len);


#ifdef __cplusplus
}
#endif

#endif


