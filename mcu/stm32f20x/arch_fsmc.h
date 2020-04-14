#ifndef __ARCH_EPI_H__
#define __ARCH_EPI_H__

#ifdef __cplusplus
extern "C" {
#endif





//External Functions
void stm32_FsmcInit(void);

#if NANDFLASH_ENABLE && NAND_ECC_SOFT == 0
__INLINE u32 arch_NandGetEcc(void);
__INLINE void arch_NandEccStart(void);
#endif


#ifdef __cplusplus
}
#endif

#endif

