/**************************************************************************//**
 * @file     LPC17xx.h
 * @brief    CMSIS Cortex-M3 Core Peripheral Access Layer Header File for 
 *           NXP LPC17xx Device Series
 * @version  V1.07
 * @date     19. October 2009
 *
 * @note
 * Copyright (C) 2009 ARM Limited. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M 
 * processor based microcontrollers.  This file can be freely distributed 
 * within development tools that are supporting such ARM based processors. 
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/


#ifndef __LPC17xx_H__
#define __LPC17xx_H__

/*
 * ==========================================================================
 * ---------- Interrupt Number Definition -----------------------------------
 * ==========================================================================
 */

typedef enum IRQn
{
/******  Cortex-M3 Processor Exceptions Numbers ***************************************************/
  NonMaskableInt_IRQn           = -14,      /*!< 2 Non Maskable Interrupt                         */
  MemoryManagement_IRQn         = -12,      /*!< 4 Cortex-M3 Memory Management Interrupt          */
  BusFault_IRQn                 = -11,      /*!< 5 Cortex-M3 Bus Fault Interrupt                  */
  UsageFault_IRQn               = -10,      /*!< 6 Cortex-M3 Usage Fault Interrupt                */
  SVCall_IRQn                   = -5,       /*!< 11 Cortex-M3 SV Call Interrupt                   */
  DebugMonitor_IRQn             = -4,       /*!< 12 Cortex-M3 Debug Monitor Interrupt             */
  PendSV_IRQn                   = -2,       /*!< 14 Cortex-M3 Pend SV Interrupt                   */
  SysTick_IRQn                  = -1,       /*!< 15 Cortex-M3 System Tick Interrupt               */

/******  LPC17xx Specific Interrupt Numbers *******************************************************/
  WDT_IRQn                      = 0,        /*!< Watchdog Timer Interrupt                         */
  TIMER0_IRQn                   = 1,        /*!< Timer0 Interrupt                                 */
  TIMER1_IRQn                   = 2,        /*!< Timer1 Interrupt                                 */
  TIMER2_IRQn                   = 3,        /*!< Timer2 Interrupt                                 */
  TIMER3_IRQn                   = 4,        /*!< Timer3 Interrupt                                 */
  UART0_IRQn                    = 5,        /*!< UART0 Interrupt                                  */
  UART1_IRQn                    = 6,        /*!< UART1 Interrupt                                  */
  UART2_IRQn                    = 7,        /*!< UART2 Interrupt                                  */
  UART3_IRQn                    = 8,        /*!< UART3 Interrupt                                  */
  PWM1_IRQn                     = 9,        /*!< PWM1 Interrupt                                   */
  I2C0_IRQn                     = 10,       /*!< I2C0 Interrupt                                   */
  I2C1_IRQn                     = 11,       /*!< I2C1 Interrupt                                   */
  I2C2_IRQn                     = 12,       /*!< I2C2 Interrupt                                   */
  SPI_IRQn                      = 13,       /*!< SPI Interrupt                                    */
  SSP0_IRQn                     = 14,       /*!< SSP0 Interrupt                                   */
  SSP1_IRQn                     = 15,       /*!< SSP1 Interrupt                                   */
  PLL0_IRQn                     = 16,       /*!< PLL0 Lock (Main PLL) Interrupt                   */
  RTC_IRQn                      = 17,       /*!< Real Time Clock Interrupt                        */
  EINT0_IRQn                    = 18,       /*!< External Interrupt 0 Interrupt                   */
  EINT1_IRQn                    = 19,       /*!< External Interrupt 1 Interrupt                   */
  EINT2_IRQn                    = 20,       /*!< External Interrupt 2 Interrupt                   */
  EINT3_IRQn                    = 21,       /*!< External Interrupt 3 Interrupt                   */
  ADC_IRQn                      = 22,       /*!< A/D Converter Interrupt                          */
  BOD_IRQn                      = 23,       /*!< Brown-Out Detect Interrupt                       */
  USB_IRQn                      = 24,       /*!< USB Interrupt                                    */
  CAN_IRQn                      = 25,       /*!< CAN Interrupt                                    */
  DMA_IRQn                      = 26,       /*!< General Purpose DMA Interrupt                    */
  I2S_IRQn                      = 27,       /*!< I2S Interrupt                                    */
  ENET_IRQn                     = 28,       /*!< Ethernet Interrupt                               */
  RIT_IRQn                      = 29,       /*!< Repetitive Interrupt Timer Interrupt             */
  MCPWM_IRQn                    = 30,       /*!< Motor Control PWM Interrupt                      */
  QEI_IRQn                      = 31,       /*!< Quadrature Encoder Interface Interrupt           */
  PLL1_IRQn                     = 32,       /*!< PLL1 Lock (USB PLL) Interrupt                    */
} IRQn_Type;


/*
 * ==========================================================================
 * ----------- Processor and Core Peripheral Section ------------------------
 * ==========================================================================
 */

/* Configuration of the Cortex-M3 Processor and Core Peripherals */
#define __MPU_PRESENT             1         /*!< MPU present or not                               */
#define __NVIC_PRIO_BITS          5         /*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used     */


#include "core_cm3.h"                       /* Cortex-M3 processor and core peripherals           */
#include "system_LPC17xx.h"                 /* System Header                                      */


/******************************************************************************/
/*                Device Specific Peripheral registers structures             */
/******************************************************************************/

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/*------------- System Control (SC) ------------------------------------------*/
typedef struct
{
  __IO u32 FLASHCFG;               /* Flash Accelerator Module           */
       u32 RESERVED0[31];
  __IO u32 PLL0CON;                /* Clocking and Power Control         */
  __IO u32 PLL0CFG;
  __I  u32 PLL0STAT;
  __O  u32 PLL0FEED;
       u32 RESERVED1[4];
  __IO u32 PLL1CON;
  __IO u32 PLL1CFG;
  __I  u32 PLL1STAT;
  __O  u32 PLL1FEED;
       u32 RESERVED2[4];
  __IO u32 PCON;
  __IO u32 PCONP;
       u32 RESERVED3[15];
  __IO u32 CCLKCFG;
  __IO u32 USBCLKCFG;
  __IO u32 CLKSRCSEL;
       u32 RESERVED4[12];
  __IO u32 EXTINT;                 /* External Interrupts                */
       u32 RESERVED5;
  __IO u32 EXTMODE;
  __IO u32 EXTPOLAR;
       u32 RESERVED6[12];
  __IO u32 RSID;                   /* Reset                              */
       u32 RESERVED7[7];
  __IO u32 SCS;                    /* Syscon Miscellaneous Registers     */
  __IO u32 IRCTRIM;                /* Clock Dividers                     */
  __IO u32 PCLKSEL0;
  __IO u32 PCLKSEL1;
       u32 RESERVED8[4];
  __IO u32 USBIntSt;               /* USB Device/OTG Interrupt Register  */
  __IO u32 DMAREQSEL;
  __IO u32 CLKOUTCFG;              /* Clock Output Configuration         */
 } LPC_SC_TypeDef;

/*------------- Pin Connect Block (PINCON) -----------------------------------*/
typedef struct
{
  __IO u32 PINSEL0;
  __IO u32 PINSEL1;
  __IO u32 PINSEL2;
  __IO u32 PINSEL3;
  __IO u32 PINSEL4;
  __IO u32 PINSEL5;
  __IO u32 PINSEL6;
  __IO u32 PINSEL7;
  __IO u32 PINSEL8;
  __IO u32 PINSEL9;
  __IO u32 PINSEL10;
       u32 RESERVED0[5];
  __IO u32 PINMODE0;
  __IO u32 PINMODE1;
  __IO u32 PINMODE2;
  __IO u32 PINMODE3;
  __IO u32 PINMODE4;
  __IO u32 PINMODE5;
  __IO u32 PINMODE6;
  __IO u32 PINMODE7;
  __IO u32 PINMODE8;
  __IO u32 PINMODE9;
  __IO u32 PINMODE_OD0;
  __IO u32 PINMODE_OD1;
  __IO u32 PINMODE_OD2;
  __IO u32 PINMODE_OD3;
  __IO u32 PINMODE_OD4;
  __IO u32 I2CPADCFG;
} LPC_PINCON_TypeDef;

/*------------- General Purpose Input/Output (GPIO) --------------------------*/
typedef struct
{
  union {
    __IO u32 FIODIR;
    struct {
      __IO uint16_t FIODIRL;
      __IO uint16_t FIODIRH;
    };
    struct {
      __IO u8  FIODIR0;
      __IO u8  FIODIR1;
      __IO u8  FIODIR2;
      __IO u8  FIODIR3;
    };
  };
  u32 RESERVED0[3];
  union {
    __IO u32 FIOMASK;
    struct {
      __IO uint16_t FIOMASKL;
      __IO uint16_t FIOMASKH;
    };
    struct {
      __IO u8  FIOMASK0;
      __IO u8  FIOMASK1;
      __IO u8  FIOMASK2;
      __IO u8  FIOMASK3;
    };
  };
  union {
    __IO u32 FIOPIN;
    struct {
      __IO uint16_t FIOPINL;
      __IO uint16_t FIOPINH;
    };
    struct {
      __IO u8  FIOPIN0;
      __IO u8  FIOPIN1;
      __IO u8  FIOPIN2;
      __IO u8  FIOPIN3;
    };
  };
  union {
    __IO u32 FIOSET;
    struct {
      __IO uint16_t FIOSETL;
      __IO uint16_t FIOSETH;
    };
    struct {
      __IO u8  FIOSET0;
      __IO u8  FIOSET1;
      __IO u8  FIOSET2;
      __IO u8  FIOSET3;
    };
  };
  union {
    __O  u32 FIOCLR;
    struct {
      __O  uint16_t FIOCLRL;
      __O  uint16_t FIOCLRH;
    };
    struct {
      __O  u8  FIOCLR0;
      __O  u8  FIOCLR1;
      __O  u8  FIOCLR2;
      __O  u8  FIOCLR3;
    };
  };
} LPC_GPIO_TypeDef;

typedef struct
{
  __I  u32 IntStatus;
  __I  u32 IO0IntStatR;
  __I  u32 IO0IntStatF;
  __O  u32 IO0IntClr;
  __IO u32 IO0IntEnR;
  __IO u32 IO0IntEnF;
       u32 RESERVED0[3];
  __I  u32 IO2IntStatR;
  __I  u32 IO2IntStatF;
  __O  u32 IO2IntClr;
  __IO u32 IO2IntEnR;
  __IO u32 IO2IntEnF;
} LPC_GPIOINT_TypeDef;

/*------------- Timer (TIM) --------------------------------------------------*/
typedef struct
{
  __IO u32 IR;
  __IO u32 TCR;
  __IO u32 TC;
  __IO u32 PR;
  __IO u32 PC;
  __IO u32 MCR;
  __IO u32 MR0;
  __IO u32 MR1;
  __IO u32 MR2;
  __IO u32 MR3;
  __IO u32 CCR;
  __I  u32 CR0;
  __I  u32 CR1;
       u32 RESERVED0[2];
  __IO u32 EMR;
       u32 RESERVED1[12];
  __IO u32 CTCR;
} LPC_TIM_TypeDef;

/*------------- Pulse-Width Modulation (PWM) ---------------------------------*/
typedef struct
{
  __IO u32 IR;
  __IO u32 TCR;
  __IO u32 TC;
  __IO u32 PR;
  __IO u32 PC;
  __IO u32 MCR;
  __IO u32 MR0;
  __IO u32 MR1;
  __IO u32 MR2;
  __IO u32 MR3;
  __IO u32 CCR;
  __I  u32 CR0;
  __I  u32 CR1;
  __I  u32 CR2;
  __I  u32 CR3;
       u32 RESERVED0;
  __IO u32 MR4;
  __IO u32 MR5;
  __IO u32 MR6;
  __IO u32 PCR;
  __IO u32 LER;
       u32 RESERVED1[7];
  __IO u32 CTCR;
} LPC_PWM_TypeDef;

/*------------- Universal Asynchronous Receiver Transmitter (UART) -----------*/
typedef struct
{
  union {
  __I  u8  RBR;
  __O  u8  THR;
  __IO u8  DLL;
       u32 RESERVED0;
  };
  union {
  __IO u8  DLM;
  __IO u32 IER;
  };
  union {
  __I  u32 IIR;
  __O  u8  FCR;
  };
  __IO u8  LCR;
       u8  RESERVED1[7];
  __I  u8  LSR;
       u8  RESERVED2[7];
  __IO u8  SCR;
       u8  RESERVED3[3];
  __IO u32 ACR;
  __IO u8  ICR;
       u8  RESERVED4[3];
  __IO u8  FDR;
       u8  RESERVED5[7];
  __IO u8  TER;
       u8  RESERVED6[39];
  __I  u8  FIFOLVL;
} LPC_UART_TypeDef;

typedef struct
{
  union {
  __I  u8  RBR;
  __O  u8  THR;
  __IO u8  DLL;
       u32 RESERVED0;
  };
  union {
  __IO u8  DLM;
  __IO u32 IER;
  };
  union {
  __I  u32 IIR;
  __O  u8  FCR;
  };
  __IO u8  LCR;
       u8  RESERVED1[7];
  __I  u8  LSR;
       u8  RESERVED2[7];
  __IO u8  SCR;
       u8  RESERVED3[3];
  __IO u32 ACR;
  __IO u8  ICR;
       u8  RESERVED4[3];
  __IO u8  FDR;
       u8  RESERVED5[7];
  __IO u8  TER;
       u8  RESERVED6[39];
  __I  u8  FIFOLVL;
} LPC_UART0_TypeDef;

typedef struct
{
  union {
  __I  u8  RBR;
  __O  u8  THR;
  __IO u8  DLL;
       u32 RESERVED0;
  };
  union {
  __IO u8  DLM;
  __IO u32 IER;
  };
  union {
  __I  u32 IIR;
  __O  u8  FCR;
  };
  __IO u8  LCR;
       u8  RESERVED1[3];
  __IO u8  MCR;
       u8  RESERVED2[3];
  __I  u8  LSR;
       u8  RESERVED3[3];
  __I  u8  MSR;
       u8  RESERVED4[3];
  __IO u8  SCR;
       u8  RESERVED5[3];
  __IO u32 ACR;
       u32 RESERVED6;
  __IO u32 FDR;
       u32 RESERVED7;
  __IO u8  TER;
       u8  RESERVED8[27];
  __IO u8  RS485CTRL;
       u8  RESERVED9[3];
  __IO u8  ADRMATCH;
       u8  RESERVED10[3];
  __IO u8  RS485DLY;
       u8  RESERVED11[3];
  __I  u8  FIFOLVL;
} LPC_UART1_TypeDef;

/*------------- Serial Peripheral Interface (SPI) ----------------------------*/
typedef struct
{
  __IO u32 SPCR;
  __I  u32 SPSR;
  __IO u32 SPDR;
  __IO u32 SPCCR;
       u32 RESERVED0[3];
  __IO u32 SPINT;
} LPC_SPI_TypeDef;

/*------------- Synchronous Serial Communication (SSP) -----------------------*/
typedef struct
{
  __IO u32 CR0;
  __IO u32 CR1;
  __IO u32 DR;
  __I  u32 SR;
  __IO u32 CPSR;
  __IO u32 IMSC;
  __IO u32 RIS;
  __IO u32 MIS;
  __IO u32 ICR;
  __IO u32 DMACR;
} LPC_SSP_TypeDef;

/*------------- Inter-Integrated Circuit (I2C) -------------------------------*/
typedef struct
{
  __IO u32 I2CONSET;
  __I  u32 I2STAT;
  __IO u32 I2DAT;
  __IO u32 I2ADR0;
  __IO u32 I2SCLH;
  __IO u32 I2SCLL;
  __O  u32 I2CONCLR;
  __IO u32 MMCTRL;
  __IO u32 I2ADR1;
  __IO u32 I2ADR2;
  __IO u32 I2ADR3;
  __I  u32 I2DATA_BUFFER;
  __IO u32 I2MASK0;
  __IO u32 I2MASK1;
  __IO u32 I2MASK2;
  __IO u32 I2MASK3;
} LPC_I2C_TypeDef;

/*------------- Inter IC Sound (I2S) -----------------------------------------*/
typedef struct
{
  __IO u32 I2SDAO;
  __IO u32 I2SDAI;
  __O  u32 I2STXFIFO;
  __I  u32 I2SRXFIFO;
  __I  u32 I2SSTATE;
  __IO u32 I2SDMA1;
  __IO u32 I2SDMA2;
  __IO u32 I2SIRQ;
  __IO u32 I2STXRATE;
  __IO u32 I2SRXRATE;
  __IO u32 I2STXBITRATE;
  __IO u32 I2SRXBITRATE;
  __IO u32 I2STXMODE;
  __IO u32 I2SRXMODE;
} LPC_I2S_TypeDef;

/*------------- Repetitive Interrupt Timer (RIT) -----------------------------*/
typedef struct
{
  __IO u32 RICOMPVAL;
  __IO u32 RIMASK;
  __IO u8  RICTRL;
       u8  RESERVED0[3];
  __IO u32 RICOUNTER;
} LPC_RIT_TypeDef;

/*------------- Real-Time Clock (RTC) ----------------------------------------*/
typedef struct
{
  __IO u8  ILR;
       u8  RESERVED0[7];
  __IO u8  CCR;
       u8  RESERVED1[3];
  __IO u8  CIIR;
       u8  RESERVED2[3];
  __IO u8  AMR;
       u8  RESERVED3[3];
  __I  u32 CTIME0;
  __I  u32 CTIME1;
  __I  u32 CTIME2;
  __IO u8  SEC;
       u8  RESERVED4[3];
  __IO u8  MIN;
       u8  RESERVED5[3];
  __IO u8  HOUR;
       u8  RESERVED6[3];
  __IO u8  DOM;
       u8  RESERVED7[3];
  __IO u8  DOW;
       u8  RESERVED8[3];
  __IO uint16_t DOY;
       uint16_t RESERVED9;
  __IO u8  MONTH;
       u8  RESERVED10[3];
  __IO uint16_t YEAR;
       uint16_t RESERVED11;
  __IO u32 CALIBRATION;
  __IO u32 GPREG0;
  __IO u32 GPREG1;
  __IO u32 GPREG2;
  __IO u32 GPREG3;
  __IO u32 GPREG4;
  __IO u8  RTC_AUXEN;
       u8  RESERVED12[3];
  __IO u8  RTC_AUX;
       u8  RESERVED13[3];
  __IO u8  ALSEC;
       u8  RESERVED14[3];
  __IO u8  ALMIN;
       u8  RESERVED15[3];
  __IO u8  ALHOUR;
       u8  RESERVED16[3];
  __IO u8  ALDOM;
       u8  RESERVED17[3];
  __IO u8  ALDOW;
       u8  RESERVED18[3];
  __IO uint16_t ALDOY;
       uint16_t RESERVED19;
  __IO u8  ALMON;
       u8  RESERVED20[3];
  __IO uint16_t ALYEAR;
       uint16_t RESERVED21;
} LPC_RTC_TypeDef;

/*------------- Watchdog Timer (WDT) -----------------------------------------*/
typedef struct
{
  __IO u8  WDMOD;
       u8  RESERVED0[3];
  __IO u32 WDTC;
  __O  u8  WDFEED;
       u8  RESERVED1[3];
  __I  u32 WDTV;
  __IO u32 WDCLKSEL;
} LPC_WDT_TypeDef;

/*------------- Analog-to-Digital Converter (ADC) ----------------------------*/
typedef struct
{
  __IO u32 ADCR;
  __IO u32 ADGDR;
       u32 RESERVED0;
  __IO u32 ADINTEN;
  __I  u32 ADDR0;
  __I  u32 ADDR1;
  __I  u32 ADDR2;
  __I  u32 ADDR3;
  __I  u32 ADDR4;
  __I  u32 ADDR5;
  __I  u32 ADDR6;
  __I  u32 ADDR7;
  __I  u32 ADSTAT;
  __IO u32 ADTRM;
} LPC_ADC_TypeDef;

/*------------- Digital-to-Analog Converter (DAC) ----------------------------*/
typedef struct
{
  __IO u32 DACR;
  __IO u32 DACCTRL;
  __IO uint16_t DACCNTVAL;
} LPC_DAC_TypeDef;

/*------------- Motor Control Pulse-Width Modulation (MCPWM) -----------------*/
typedef struct
{
  __I  u32 MCCON;
  __O  u32 MCCON_SET;
  __O  u32 MCCON_CLR;
  __I  u32 MCCAPCON;
  __O  u32 MCCAPCON_SET;
  __O  u32 MCCAPCON_CLR;
  __IO u32 MCTIM0;
  __IO u32 MCTIM1;
  __IO u32 MCTIM2;
  __IO u32 MCPER0;
  __IO u32 MCPER1;
  __IO u32 MCPER2;
  __IO u32 MCPW0;
  __IO u32 MCPW1;
  __IO u32 MCPW2;
  __IO u32 MCDEADTIME;
  __IO u32 MCCCP;
  __IO u32 MCCR0;
  __IO u32 MCCR1;
  __IO u32 MCCR2;
  __I  u32 MCINTEN;
  __O  u32 MCINTEN_SET;
  __O  u32 MCINTEN_CLR;
  __I  u32 MCCNTCON;
  __O  u32 MCCNTCON_SET;
  __O  u32 MCCNTCON_CLR;
  __I  u32 MCINTFLAG;
  __O  u32 MCINTFLAG_SET;
  __O  u32 MCINTFLAG_CLR;
  __O  u32 MCCAP_CLR;
} LPC_MCPWM_TypeDef;

/*------------- Quadrature Encoder Interface (QEI) ---------------------------*/
typedef struct
{
  __O  u32 QEICON;
  __I  u32 QEISTAT;
  __IO u32 QEICONF;
  __I  u32 QEIPOS;
  __IO u32 QEIMAXPOS;
  __IO u32 CMPOS0;
  __IO u32 CMPOS1;
  __IO u32 CMPOS2;
  __I  u32 INXCNT;
  __IO u32 INXCMP;
  __IO u32 QEILOAD;
  __I  u32 QEITIME;
  __I  u32 QEIVEL;
  __I  u32 QEICAP;
  __IO u32 VELCOMP;
  __IO u32 FILTER;
       u32 RESERVED0[998];
  __O  u32 QEIIEC;
  __O  u32 QEIIES;
  __I  u32 QEIINTSTAT;
  __I  u32 QEIIE;
  __O  u32 QEICLR;
  __O  u32 QEISET;
} LPC_QEI_TypeDef;

/*------------- Controller Area Network (CAN) --------------------------------*/
typedef struct
{
  __IO u32 mask[512];              /* ID Masks                           */
} LPC_CANAF_RAM_TypeDef;

typedef struct                          /* Acceptance Filter Registers        */
{
  __IO u32 AFMR;
  __IO u32 SFF_sa;
  __IO u32 SFF_GRP_sa;
  __IO u32 EFF_sa;
  __IO u32 EFF_GRP_sa;
  __IO u32 ENDofTable;
  __I  u32 LUTerrAd;
  __I  u32 LUTerr;
  __IO u32 FCANIE;
  __IO u32 FCANIC0;
  __IO u32 FCANIC1;
} LPC_CANAF_TypeDef;

typedef struct                          /* Central Registers                  */
{
  __I  u32 CANTxSR;
  __I  u32 CANRxSR;
  __I  u32 CANMSR;
} LPC_CANCR_TypeDef;

typedef struct                          /* Controller Registers               */
{
  __IO u32 MOD;
  __O  u32 CMR;
  __IO u32 GSR;
  __I  u32 ICR;
  __IO u32 IER;
  __IO u32 BTR;
  __IO u32 EWL;
  __I  u32 SR;
  __IO u32 RFS;
  __IO u32 RID;
  __IO u32 RDA;
  __IO u32 RDB;
  __IO u32 TFI1;
  __IO u32 TID1;
  __IO u32 TDA1;
  __IO u32 TDB1;
  __IO u32 TFI2;
  __IO u32 TID2;
  __IO u32 TDA2;
  __IO u32 TDB2;
  __IO u32 TFI3;
  __IO u32 TID3;
  __IO u32 TDA3;
  __IO u32 TDB3;
} LPC_CAN_TypeDef;

/*------------- General Purpose Direct Memory Access (GPDMA) -----------------*/
typedef struct                          /* Common Registers                   */
{
  __I  u32 DMACIntStat;
  __I  u32 DMACIntTCStat;
  __O  u32 DMACIntTCClear;
  __I  u32 DMACIntErrStat;
  __O  u32 DMACIntErrClr;
  __I  u32 DMACRawIntTCStat;
  __I  u32 DMACRawIntErrStat;
  __I  u32 DMACEnbldChns;
  __IO u32 DMACSoftBReq;
  __IO u32 DMACSoftSReq;
  __IO u32 DMACSoftLBReq;
  __IO u32 DMACSoftLSReq;
  __IO u32 DMACConfig;
  __IO u32 DMACSync;
} LPC_GPDMA_TypeDef;

typedef struct                          /* Channel Registers                  */
{
  __IO u32 DMACCSrcAddr;
  __IO u32 DMACCDestAddr;
  __IO u32 DMACCLLI;
  __IO u32 DMACCControl;
  __IO u32 DMACCConfig;
} LPC_GPDMACH_TypeDef;

/*------------- Universal Serial Bus (USB) -----------------------------------*/
typedef struct
{
  __I  u32 HcRevision;             /* USB Host Registers                 */
  __IO u32 HcControl;
  __IO u32 HcCommandStatus;
  __IO u32 HcInterruptStatus;
  __IO u32 HcInterruptEnable;
  __IO u32 HcInterruptDisable;
  __IO u32 HcHCCA;
  __I  u32 HcPeriodCurrentED;
  __IO u32 HcControlHeadED;
  __IO u32 HcControlCurrentED;
  __IO u32 HcBulkHeadED;
  __IO u32 HcBulkCurrentED;
  __I  u32 HcDoneHead;
  __IO u32 HcFmInterval;
  __I  u32 HcFmRemaining;
  __I  u32 HcFmNumber;
  __IO u32 HcPeriodicStart;
  __IO u32 HcLSTreshold;
  __IO u32 HcRhDescriptorA;
  __IO u32 HcRhDescriptorB;
  __IO u32 HcRhStatus;
  __IO u32 HcRhPortStatus1;
  __IO u32 HcRhPortStatus2;
       u32 RESERVED0[40];
  __I  u32 Module_ID;

  __I  u32 OTGIntSt;               /* USB On-The-Go Registers            */
  __IO u32 OTGIntEn;
  __O  u32 OTGIntSet;
  __O  u32 OTGIntClr;
  __IO u32 OTGStCtrl;
  __IO u32 OTGTmr;
       u32 RESERVED1[58];

  __I  u32 USBDevIntSt;            /* USB Device Interrupt Registers     */
  __IO u32 USBDevIntEn;
  __O  u32 USBDevIntClr;
  __O  u32 USBDevIntSet;

  __O  u32 USBCmdCode;             /* USB Device SIE Command Registers   */
  __I  u32 USBCmdData;

  __I  u32 USBRxData;              /* USB Device Transfer Registers      */
  __O  u32 USBTxData;
  __I  u32 USBRxPLen;
  __O  u32 USBTxPLen;
  __IO u32 USBCtrl;
  __O  u32 USBDevIntPri;

  __I  u32 USBEpIntSt;             /* USB Device Endpoint Interrupt Regs */
  __IO u32 USBEpIntEn;
  __O  u32 USBEpIntClr;
  __O  u32 USBEpIntSet;
  __O  u32 USBEpIntPri;

  __IO u32 USBReEp;                /* USB Device Endpoint Realization Reg*/
  __O  u32 USBEpInd;
  __IO u32 USBMaxPSize;

  __I  u32 USBDMARSt;              /* USB Device DMA Registers           */
  __O  u32 USBDMARClr;
  __O  u32 USBDMARSet;
       u32 RESERVED2[9];
  __IO u32 USBUDCAH;
  __I  u32 USBEpDMASt;
  __O  u32 USBEpDMAEn;
  __O  u32 USBEpDMADis;
  __I  u32 USBDMAIntSt;
  __IO u32 USBDMAIntEn;
       u32 RESERVED3[2];
  __I  u32 USBEoTIntSt;
  __O  u32 USBEoTIntClr;
  __O  u32 USBEoTIntSet;
  __I  u32 USBNDDRIntSt;
  __O  u32 USBNDDRIntClr;
  __O  u32 USBNDDRIntSet;
  __I  u32 USBSysErrIntSt;
  __O  u32 USBSysErrIntClr;
  __O  u32 USBSysErrIntSet;
       u32 RESERVED4[15];

  __I  u32 I2C_RX;                 /* USB OTG I2C Registers              */
  __O  u32 I2C_WO;
  __I  u32 I2C_STS;
  __IO u32 I2C_CTL;
  __IO u32 I2C_CLKHI;
  __O  u32 I2C_CLKLO;
       u32 RESERVED5[823];

  union {
  __IO u32 USBClkCtrl;             /* USB Clock Control Registers        */
  __IO u32 OTGClkCtrl;
  };
  union {
  __I  u32 USBClkSt;
  __I  u32 OTGClkSt;
  };
} LPC_USB_TypeDef;

/*------------- Ethernet Media Access Controller (EMAC) ----------------------*/
typedef struct
{
  __IO u32 MAC1;                   /* MAC Registers                      */
  __IO u32 MAC2;
  __IO u32 IPGT;
  __IO u32 IPGR;
  __IO u32 CLRT;
  __IO u32 MAXF;
  __IO u32 SUPP;
  __IO u32 TEST;
  __IO u32 MCFG;
  __IO u32 MCMD;
  __IO u32 MADR;
  __O  u32 MWTD;
  __I  u32 MRDD;
  __I  u32 MIND;
       u32 RESERVED0[2];
  __IO u32 SA0;
  __IO u32 SA1;
  __IO u32 SA2;
       u32 RESERVED1[45];
  __IO u32 Command;                /* Control Registers                  */
  __I  u32 Status;
  __IO u32 RxDescriptor;
  __IO u32 RxStatus;
  __IO u32 RxDescriptorNumber;
  __I  u32 RxProduceIndex;
  __IO u32 RxConsumeIndex;
  __IO u32 TxDescriptor;
  __IO u32 TxStatus;
  __IO u32 TxDescriptorNumber;
  __IO u32 TxProduceIndex;
  __I  u32 TxConsumeIndex;
       u32 RESERVED2[10];
  __I  u32 TSV0;
  __I  u32 TSV1;
  __I  u32 RSV;
       u32 RESERVED3[3];
  __IO u32 FlowControlCounter;
  __I  u32 FlowControlStatus;
       u32 RESERVED4[34];
  __IO u32 RxFilterCtrl;           /* Rx Filter Registers                */
  __IO u32 RxFilterWoLStatus;
  __IO u32 RxFilterWoLClear;
       u32 RESERVED5;
  __IO u32 HashFilterL;
  __IO u32 HashFilterH;
       u32 RESERVED6[882];
  __I  u32 IntStatus;              /* Module Control Registers           */
  __IO u32 IntEnable;
  __O  u32 IntClear;
  __O  u32 IntSet;
       u32 RESERVED7;
  __IO u32 PowerDown;
       u32 RESERVED8;
  __IO u32 Module_ID;
} LPC_EMAC_TypeDef;

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif


/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/
/* Base addresses                                                             */
#define LPC_FLASH_BASE        (0x00000000UL)
#define LPC_RAM_BASE          (0x10000000UL)
#define LPC_GPIO_BASE         (0x2009C000UL)
#define LPC_APB0_BASE         (0x40000000UL)
#define LPC_APB1_BASE         (0x40080000UL)
#define LPC_AHB_BASE          (0x50000000UL)
#define LPC_CM3_BASE          (0xE0000000UL)

/* APB0 peripherals                                                           */
#define LPC_WDT_BASE          (LPC_APB0_BASE + 0x00000)
#define LPC_TIM0_BASE         (LPC_APB0_BASE + 0x04000)
#define LPC_TIM1_BASE         (LPC_APB0_BASE + 0x08000)
#define LPC_UART0_BASE        (LPC_APB0_BASE + 0x0C000)
#define LPC_UART1_BASE        (LPC_APB0_BASE + 0x10000)
#define LPC_PWM1_BASE         (LPC_APB0_BASE + 0x18000)
#define LPC_I2C0_BASE         (LPC_APB0_BASE + 0x1C000)
#define LPC_SPI_BASE          (LPC_APB0_BASE + 0x20000)
#define LPC_RTC_BASE          (LPC_APB0_BASE + 0x24000)
#define LPC_GPIOINT_BASE      (LPC_APB0_BASE + 0x28080)
#define LPC_PINCON_BASE       (LPC_APB0_BASE + 0x2C000)
#define LPC_SSP1_BASE         (LPC_APB0_BASE + 0x30000)
#define LPC_ADC_BASE          (LPC_APB0_BASE + 0x34000)
#define LPC_CANAF_RAM_BASE    (LPC_APB0_BASE + 0x38000)
#define LPC_CANAF_BASE        (LPC_APB0_BASE + 0x3C000)
#define LPC_CANCR_BASE        (LPC_APB0_BASE + 0x40000)
#define LPC_CAN1_BASE         (LPC_APB0_BASE + 0x44000)
#define LPC_CAN2_BASE         (LPC_APB0_BASE + 0x48000)
#define LPC_I2C1_BASE         (LPC_APB0_BASE + 0x5C000)

/* APB1 peripherals                                                           */
#define LPC_SSP0_BASE         (LPC_APB1_BASE + 0x08000)
#define LPC_DAC_BASE          (LPC_APB1_BASE + 0x0C000)
#define LPC_TIM2_BASE         (LPC_APB1_BASE + 0x10000)
#define LPC_TIM3_BASE         (LPC_APB1_BASE + 0x14000)
#define LPC_UART2_BASE        (LPC_APB1_BASE + 0x18000)
#define LPC_UART3_BASE        (LPC_APB1_BASE + 0x1C000)
#define LPC_I2C2_BASE         (LPC_APB1_BASE + 0x20000)
#define LPC_I2S_BASE          (LPC_APB1_BASE + 0x28000)
#define LPC_RIT_BASE          (LPC_APB1_BASE + 0x30000)
#define LPC_MCPWM_BASE        (LPC_APB1_BASE + 0x38000)
#define LPC_QEI_BASE          (LPC_APB1_BASE + 0x3C000)
#define LPC_SC_BASE           (LPC_APB1_BASE + 0x7C000)

/* AHB peripherals                                                            */
#define LPC_EMAC_BASE         (LPC_AHB_BASE  + 0x00000)
#define LPC_GPDMA_BASE        (LPC_AHB_BASE  + 0x04000)
#define LPC_GPDMACH0_BASE     (LPC_AHB_BASE  + 0x04100)
#define LPC_GPDMACH1_BASE     (LPC_AHB_BASE  + 0x04120)
#define LPC_GPDMACH2_BASE     (LPC_AHB_BASE  + 0x04140)
#define LPC_GPDMACH3_BASE     (LPC_AHB_BASE  + 0x04160)
#define LPC_GPDMACH4_BASE     (LPC_AHB_BASE  + 0x04180)
#define LPC_GPDMACH5_BASE     (LPC_AHB_BASE  + 0x041A0)
#define LPC_GPDMACH6_BASE     (LPC_AHB_BASE  + 0x041C0)
#define LPC_GPDMACH7_BASE     (LPC_AHB_BASE  + 0x041E0)
#define LPC_USB_BASE          (LPC_AHB_BASE  + 0x0C000)

/* GPIOs                                                                      */
#define LPC_GPIO0_BASE        (LPC_GPIO_BASE + 0x00000)
#define LPC_GPIO1_BASE        (LPC_GPIO_BASE + 0x00020)
#define LPC_GPIO2_BASE        (LPC_GPIO_BASE + 0x00040)
#define LPC_GPIO3_BASE        (LPC_GPIO_BASE + 0x00060)
#define LPC_GPIO4_BASE        (LPC_GPIO_BASE + 0x00080)


/******************************************************************************/
/*                         Peripheral declaration                             */
/******************************************************************************/
#define LPC_SC                ((LPC_SC_TypeDef        *) LPC_SC_BASE       )
#define LPC_GPIO0             ((LPC_GPIO_TypeDef      *) LPC_GPIO0_BASE    )
#define LPC_GPIO1             ((LPC_GPIO_TypeDef      *) LPC_GPIO1_BASE    )
#define LPC_GPIO2             ((LPC_GPIO_TypeDef      *) LPC_GPIO2_BASE    )
#define LPC_GPIO3             ((LPC_GPIO_TypeDef      *) LPC_GPIO3_BASE    )
#define LPC_GPIO4             ((LPC_GPIO_TypeDef      *) LPC_GPIO4_BASE    )
#define LPC_WDT               ((LPC_WDT_TypeDef       *) LPC_WDT_BASE      )
#define LPC_TIM0              ((LPC_TIM_TypeDef       *) LPC_TIM0_BASE     )
#define LPC_TIM1              ((LPC_TIM_TypeDef       *) LPC_TIM1_BASE     )
#define LPC_TIM2              ((LPC_TIM_TypeDef       *) LPC_TIM2_BASE     )
#define LPC_TIM3              ((LPC_TIM_TypeDef       *) LPC_TIM3_BASE     )
#define LPC_RIT               ((LPC_RIT_TypeDef       *) LPC_RIT_BASE      )
#define LPC_UART0             ((LPC_UART0_TypeDef     *) LPC_UART0_BASE    )
#define LPC_UART1             ((LPC_UART1_TypeDef     *) LPC_UART1_BASE    )
#define LPC_UART2             ((LPC_UART_TypeDef      *) LPC_UART2_BASE    )
#define LPC_UART3             ((LPC_UART_TypeDef      *) LPC_UART3_BASE    )
#define LPC_PWM1              ((LPC_PWM_TypeDef       *) LPC_PWM1_BASE     )
#define LPC_I2C0              ((LPC_I2C_TypeDef       *) LPC_I2C0_BASE     )
#define LPC_I2C1              ((LPC_I2C_TypeDef       *) LPC_I2C1_BASE     )
#define LPC_I2C2              ((LPC_I2C_TypeDef       *) LPC_I2C2_BASE     )
#define LPC_I2S               ((LPC_I2S_TypeDef       *) LPC_I2S_BASE      )
#define LPC_SPI               ((LPC_SPI_TypeDef       *) LPC_SPI_BASE      )
#define LPC_RTC               ((LPC_RTC_TypeDef       *) LPC_RTC_BASE      )
#define LPC_GPIOINT           ((LPC_GPIOINT_TypeDef   *) LPC_GPIOINT_BASE  )
#define LPC_PINCON            ((LPC_PINCON_TypeDef    *) LPC_PINCON_BASE   )
#define LPC_SSP0              ((LPC_SSP_TypeDef       *) LPC_SSP0_BASE     )
#define LPC_SSP1              ((LPC_SSP_TypeDef       *) LPC_SSP1_BASE     )
#define LPC_ADC               ((LPC_ADC_TypeDef       *) LPC_ADC_BASE      )
#define LPC_DAC               ((LPC_DAC_TypeDef       *) LPC_DAC_BASE      )
#define LPC_CANAF_RAM         ((LPC_CANAF_RAM_TypeDef *) LPC_CANAF_RAM_BASE)
#define LPC_CANAF             ((LPC_CANAF_TypeDef     *) LPC_CANAF_BASE    )
#define LPC_CANCR             ((LPC_CANCR_TypeDef     *) LPC_CANCR_BASE    )
#define LPC_CAN1              ((LPC_CAN_TypeDef       *) LPC_CAN1_BASE     )
#define LPC_CAN2              ((LPC_CAN_TypeDef       *) LPC_CAN2_BASE     )
#define LPC_MCPWM             ((LPC_MCPWM_TypeDef     *) LPC_MCPWM_BASE    )
#define LPC_QEI               ((LPC_QEI_TypeDef       *) LPC_QEI_BASE      )
#define LPC_EMAC              ((LPC_EMAC_TypeDef      *) LPC_EMAC_BASE     )
#define LPC_GPDMA             ((LPC_GPDMA_TypeDef     *) LPC_GPDMA_BASE    )
#define LPC_GPDMACH0          ((LPC_GPDMACH_TypeDef   *) LPC_GPDMACH0_BASE )
#define LPC_GPDMACH1          ((LPC_GPDMACH_TypeDef   *) LPC_GPDMACH1_BASE )
#define LPC_GPDMACH2          ((LPC_GPDMACH_TypeDef   *) LPC_GPDMACH2_BASE )
#define LPC_GPDMACH3          ((LPC_GPDMACH_TypeDef   *) LPC_GPDMACH3_BASE )
#define LPC_GPDMACH4          ((LPC_GPDMACH_TypeDef   *) LPC_GPDMACH4_BASE )
#define LPC_GPDMACH5          ((LPC_GPDMACH_TypeDef   *) LPC_GPDMACH5_BASE )
#define LPC_GPDMACH6          ((LPC_GPDMACH_TypeDef   *) LPC_GPDMACH6_BASE )
#define LPC_GPDMACH7          ((LPC_GPDMACH_TypeDef   *) LPC_GPDMACH7_BASE )
#define LPC_USB               ((LPC_USB_TypeDef       *) LPC_USB_BASE      )

#endif  // __LPC17xx_H__
