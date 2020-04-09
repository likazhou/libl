


#if (ARCH_TYPE == ARCH_T_STM32F10X_HD) || (ARCH_TYPE == ARCH_T_STM32F10X_MD) || \
	(ARCH_TYPE == ARCH_T_STM32F10X_CL) || (ARCH_TYPE == ARCH_T_STM32F10X_XL) || \
	(ARCH_TYPE == ARCH_T_LM3S5X) || (ARCH_TYPE == ARCH_T_LM3S9X) || \
	(ARCH_TYPE == ARCH_T_LPC176X) || (ARCH_TYPE == ARCH_T_STM32F20X)

#if OS_TYPE == OS_T_RTTHREAD
#include <arch/cm3/port_rtt/cpuport.c>
#endif

#if OS_TYPE == OS_T_CHNIL || OS_TYPE == OS_T_CHRT
#include <arch/cm3/port_ch/chcore.c>
#include <arch/cm3/port_ch/chcore_v7m.c>
#endif

#if OS_TYPE == OS_T_FREERTOS
#if defined(__CC_ARM)   /* ARMCC compiler */
#include <arch/cm3/port_freertos/RVDS/port.c>
#elif defined(__IAR_SYSTEMS_ICC__)   /* IAR Compiler */
#include <arch/cm3/port_freertos/IAR/port.c>
#elif defined(__GNUC__)     /* GNU GCC Compiler */
#include <arch/cm3/port_freertos/GCC/port.c>
#endif
#endif

#elif ARCH_TYPE == ARCH_T_M051X
#include <arch/cm3/port_ch/chcore.c>
#include <arch/cm3/port_ch/chcore_v6m.c>

#endif





#if (ARCH_TYPE == ARCH_T_STM32F10X_HD) || (ARCH_TYPE == ARCH_T_STM32F10X_MD) || \
	(ARCH_TYPE == ARCH_T_STM32F10X_CL) || (ARCH_TYPE == ARCH_T_STM32F10X_XL)

#if defined(__GNUC__) && (__GNUC__ > 4)    /* GNU GCC Compiler */
#if ARCH_TYPE == ARCH_T_STM32F10X_XL
#include <mcu/stm32f10x/startup_stm32f10x_xl.c>
#endif
#if ARCH_TYPE == ARCH_T_STM32F10X_HD
#include <mcu/stm32f10x/startup_stm32f10x_hd.c>
#endif
#if ARCH_TYPE == ARCH_T_STM32F10X_MD
#include <mcu/stm32f10x/startup_stm32f10x_md.c>
#endif
#if ARCH_TYPE == ARCH_T_STM32F10X_CL
#include <mcu/stm32f10x/startup_stm32f10x_cl.c>
#endif
#endif

#include <mcu/stm32f10x/misc.c>
#include <mcu/stm32f10x/stm32f10x_adc.c>
#include <mcu/stm32f10x/stm32f10x_dma.c>
#include <mcu/stm32f10x/stm32f10x_exti.c>
#include <mcu/stm32f10x/stm32f10x_flash.c>
#include <mcu/stm32f10x/stm32f10x_fsmc.c>
#include <mcu/stm32f10x/stm32f10x_gpio.c>
#include <mcu/stm32f10x/stm32f10x_iwdg.c>
#include <mcu/stm32f10x/stm32f10x_pwr.c>
#include <mcu/stm32f10x/stm32f10x_rcc.c>
#include <mcu/stm32f10x/stm32f10x_tim.c>
#include <mcu/stm32f10x/stm32f10x_usart.c>

#ifdef RT_USING_CONSOLE
#include <mcu/stm32f10x/serial.c>
#endif
#if ARCH_TYPE == ARCH_T_STM32F10X_CL
#include <mcu/stm32f10x/system_stm32f10x.c>
#endif
#include <mcu/stm32f10x/arch.c>
#include <mcu/stm32f10x/arch_adc.c>
#include <mcu/stm32f10x/arch_bkp.c>
#include <mcu/stm32f10x/arch_emac.c>
#include <mcu/stm32f10x/arch_flash.c>
#include <mcu/stm32f10x/arch_fsmc.c>
#include <mcu/stm32f10x/arch_gpio.c>
#include <mcu/stm32f10x/arch_i2c.c>
#if ARCH_TYPE == ARCH_T_STM32F10X_CL
#include <mcu/stm32f10x/arch_it_cl.c>
#else
#include <mcu/stm32f10x/arch_it.c>
#endif
#include <mcu/stm32f10x/arch_iwdg.c>
#include <mcu/stm32f10x/arch_nand.c>
#include <mcu/stm32f10x/arch_rtc.c>
#include <mcu/stm32f10x/arch_serial.c>
#include <mcu/stm32f10x/arch_spi.c>
#include <mcu/stm32f10x/arch_timer.c>
#include <mcu/stm32f10x/arch_uart.c>

#elif ARCH_TYPE == ARCH_T_STM32F20X
#include <mcu/stm32f20x/misc.c>
#include <mcu/stm32f20x/stm32f2xx_adc.c>
#include <mcu/stm32f20x/stm32f2xx_dma.c>
#include <mcu/stm32f20x/stm32f2xx_exti.c>
#include <mcu/stm32f20x/stm32f2xx_fsmc.c>
#include <mcu/stm32f20x/stm32f2xx_flash.c>
#include <mcu/stm32f20x/stm32f2xx_gpio.c>
#include <mcu/stm32f20x/stm32f2xx_iwdg.c>
#include <mcu/stm32f20x/stm32f2xx_pwr.c>
#include <mcu/stm32f20x/stm32f2xx_rcc.c>
#include <mcu/stm32f20x/stm32f2xx_syscfg.c>
#include <mcu/stm32f20x/stm32f2xx_tim.c>
#include <mcu/stm32f20x/stm32f2xx_usart.c>
#include <mcu/stm32f20x/stm32f2xx_spi.c>

#ifdef RT_USING_CONSOLE
#include <mcu/stm32f20x/serial.c>
#endif
#include <mcu/stm32f20x/system_stm32f2xx.c>
#include <mcu/stm32f20x/arch.c>
#include <mcu/stm32f20x/arch_adc.c>
#include <mcu/stm32f20x/arch_emac.c>
#include <mcu/stm32f20x/arch_flash.c>
#include <mcu/stm32f20x/arch_fsmc.c>
#include <mcu/stm32f20x/arch_gpio.c>
#include <mcu/stm32f20x/arch_i2c.c>
#include <mcu/stm32f20x/arch_it.c>
#include <mcu/stm32f20x/arch_iwdg.c>
#include <mcu/stm32f20x/arch_nand.c>
#include <mcu/stm32f20x/arch_serial.c>
#include <mcu/stm32f20x/arch_spi.c>
#include <mcu/stm32f20x/arch_timer.c>
#include <mcu/stm32f20x/arch_uart.c>


#elif ARCH_TYPE == ARCH_T_LPC176X
#ifdef RT_USING_CONSOLE
#include <mcu/lpc176x/serial.c>
#endif
#include <mcu/lpc176x/system_LPC17xx.c>
#include <mcu/lpc176x/arch.c>
#include <mcu/lpc176x/arch_emac.c>
#include <mcu/lpc176x/arch_flash.c>
#include <mcu/lpc176x/arch_gpio.c>
#include <mcu/lpc176x/arch_iwdg.c>
#include <mcu/lpc176x/arch_it.c>
#include <mcu/lpc176x/arch_pwm.c>
#include <mcu/lpc176x/arch_serial.c>
#include <mcu/lpc176x/arch_timer.c>
#include <mcu/lpc176x/arch_uart.c>



#elif (ARCH_TYPE == ARCH_T_LM3S9X) || (ARCH_TYPE == ARCH_T_LM3S5X)
#ifdef RT_USING_CONSOLE
#include <mcu/lm3s/serial.c>
#endif
#if 0
#include <mcu/lm3s/driverlib/adc.c>
#include <mcu/lm3s/driverlib/cpu.c>
#include <mcu/lm3s/driverlib/epi.c>
#include <mcu/lm3s/driverlib/ethernet.c>
#include <mcu/lm3s/driverlib/flash.c>
#include <mcu/lm3s/driverlib/gpio.c>
#include <mcu/lm3s/driverlib/interrupt.c>
#include <mcu/lm3s/driverlib/sysctl.c>
#include <mcu/lm3s/driverlib/systick.c>
#include <mcu/lm3s/driverlib/timer.c>
#include <mcu/lm3s/driverlib/uart.c>
#include <mcu/lm3s/driverlib/usb.c>
#include <mcu/lm3s/driverlib/watchdog.c>
#endif
#include <mcu/lm3s/arch.c>
#include <mcu/lm3s/arch_emac.c>
#include <mcu/lm3s/arch_flash.c>
#include <mcu/lm3s/arch_gpio.c>
#include <mcu/lm3s/arch_iwdg.c>
#include <mcu/lm3s/arch_it.c>
#include <mcu/lm3s/arch_pwm.c>
#include <mcu/lm3s/arch_serial.c>
#include <mcu/lm3s/arch_timer.c>
#include <mcu/lm3s/arch_uart.c>



#elif ARCH_TYPE == ARCH_T_M051X
#include <mcu/m051/arch.c>
#include <mcu/m051/arch_gpio.c>
#include <mcu/m051/arch_it.c>
#include <mcu/m051/arch_serial.c>
#include <mcu/m051/arch_timer.c>
#include <mcu/m051/arch_uart.c>







#elif ARCH_TYPE == ARCH_T_STR71X
#include <mcu/str71x/arch.c>
#include <mcu/str71x/arch_emi.c>
#include <mcu/str71x/arch_gpio.c>
#include <mcu/str71x/arch_it.c>
#include <mcu/str71x/arch_rtc.c>
#include <mcu/str71x/arch_serial.c>
#include <mcu/str71x/arch_uart.c>

#elif ARCH_TYPE == ARCH_T_LPC22XX
#if OS_TYPE
#include <mcu/lpc22xx/cpuport.c>
#endif
#include <mcu/lpc22xx/arch.c>
#include <mcu/lpc22xx/arch_flash.c>
#include <mcu/lpc22xx/arch_gpio.c>
#include <mcu/lpc22xx/arch_iwdg.c>
#include <mcu/lpc22xx/arch_it.c>
#include <mcu/lpc22xx/arch_timer.c>
#include <mcu/lpc22xx/arch_uart.c>


#endif



