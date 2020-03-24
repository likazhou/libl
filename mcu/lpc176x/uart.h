#ifndef __ARCH_UART_H__
#define __ARCH_UART_H__


#ifdef __cplusplus
extern "C" {
#endif









//External Functions
void arch_UartInit(uart_t *p);
sys_res arch_UartOpen(uint nId, uart_para_t *pPara);
void arch_UartTxIEnable(uint nId);
void arch_UartSendChar(uint nId, const int nData);
int arch_UartGetChar(uint nId);

void arch_UartISR(uint nId);



#ifdef __cplusplus
}
#endif

#endif


