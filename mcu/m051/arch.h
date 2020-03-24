#ifndef __ARCH_ARCH_H__
#define __ARCH_ARCH_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <M051Series.h>
//#include <mcu/m051/M051Series.h>

#include <mcu/m051/gpio.h>
#include <mcu/m051/timer.h>
#include <mcu/m051/uart.h>

#include <mcu/m051/it.h>


/* Oscillator frequency */
#define XTAL				(12000000UL)

/* System clock */
#if MCU_FREQUENCY == MCU_SPEED_LOW
#error "not support low speed yet"
#elif MCU_FREQUENCY == MCU_SPEED_HALF
#error "not support half speed yet"
#else
#define MCU_CLOCK			(XTAL * 4)
#endif







//External Functions
void arch_Init(void);
void arch_IdleEntry(void);
void arch_Reset(void);



#ifdef __cplusplus
}
#endif


#endif



