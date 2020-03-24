#ifndef __SC16IS7X_H__
#define __SC16IS7X_H__


#ifdef __cplusplus
extern "C" {
#endif










//External Functions
sys_res sc16is7x_Init(void);
sys_res sc16is7x_Reset(void);
sys_res sc16is7x_UartConfig(int nChl, uart_para_t *pPara);
sys_res sc16is7x_UartSend(int nChl, void *pData, size_t nLen);
void sc16is7x_ItHandler(void);
int sc16is7x_GpioRead(int nPin);
void sc16is7x_GpioSet(int nPin, int nHL);


#ifdef __cplusplus
}
#endif

#endif

