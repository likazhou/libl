#ifndef __DLRCP_H__
#define __DLRCP_H__

#ifdef __cplusplus
extern "C" {
#endif


//-------------------------------------------------------------------------
//Public Defines
//-------------------------------------------------------------------------
//通讯状态定义
#define DLRCP_S_IDLE					0
#define DLRCP_S_CHECK					1
#define DLRCP_S_READY					2

//发送报文处理策略
#define DLRCP_TMSG_RESPOND				0
#define DLRCP_TMSG_REPORT				1
#define DLRCP_TMSG_CASCADE				2



//-------------------------------------------------------------------------
//Public Typedefs
//-------------------------------------------------------------------------
#ifdef __CC_ARM
#pragma anon_unions
#endif

struct dlrcp
{
	u8	ste;
#if LIB_ZIP_ENABLE
	u8	zip;
#endif
	u8	time;
	u8	retry;
	u8	pfc;
	u16	refresh;
	u16	cnt;
	u16	chlid;
	u16	rcvtime;
	chl chl;
	buf	rbuf;
	union
	{
		uart_para_t uart;
		u8		ip[4];
	};
	sys_res	(*linkcheck)(void *, int);
	sys_res (*analyze)(void *);
};
typedef struct dlrcp *p_dlrcp;

#ifdef __CC_ARM
#pragma no_anon_unions
#endif








//External Functions
void dlrcp_Init(p_dlrcp p, sys_res (*linkcheck)(void *, int), sys_res (*analyze)(void *));
sys_res dlrcp_SetChl(p_dlrcp p, int nType, int nId, int nPar1, int nPar2, int nPar3, int nPar4);
sys_res dlrcp_TmsgSend(p_dlrcp p, void *pHeader, size_t nHeaderLen, void *pData, size_t nDataLen, int nType);
sys_res dlrcp_Handler(p_dlrcp p);





#ifdef __cplusplus
}
#endif

#endif

