#ifndef __CHANNEL_H__
#define __CHANNEL_H__


//通道状态
#define CHL_S_IDLE				0
#define CHL_S_STANDBY			1
#define CHL_S_CONNECT			2
#define CHL_S_READY				3

//通道类型
#define CHL_T_RS232				0
#define CHL_T_IRDA				1

#define CHL_T_SOC_TC			(BITMASK(4) | 0)
#define CHL_T_SOC_TS			(BITMASK(4) | 1)
#define CHL_T_SOC_UC			(BITMASK(4) | 2)
#define CHL_T_SOC_US			(BITMASK(4) | 3)
#if TCPPS_ETH_RECON_EN
#define CHL_T_SOC_TC_RECON		(BITMASK(4) | 4)
#endif

struct _chl
{
	u8		ste;
	u8		type;
	void *	pIf;
} PACK_STRUCT_STRUCT;
typedef struct _chl chl_t, chl[1];

#include <chl/rs232.h>
#include <chl/sockchl.h>





//External Functions
void chl_Init(chl p);
sys_res chl_Bind(chl p, int nType, int nId, size_t nTmo);
sys_res chl_Release(chl p);
sys_res chl_Send(chl p, const void *pData, size_t nLen);
sys_res chl_RecData(chl p, buf b, size_t nTmo);


#endif



