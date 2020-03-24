#ifndef __ARCH_ARCH_H__
#define __ARCH_ARCH_H__

#ifdef __cplusplus
extern "C" {
#endif


//Header Files
#include <mcu/lpc22xx/LPC22xx.H>

#include <mcu/lpc22xx/gpio.h>
#include <mcu/lpc22xx/timer.h>

#if OS_TYPE
#include <mcu/lpc22xx/uart.h>
#endif

#include <mcu/lpc22xx/it.h>


#if MCU_FREQUENCY == MCU_SPEED_LOW
#error "not support low speed yet"
#elif MCU_FREQUENCY == MCU_SPEED_HALF
#error "not support half speed yet"
#else
#define MCU_CLOCK			__CORE_CLK
#endif

#define __CORE_CLK			(11059200 * 5)			//系统频率，必须为OSC_CLK的整数倍(1~32)，且<=60MHZ
#define FCCO_CLK			(__CORE_CLK * 2)		//CCO频率，必须为__CORE_CLK的2、4、8、16倍，范围为156MHz~320MHz
#define PERI_CLOCK			(__CORE_CLK / 4)		//VPB时钟频率，只能为(__CORE_CLK / 4)的1 ~ 4倍




//Public Defines
#define __I		volatile const
#define __O		volatile
#define __IO	volatile


#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/*------------- General Purpose Input/Output (GPIO) --------------------------*/
typedef struct
{
	__I unsigned long FIOPIN;
	__O unsigned long FIOSET;
	__O unsigned long FIODIR;
	__O unsigned long FIOCLR;
} LPC_GPIO_TypeDef;

 
#define LPC_GPIO0             ((LPC_GPIO_TypeDef      *) 0xE0028000     )
#define LPC_GPIO1             ((LPC_GPIO_TypeDef      *) 0xE0028010     )
#define LPC_GPIO2             ((LPC_GPIO_TypeDef      *) 0xE0028020     )
#define LPC_GPIO3             ((LPC_GPIO_TypeDef      *) 0xE0028030     )



/*------------- Timer (TIM) --------------------------------------------------*/
typedef struct
{
	__IO unsigned long IR;
	__IO unsigned long TCR;
	__IO unsigned long TC;
	__IO unsigned long PR;
	__IO unsigned long PC;
	__IO unsigned long MCR;
	__IO unsigned long MR0;
	__IO unsigned long MR1;
	__IO unsigned long MR2;
	__IO unsigned long MR3;
	__IO unsigned long CCR0;
	__I  unsigned long CR0;
	__I  unsigned long CR1;
	__I  unsigned long CR2;
	__I  unsigned long CR3;
	__IO unsigned long EMR;
} LPC_TIM_TypeDef;
 

#define LPC_TIM0              ((LPC_TIM_TypeDef       *) 0xE0004000     )
#define LPC_TIM1              ((LPC_TIM_TypeDef       *) 0xE0008000     )
 


/*------------- Universal Asynchronous Receiver Transmitter (UART) -----------*/
typedef struct
{
  union {
  __I  unsigned char RBR;
  __O  unsigned char THR;
  __IO unsigned char DLL;
       unsigned long RESERVED0;
  };
  union {
  __IO unsigned char DLM;
  __IO unsigned long IER;
  };
  union {
  __I  unsigned long IIR;
  __O  unsigned char FCR;
  };
  __IO unsigned char LCR;
       unsigned char RESERVED1[3];
  __IO unsigned char MCR;
       unsigned char RESERVED2[3];
  __I  unsigned char LSR;
       unsigned char RESERVED3[3];
  __I  unsigned char MSR;
       unsigned char RESERVED4[3];
  __IO unsigned char SCR;
       unsigned char RESERVED5[3];
  __IO unsigned long ACR;
       unsigned long RESERVED6;
  __IO unsigned long FDR;
       unsigned long RESERVED7;
  __IO unsigned char TER;
       unsigned char RESERVED8[3];
} LPC_UART_TypeDef;


#define LPC_UART0             ((LPC_UART_TypeDef     *) 0xE000C000     )
#define LPC_UART1             ((LPC_UART_TypeDef     *) 0xE0010000     )


/*------------- Watchdog Timer (WDT) -----------------------------------------*/
typedef struct
{
  __IO unsigned char WDMOD;
       unsigned char RESERVED0[3];
  __IO unsigned long WDTC;
  __O  unsigned char WDFEED;
       unsigned char RESERVED1[3];
  __I  unsigned long WDTV;
} LPC_WDT_TypeDef;


#define LPC_WDT               ((LPC_WDT_TypeDef      *) 0xE0000000     )




#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif



/*------------- Interrupt (IRQ) -------------------------------------------------*/
#define VIC_BASE_ADDR	0xFFFFF000

enum LPC22xx_INT
{
	WDT_INT	= 0,
	SW_INT_reserved,
	DbgCommRx_INT,
	DbgCommTx_INT,
	TIMER0_INT,
	TIMER1_INT,
	UART0_INT,
	UART1_INT,
	PWM0_INT,
	I2C0_INT,
	SP0_INT,
	SP1_INT,
	PLL_INT,
	RTC_INT,
	EINT0_INT,
	EINT1_INT,
	EINT2_INT,
	EINT3_INT,
	ADC0_INT,
	I2C1_INT,
	BOD_INT,
	ADC1_INT,
	USB_INT
};




//External Functions
void arch_Init(void);
void arch_IdleEntry(void);
void arch_Reset(void);



#ifdef __cplusplus
}
#endif


#endif


