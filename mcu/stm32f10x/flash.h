#ifndef __ARCH_FLASH_H__
#define __ARCH_FLASH_H__


#ifdef __cplusplus
extern "C" {
#endif









#define INTFLASH_STM32_LD		0x400
#define INTFLASH_STM32_MD		0x400
#define INTFLASH_STM32_HD		0x800


//External Functions
void arch_IntfInit(void);
sys_res arch_IntfErase(adr_t adr);
sys_res arch_IntfProgram(adr_t adr, const void *pData, size_t nLen);


#ifdef __cplusplus
}
#endif

#endif

