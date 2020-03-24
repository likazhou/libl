#ifndef __VK321X_H__
#define __VK321X_H__








//-------------------------------------------------------------------
//External Functions
//-------------------------------------------------------------------
sys_res vk321x_Init(void);
sys_res vk321x_UartConfig(int nChl, uart_para_t *pPara);
sys_res vk321x_UartSend(int nChl, const void *pData, size_t nLen);




#endif

