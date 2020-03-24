#ifndef __INTFLASH_H__
#define __INTFLASH_H__

#ifdef __cplusplus
extern "C" {
#endif











//External Functions
#define intf_nolockErase(a)					arch_IntfErase(a)
#define intf_nolockProgram(a, p, l)			arch_IntfProgram(a, p, l)

void intf_Init(void);
sys_res intf_Erase(adr_t adr);
sys_res intf_Program(adr_t adr, const void *pData, size_t nLen);



#ifdef __cplusplus
}
#endif

#endif






