#ifndef __NET_RTX_H__
#define __NET_RTX_H__


#ifdef __cplusplus
extern "C" {
#endif



//Public Typedefs
typedef int		socklen_t;

//-------------------------------------
//System Functions
void sys_error(ERROR_CODE code);

//External Functions
void rtxip_Handler(void *args);
void net_Init(void);
int net_Socket(int domain, int type, int protocol);
int net_Close(int s);
int net_Bind(int s, const struct sockaddr *name, int namelen);
int net_Listen(int s, int backlog);
int net_Connect(int s, struct sockaddr *name, int namelen);
int net_IsConnect(int s);
int net_Send(int s, u8 *pBuf, size_t nLen);
int net_Recv(int s, char *pBuf, size_t nMaxLen);

void net_GetIpPPP(void *pIp, void *pMask, void *pGetway);
#if TCPPS_ETH_ENABLE
int net_IsLinkETH(void);
void net_GetIpETH(void *pIp, void *pMask, void *pGetway);
void net_SetIpETH(const void *pIp, const void *pMask, const void *pGetway);
void net_SetMacETH(const void *pMac);
void net_SetGateWayETH(int nEnable);
#endif


#ifdef __cplusplus
}
#endif

#endif

