#ifndef __DM9000_H__
#define __DM9000_H__



#ifdef __cplusplus
extern "C" {
#endif




sys_res dm9000_PacketReceive(void);
sys_res dm9000_PacketSend(const void *pBuf, size_t nLen);
sys_res dm9000_Control(int nCmd, void *args);
sys_res dm9000_Reset(void);
void dm9000_SetMAC(const u8 *pMac);
void dm9000_Init(void);


#ifdef __cplusplus
}
#endif

#endif

