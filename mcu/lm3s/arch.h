#ifndef __ARCH_ARCH_H__
#define __ARCH_ARCH_H__

#ifdef __cplusplus
extern "C" {
#endif

//Driverlib Defines
#if ARCH_TYPE == ARCH_T_LM3S5X
#define PART_LM3S5956
#elif ARCH_TYPE == ARCH_T_LM3S9X
#define PART_LM3S9B92
#endif


//#define Using ROM Library
//#define TARGET_IS_DUSTDEVIL_RA0
//#define TARGET_IS_TEMPEST_RB1
//#define TARGET_IS_TEMPEST_RC1
#define TARGET_IS_TEMPEST_RC3
//#define TARGET_IS_TEMPEST_RC5
//#define TARGET_IS_FIRESTORM_RA2

//Header Files
#define DEPRECATED

#include <mcu/lm3s/inc/hw_adc.h>
#include <mcu/lm3s/inc/hw_can.h>
#include <mcu/lm3s/inc/hw_comp.h>
//#include <mcu/lm3s/inc/hw_ethernet.h>
#include <mcu/lm3s/inc/hw_flash.h>
#include <mcu/lm3s/inc/hw_gpio.h>
#include <mcu/lm3s/inc/hw_hibernate.h>
#include <mcu/lm3s/inc/hw_i2c.h>
#include <mcu/lm3s/inc/hw_ints.h>
#include <mcu/lm3s/inc/hw_memmap.h>
#include <mcu/lm3s/inc/hw_nvic.h>
#include <mcu/lm3s/inc/hw_pwm.h>
#include <mcu/lm3s/inc/hw_qei.h>
#include <mcu/lm3s/inc/hw_ssi.h>
#include <mcu/lm3s/inc/hw_sysctl.h>
#include <mcu/lm3s/inc/hw_timer.h>
#include <mcu/lm3s/inc/hw_types.h>
#include <mcu/lm3s/inc/hw_uart.h>
#include <mcu/lm3s/inc/hw_watchdog.h>

#include <mcu/lm3s/driverlib/adc.h>
#include <mcu/lm3s/driverlib/can.h>
#include <mcu/lm3s/driverlib/comp.h>
#include <mcu/lm3s/driverlib/cpu.h>
#include <mcu/lm3s/driverlib/debug.h>
//#include <mcu/lm3s/driverlib/ethernet.h>
#include <mcu/lm3s/driverlib/flash.h>
#include <mcu/lm3s/driverlib/gpio.h>
#include <mcu/lm3s/driverlib/hibernate.h>
#include <mcu/lm3s/driverlib/i2c.h>
#include <mcu/lm3s/driverlib/interrupt.h>
#include <mcu/lm3s/driverlib/pin_map.h>
#include <mcu/lm3s/driverlib/pwm.h>
#include <mcu/lm3s/driverlib/qei.h>
#include <mcu/lm3s/driverlib/ssi.h>
#include <mcu/lm3s/driverlib/sysctl.h>
#include <mcu/lm3s/driverlib/systick.h>
#include <mcu/lm3s/driverlib/timer.h>
#include <mcu/lm3s/driverlib/uart.h>
#include <mcu/lm3s/driverlib/udma.h>
#include <mcu/lm3s/driverlib/usb.h>
#include <mcu/lm3s/driverlib/watchdog.h>

#include <mcu/lm3s/driverlib/rom.h>
#include <mcu/lm3s/driverlib/rom_map.h>

#include <mcu/lm3s/flash.h>
#include <mcu/lm3s/gpio.h>
#include <mcu/lm3s/timer.h>

#if OS_TYPE
#include <mcu/lm3s/uart.h>
#endif

#include <mcu/lm3s/it.h>


#if MCU_FREQUENCY == MCU_SPEED_LOW
#define MCU_CLOCK			16000000
#elif MCU_FREQUENCY == MCU_SPEED_HALF
#define MCU_CLOCK			50000000
#else
#define MCU_CLOCK			80000000
#endif







//External Functions
void arch_Init(void);
void arch_IdleEntry(void);
void arch_Reset(void);



#ifdef __cplusplus
}
#endif


#endif


