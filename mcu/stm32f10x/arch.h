#ifndef __ARCH_ARCH_H__
#define __ARCH_ARCH_H__

#ifdef __cplusplus
extern "C" {
#endif


//Header Files
#if ARCH_TYPE == ARCH_T_STM32F10X_CL
#define USE_USB_OTG_FS
#endif

#define USE_STDPERIPH_DRIVER
#define HSE_VALUE				MCU_HSE_FREQ

#include "stm32f10x.h"


#include <mcu/stm32f10x/arch_adc.h>
#include <mcu/stm32f10x/arch_bkp.h>
#include <mcu/stm32f10x/arch_emac.h>
#include <mcu/stm32f10x/arch_flash.h>
#include <mcu/stm32f10x/arch_fsmc.h>
#include <mcu/stm32f10x/arch_gpio.h>
#include <mcu/stm32f10x/arch_i2c.h>
#include <mcu/stm32f10x/arch_iwdg.h>
#include <mcu/stm32f10x/arch_nand.h>
#include <mcu/stm32f10x/arch_rtc.h>
#include <mcu/stm32f10x/arch_spi.h>
#include <mcu/stm32f10x/arch_timer.h>
#if OS_TYPE
#include <mcu/stm32f10x/arch_uart.h>
#endif
#include <mcu/stm32f10x/arch_it.h>



#if MCU_FREQUENCY == MCU_SPEED_LOW
#define MCU_CLOCK			8000000
#elif MCU_FREQUENCY == MCU_SPEED_HALF
#define MCU_CLOCK			36000000
#else
#if MCU_HSE_ENABLE
#define MCU_CLOCK			72000000
#else
#define MCU_CLOCK			64000000
#endif
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

