#ifndef __UARTBUS_H__
#define __UARTBUS_H__


#ifdef __cplusplus
extern "C" {
#endif













//External Functions
void swuart_Init(uart_t *p);
sys_res swuart_Open(int nId, uart_para_t *pPara);
void swuart_TxStart(int nId);




#ifdef __cplusplus
}
#endif

#endif


