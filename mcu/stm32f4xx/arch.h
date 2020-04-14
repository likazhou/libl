#ifndef __ARCH_ARCH_H__
#define __ARCH_ARCH_H__

#ifdef __cplusplus
extern "C" {
#endif


//Header Files
#define USE_FULL_LL_DRIVER
#define HSE_VALUE				MCU_HSE_FREQ

#include "stm32f4xx.h"

#include <mcu/stm32f4xx/stm32f4xx_ll_iwdg.h>

#include <mcu/stm32f4xx/arch_gpio.h>
#include <mcu/stm32f4xx/arch_iwdg.h>
#include <mcu/stm32f4xx/arch_spi.h>
#if OS_TYPE
#include <mcu/stm32f4xx/arch_uart.h>
#endif



#if MCU_FREQUENCY == MCU_SPEED_LOW
#define MCU_CLOCK			25000000
#elif MCU_FREQUENCY == MCU_SPEED_HALF
#define MCU_CLOCK			84000000
#else
#define MCU_CLOCK			168000000
#endif







//External Functions
void arch_Init(void);
void arch_IrqPri(int chl, int pri);
void arch_IdleEntry(void);
void arch_Reset(void);



#ifdef __cplusplus
}
#endif


#endif


