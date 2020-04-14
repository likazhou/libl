#ifndef __ARCH_SPI_H__
#define __ARCH_SPI_H__


#ifdef __cplusplus
extern "C" {
#endif











//External Functions
void arch_SpiInit(spi_t *p);
sys_res arch_SpiConfig(spi_t *p, int sckmode, int latch, int speed);
sys_res arch_SpiSend(spi_t *p, const void *send, size_t len);
sys_res arch_SpiRecv(spi_t *p, u8 *rec, size_t len);
sys_res arch_SpiTransce(spi_t *p, const void *send, size_t slen, u8 *rec, size_t rlen);



#ifdef __cplusplus
}
#endif

#endif

