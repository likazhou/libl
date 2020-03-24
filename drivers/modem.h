#ifndef __MODEM_H__
#define __MODEM_H__


/* Modem State */
enum
{
	MODEM_S_RESET = 0,
	MODEM_S_POWEROFF,	
	MODEM_S_POWERON,
	MODEM_S_INIT,
	MODEM_S_WAITDIAL,
	MODEM_S_ONLINE,
};


/* Modem Type */
#define MODEM_TYPE_GPRS		1
#define MODEM_TYPE_CDMA		0



//Public Typedefs
struct modem
{
	u8		ste;
	u8		type;
#if MODEM_TCP_ENABLE
	u8		mtcp;
	u8		mcon;
#endif
	u8		dialed;
	u8		signal;
	u8		retrytime;
	u8		retryed;
	u8		hour;
#if MODEM_TCP_ENABLE
	u8		flag;	//强制使用内部协议栈标识
#endif
	char	apn[20];
	char	user[32];
	char	pwd[32];
	char	ver[20];
	char	ccid[20];
	char	imei[20];
#if MODEM_SMS_ENABLE
	char	SCA[16];			//短消息服务中心号码(SMSC地址)
#endif
	u16		cnt;
	u16		tmo;
	u16		span;
	u16		idle;
#if MODEM_FLOWCTL_ENABLE
	size_t	flow_r;
	size_t	flow_t;
#endif
	uart_t *uart;
	buf		rbuf;
};
typedef struct modem t_modem, *p_modem;


struct modem_def
{
	u8			act_effect;
	t_gpio_def	act;
#if MODEM_PWR_ENABLE
	u8			pwr_effect;
	t_gpio_def	power;
#endif
#if MODEM_RST_ENABLE
	u8			rst_effect;
	t_gpio_def	rst;
#endif
#if MODEM_CTS_ENABLE
	t_gpio_def	cts;
#endif
#if MODEM_RTS_ENABLE
	t_gpio_def	rts;
#endif
#if MODEM_DTR_ENABLE
	t_gpio_def	dtr;
#endif
#if MODEM_DCD_ENABLE
	t_gpio_def	dcd;
#endif
};
typedef const struct modem_def t_modem_def;


//External Functions
void modem_Init(void);
void modem_Run(void);
void modem_Reset(void);
void modem_PowerOff(void);
void modem_Config(const char *pApn, const char *pUser, const char *pPwd,
							int nSpan, int nRetry, int nFlag);
int modem_RxDialed(buf b);
int modem_IsDataMode(void);
int modem_IsOnline(void);
int modem_GetSignal(void);
int modem_GetState(void);
int modem_GetType(void);
int modem_GetVersion(char *pVersion);
int modem_GetCCID(char *pCCID);
int modem_GetIMEI(char *pIMEI);
#if MODEM_FLOWCTL_ENABLE
void modem_FlowStep(int nIsTx, size_t nLen);
int modem_GetFlow(void);
int modem_GetRcvFlow(void);
int modem_GetSendFlow(void);
#endif
void modem_Refresh(void);
#if MODEM_TCP_ENABLE
int modem_IsMTcp(void);
sys_res mtcp_TcpRecv(buf b);
sys_res mtcp_TcpConnect(const u8 *pIp, int nPort);
sys_res mtcp_TcpSend(const void *pData, size_t nLen);
sys_res mtcp_TcpClose(void);
#endif

#endif

