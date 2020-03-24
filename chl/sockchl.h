#ifndef __CHL_SOCKET_H__
#define __CHL_SOCKET_H__











//External Functions
sys_res chl_soc_Bind(chl p, int nType, int nId);
sys_res chl_soc_Connect(chl p, const void *pIp, int nPort);
sys_res chl_soc_Listen(chl p);
int chl_soc_IsConnect(chl p);




#endif

