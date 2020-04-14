#ifndef __DEF_H__
#define __DEF_H__

#include <stddef.h>

//Archives Defines
#define ARCH_T_STM32F10X_HD	1
#define ARCH_T_STM32F10X_MD	2
#define ARCH_T_STM32F10X_CL	3
#define ARCH_T_STM32F10X_XL	4

#define ARCH_T_STM32F20X	8

#define ARCH_T_STM32F40X	12
#define ARCH_T_STM32F42X	13

#define ARCH_T_LPC176X		16

#define ARCH_T_LM3S5X		18
#define ARCH_T_LM3S9X		19

#define ARCH_T_M051X		22

#define ARCH_T_STR71X		24
#define ARCH_T_LPC22XX		25



//System Speed Defines
#define MCU_SPEED_FULL		1
#define MCU_SPEED_HALF		2
#define MCU_SPEED_LOW		3


//Operation System Type Defines
#define OS_T_NULL			0
#define OS_T_KEILRTX		1
#define OS_T_RTTHREAD		2
#define OS_T_CHNIL			3
#define OS_T_CHRT			4
#define OS_T_FREERTOS		5


//TCP Protocol Type Defines
#define TCPPS_T_NULL		0
#define TCPPS_T_LWIP		1
#define TCPPS_T_KEILTCP		2
#define TCPPS_T_UIP			3
#define TCPPS_T_BDIP		4





//System Return Defines
typedef enum
{
	SYS_R_OK = 0,		/* 0 */
	SYS_R_NO,			/* 1 */
	SYS_R_ERR,			/* 2 */
	SYS_R_TMO,			/* 3 */
	SYS_R_FULL,			/* 4 */
	SYS_R_BUSY,			/* 5 */
	SYS_R_EMPTY,		/* 6 */
	SYS_R_EMEM,			/* 7 */
	SYS_R_RESET,		/* 8 */
}sys_res;





#endif

