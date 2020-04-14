#ifndef __ARCH_EPI_H__
#define __ARCH_EPI_H__


#ifdef __cplusplus
extern "C" {
#endif








//External Functions
void stm32_FsmcInit(void);

__inline u32 arch_NandGetEcc(void);
__inline void arch_NandEccStart(void);




#ifdef __cplusplus
}
#endif

#endif

