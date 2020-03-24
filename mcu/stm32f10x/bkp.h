#ifndef __ARCH_BKP_H__
#define __ARCH_BKP_H__


#ifdef __cplusplus
extern "C" {
#endif













//External Functions
void arch_BkpInit(void);
sys_res arch_BkpWrite(adr_t nStartAdr, u8 *p, size_t nLen);
sys_res arch_BkpRead(adr_t nStartAdr, u8 *p, size_t nLen);




#ifdef __cplusplus
}
#endif


#endif

