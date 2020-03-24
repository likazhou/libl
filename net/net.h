#ifndef __NET_NET_H__
#define __NET_NET_H__


#ifdef __cplusplus
extern "C" {
#endif


/* PHY Identifier                    */
#define PHY_ID_DP83848C		0x20005C90
#define PHY_ID_DM9161A		0x0181B8A0
#define PHY_ID_DM9161B		0x0181B8B0


#if TCPPS_TYPE == TCPPS_T_KEILTCP
#include <net/rtxip/Net_Config.h>
#include <net/rtxip/rtxip.h>
#endif












//External Functions
void net_GetIpPPP(void *pIp, void *pMask, void *pGetway);




#ifdef __cplusplus
}
#endif

#endif

