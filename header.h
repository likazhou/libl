#ifndef __HEADER_H__
#define __HEADER_H__



#ifdef __cplusplus
extern "C" {
#endif



//Include HeaderFiles
#if (ARCH_TYPE == ARCH_T_STM32F10X_HD) || (ARCH_TYPE == ARCH_T_STM32F10X_MD) || \
	(ARCH_TYPE == ARCH_T_STM32F10X_CL) || (ARCH_TYPE == ARCH_T_STM32F10X_XL) || \
	(ARCH_TYPE == ARCH_T_STM32F20X) || \
	(ARCH_TYPE == ARCH_T_LM3S9X) || (ARCH_TYPE == ARCH_T_LM3S5X) || \
	(ARCH_TYPE == ARCH_T_LPC176X) || (ARCH_TYPE == ARCH_T_M051X)

#include <arch/cmx/typedef.h>
#endif


#if (ARCH_TYPE == ARCH_T_STR71X) || \
	(ARCH_TYPE == ARCH_T_LPC22XX)

#include <arch/arm7/typedef.h>
#endif


#include <sys/system.h>
	

#if (ARCH_TYPE == ARCH_T_STM32F10X_HD) || (ARCH_TYPE == ARCH_T_STM32F10X_MD) || \
	(ARCH_TYPE == ARCH_T_STM32F10X_CL) || (ARCH_TYPE == ARCH_T_STM32F10X_XL)
#include <mcu/stm32f10x/arch.h>

#elif (ARCH_TYPE == ARCH_T_STM32F20X) 
#include <mcu/stm32f20x/arch.h>

#elif (ARCH_TYPE == ARCH_T_LM3S9X) || (ARCH_TYPE == ARCH_T_LM3S5X)
#include <mcu/lm3s/arch.h>

#elif ARCH_TYPE == ARCH_T_LPC176X
#include <mcu/lpc176x/arch.h>

#elif ARCH_TYPE == ARCH_T_M051X
#include <mcu/m051/arch.h>

#elif ARCH_TYPE == ARCH_T_STR71X
#include <mcu/str71x/arch.h>

#elif ARCH_TYPE == ARCH_T_LPC22XX
#include <mcu/lpc22xx/arch.h>

#endif

#if OS_TYPE
#include <os/os.h>
#endif



#ifdef __cplusplus
}
#endif


#endif


