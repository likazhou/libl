#if INTFLASH_ENABLE
#include <drivers/intflash.h>


//Private Defines
#define INTF_LOCK_ENABLE		0


#if INTF_LOCK_ENABLE
static os_sem_t intf_sem;
#endif


//Internal Functions
#if INTF_LOCK_ENABLE && OS_TYPE
#define intf_Lock()				os_sem_wait(&intf_sem)
#define intf_Unlock()			os_sem_signal(&intf_sem)
#else
#define intf_Lock()
#define intf_Unlock()
#endif



void intf_Init()
{

#if INTF_LOCK_ENABLE
	os_sem_init(&intf_sem, 1);
#endif
	arch_IntfInit();
}

sys_res intf_Erase(adr_t adr)
{
	sys_res res;

	intf_Lock();

	res = arch_IntfErase(adr);

	intf_Unlock();

	return res;
}

sys_res intf_Program(adr_t adr, const void *pData, size_t nLen)
{
	sys_res res;

	intf_Lock();

	res = arch_IntfProgram(adr, pData, nLen);

	intf_Unlock();

	return res;
}


#endif

