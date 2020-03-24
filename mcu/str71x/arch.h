#ifndef __ARCH_ARCH_H__
#define __ARCH_ARCH_H__


//Header Files
#include <71x_lib.h>

#include <mcu/str71x/emi.h>
#include <mcu/str71x/gpio.h>
#include <mcu/str71x/io.h>
#include <mcu/str71x/rtc.h>
#include <mcu/str71x/uart.h>

#include <mcu/str71x/it.h>


//External Functions
void arch_Init(void);
void arch_IdleEntry(void);
void arch_Reset(void);


#endif

