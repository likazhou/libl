#ifndef __ARCH_UART_H__
#define __ARCH_UART_H__


#ifdef __cplusplus
extern "C" {
#endif









//External Functions
void arch_UartInit(uart_t *p);
sys_res arch_UartOpen(int nId, uart_para_t *pPara);
void arch_UartTxIEnable(int nId);
void arch_UartSendChar(int nId, const int nData);
int arch_UartGetChar(int nId);

void arch_UartISR(int nId);



#ifdef __cplusplus
}
#endif

#endif

