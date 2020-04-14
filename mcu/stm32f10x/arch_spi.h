#ifndef __ARCH_SPI_H__
#define __ARCH_SPI_H__


#ifdef __cplusplus
extern "C" {
#endif











//External Functions
void arch_SpiInit(spi_t *p);
sys_res arch_SpiConfig(spi_t *p);
sys_res arch_SpiSend(spi_t *p, const void *pSend, size_t nLen);
sys_res arch_SpiRecv(spi_t *p, u8 *pRec, size_t nLen);
sys_res arch_SpiTransce(spi_t *p, int nCmd, u8 *pRec, size_t nLen);



#ifdef __cplusplus
}
#endif

#endif

