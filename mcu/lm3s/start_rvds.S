; <<< Use Configuration Wizard in Context Menu >>>
;******************************************************************************
;
; Startup.s - Startup code for Stellaris.
;
; Copyright (c) 2006-2008 Luminary Micro, Inc.  All rights reserved.
; 
; Software License Agreement
; 
; Luminary Micro, Inc. (LMI) is supplying this software for use solely and
; exclusively on LMI's microcontroller products.
; 
; The software is owned by LMI and/or its suppliers, and is protected under
; applicable copyright laws.  All rights are reserved.  You may not combine
; this software with "viral" open-source software in order to form a larger
; program.  Any use in violation of the foregoing restrictions may subject
; the user to criminal sanctions under applicable laws, as well as to civil
; liability for the breach of the terms and conditions of this license.
; 
; THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; 
; This is part of revision 2523 of the Stellaris Peripheral Driver Library.
;
;******************************************************************************

;******************************************************************************
;
; <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
;
;******************************************************************************
Stack   EQU     0x00000200

;******************************************************************************
;
; <o> Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
;
;******************************************************************************
Heap    EQU     0x00000000

;******************************************************************************
;
; Allocate space for the stack.
;
;******************************************************************************
        AREA    STACK, NOINIT, READWRITE, ALIGN=3
StackMem
        SPACE   Stack
__initial_sp

;******************************************************************************
;
; Allocate space for the heap.
;
;******************************************************************************
        AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
HeapMem
        SPACE   Heap
__heap_limit

		IMPORT PendSV_Handler
		IMPORT SysTick_Handler
		IMPORT HardFault_Handler
		IMPORT lm3s_isr_porta
		IMPORT lm3s_isr_portb
		IMPORT lm3s_isr_portc
		IMPORT lm3s_isr_portd
		IMPORT lm3s_isr_porte
		IMPORT lm3s_isr_uart0
		IMPORT lm3s_isr_uart1
		IMPORT lm3s_isr_timer0a
		IMPORT lm3s_isr_timer0b
		IMPORT lm3s_isr_timer1a
		IMPORT lm3s_isr_timer1b
		IMPORT lm3s_isr_timer2a
		IMPORT lm3s_isr_timer2b
		IMPORT lm3s_isr_portf
		IMPORT lm3s_isr_portg
		IMPORT lm3s_isr_porth
		IMPORT lm3s_isr_uart2
		IMPORT lm3s_isr_timer3a
		IMPORT lm3s_isr_timer3b
		IMPORT lm3s_isr_eth
		IMPORT lm3s_isr_usb0


;******************************************************************************
;
; Indicate that the code in this file preserves 8-byte alignment of the stack.
;
;******************************************************************************
        PRESERVE8

;******************************************************************************
;
; Place code into the reset code section.
;
;******************************************************************************
        AREA    RESET, CODE, READONLY
        THUMB

;******************************************************************************
;
; The vector table.
;
;******************************************************************************
        EXPORT  __Vectors
__Vectors
        DCD     StackMem + Stack            ; Top of Stack
        DCD     Reset_Handler               ; Reset Handler
        DCD     NmiSR                       ; NMI Handler
        DCD     HardFault_Handler            ; Hard Fault Handler
        DCD     IntDefaultHandler           ; MPU Fault Handler
        DCD     IntDefaultHandler           ; Bus Fault Handler
        DCD     IntDefaultHandler           ; Usage Fault Handler
        DCD     0                           ; Reserved
        DCD     0                           ; Reserved
        DCD     0                           ; Reserved
        DCD     0                           ; Reserved
        DCD     IntDefaultHandler           ; SVCall Handler
        DCD     IntDefaultHandler           ; Debug Monitor Handler
        DCD     0                           ; Reserved
        DCD     PendSV_Handler           	; PendSV Handler
        DCD     SysTick_Handler             ; SysTick Handler
        DCD     lm3s_isr_porta              ; GPIO Port A
        DCD     lm3s_isr_portb              ; GPIO Port B
        DCD     lm3s_isr_portc              ; GPIO Port C
        DCD     lm3s_isr_portd              ; GPIO Port D
        DCD     lm3s_isr_porte              ; GPIO Port E
        DCD     lm3s_isr_uart0				; UART0
        DCD     lm3s_isr_uart1				; UART1
        DCD     IntDefaultHandler           ; SSI
        DCD     IntDefaultHandler           ; I2C
        DCD     IntDefaultHandler           ; PWM Fault
        DCD     IntDefaultHandler           ; PWM Generator 0
        DCD     IntDefaultHandler           ; PWM Generator 1
        DCD     IntDefaultHandler           ; PWM Generator 2
        DCD     IntDefaultHandler           ; Quadrature Encoder
        DCD     IntDefaultHandler           ; ADC Sequence 0
        DCD     IntDefaultHandler           ; ADC Sequence 1
        DCD     IntDefaultHandler           ; ADC Sequence 2
        DCD     IntDefaultHandler           ; ADC Sequence 3
        DCD     IntDefaultHandler           ; Watchdog
        DCD     lm3s_isr_timer0a            ; Timer 0A
        DCD     lm3s_isr_timer0b            ; Timer 0B
        DCD     lm3s_isr_timer1a            ; Timer 1A
        DCD     lm3s_isr_timer1b            ; Timer 1B
        DCD     lm3s_isr_timer2a            ; Timer 2A
        DCD     lm3s_isr_timer2b            ; Timer 2B
        DCD     IntDefaultHandler           ; Comp 0
        DCD     IntDefaultHandler           ; Comp 1
        DCD     IntDefaultHandler           ; Comp 2
        DCD     IntDefaultHandler           ; System Control
        DCD     0x881                       ; Flash Control
        DCD     lm3s_isr_portf              ; GPIO Port F
        DCD     lm3s_isr_portg              ; GPIO Port G
        DCD     lm3s_isr_porth              ; GPIO Port H
        DCD     lm3s_isr_uart2				; UART2 Rx and Tx
        DCD     IntDefaultHandler           ; SSI1 Rx and Tx
        DCD     lm3s_isr_timer3a            ; Timer 3 subtimer A
        DCD     lm3s_isr_timer3b            ; Timer 3 subtimer B
        DCD     IntDefaultHandler           ; I2C1 Master and Slave
        DCD     IntDefaultHandler           ; Quadrature Encoder 1
        DCD     IntDefaultHandler           ; CAN0
        DCD     IntDefaultHandler           ; CAN1
        DCD     IntDefaultHandler           ; CAN2
        DCD     lm3s_isr_eth   	    		; Ethernet
        DCD     IntDefaultHandler          	; Hibernate
        DCD     lm3s_isr_usb0               ; USB0
        DCD     IntDefaultHandler           ; PWM Generator 3
        DCD     IntDefaultHandler           ; uDMA Software Transfer
        DCD     IntDefaultHandler           ; uDMA Error

;******************************************************************************
;
; This is the code that gets called when the processor first starts execution
; following a reset event.
;
;******************************************************************************
        EXPORT  Reset_Handler
Reset_Handler
        ;
        ; Call the C library enty point that handles startup.  This will copy
        ; the .data section initializers from flash to SRAM and zero fill the
        ; .bss section.
        ;
        IMPORT  __main
        B       __main

;******************************************************************************
;
; This is the code that gets called when the processor receives a NMI.  This
; simply enters an infinite loop, preserving the system state for examination
; by a debugger.
;
;******************************************************************************
NmiSR
        B       NmiSR

;******************************************************************************
;
; This is the code that gets called when the processor receives a fault
; interrupt.  This simply enters an infinite loop, preserving the system state
; for examination by a debugger.
;
;******************************************************************************
FaultISR
        B       FaultISR

;******************************************************************************
;
; This is the code that gets called when the processor receives an unexpected
; interrupt.  This simply enters an infinite loop, preserving the system state
; for examination by a debugger.
;
;******************************************************************************
IntDefaultHandler
        B       IntDefaultHandler

;******************************************************************************
;
; Make sure the end of this section is aligned.
;
;******************************************************************************
        ALIGN

;******************************************************************************
;
; Some code in the normal code section for initializing the heap and stack.
;
;******************************************************************************
        AREA    |.text|, CODE, READONLY

;******************************************************************************
;
; The function expected of the C library startup code for defining the stack
; and heap memory locations.  For the C library version of the startup code,
; provide this function so that the C library initialization code can find out
; the location of the stack and heap.
;
;******************************************************************************
    IF :DEF: __MICROLIB
        EXPORT  __initial_sp
        EXPORT  __heap_base
        EXPORT __heap_limit
    ELSE
        IMPORT  __use_two_region_memory
        EXPORT  __user_initial_stackheap
__user_initial_stackheap
        LDR     R0, =HeapMem
        LDR     R1, =(StackMem + Stack)
        LDR     R2, =(HeapMem + Heap)
        LDR     R3, =StackMem
        BX      LR
    ENDIF

;******************************************************************************
;
; Make sure the end of this section is aligned.
;
;******************************************************************************
        ALIGN

;******************************************************************************
;
; Tell the assembler that we're done.
;
;******************************************************************************
        END
