#ifndef __ARCH_ARCH_H__
#define __ARCH_ARCH_H__

#ifdef __cplusplus
extern "C" {
#endif


//Header Files
#if ARCH_TYPE == ARCH_T_STM32F20X
#define USE_USB_OTG_FS
#endif

#define USE_STDPERIPH_DRIVER

#include "stm32f2xx_conf.h"
#include "stm32f2xx.h"

#include <mcu/stm32f20x/adc.h>
#include <mcu/stm32f20x/bkp.h>
#include <mcu/stm32f20x/emac.h>
#include <mcu/stm32f20x/fsmc.h>
#include <mcu/stm32f20x/flash.h>
#include <mcu/stm32f20x/gpio.h>
//#include <mcu/stm32f20x/i2c.h>
#include <mcu/stm32f20x/iwdg.h>
#include <mcu/stm32f20x/nand.h>
#include <mcu/stm32f20x/rtc.h>
//#include <mcu/stm32f20x/spi.h>
#include <mcu/stm32f20x/timer.h>
#if OS_TYPE
#include <mcu/stm32f20x/uart.h>
#endif

#include <mcu/stm32f20x/it.h>



#if MCU_FREQUENCY == MCU_SPEED_LOW
#define MCU_CLOCK			30000000
#elif MCU_FREQUENCY == MCU_SPEED_HALF
#define MCU_CLOCK			60000000
#else
#if MCU_HSE_ENABLE
#define MCU_CLOCK			120000000
#else
#define MCU_CLOCK			96000000
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

