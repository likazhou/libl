#ifndef __ARCH_UART_H__
#define __ARCH_UART_H__






//External Functions
void arch_UartInit(uart_t *p);
sys_res arch_UartOpen(uart_t *p, uart_para_t *pPara);
void arch_UartTxIEnable(uart_t *p);
void arch_UartIoHandler(uart_t *p, uint nId);
void arch_UartScReset(uart_t *p, uint nHL);


#endif

