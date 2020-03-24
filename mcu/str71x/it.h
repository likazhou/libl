#ifndef __ARCH_IT_H__
#define __ARCH_IT_H__



#define ARCH_EXTIRQ_QTY			4



//External Functions
sys_res arch_ExtirqInit(p_gpio_def p, int nTriggerMode);


//Interrupt Functions
__irq void XTI_IRQHandler(void);

__irq void UART0_IRQHandler(void);
__irq void UART1_IRQHandler(void);
__irq void UART2_IRQHandler(void);
__irq void UART3_IRQHandler(void);



#endif

