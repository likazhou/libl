#ifndef __AT_SERVER_H__
#define __AT_SERVER_H__


#ifdef __cplusplus
extern "C" {
#endif



//Public Defines
	

//Public Typedefs
struct atserver
{
	u8		ste;
	u8		dlci;
#if ATSVR_CMUX_ENABLE
	u8		cmux;
#endif
#if ATSVR_PPP_ENABLE
	u8		ppp;
	u8		tmo;
#endif
#if ATSVR_ATRW_ENABLE
	u8		soc;
	u8		dip[4];
#endif
	u16		cnt;
	uart_t *uart;
#if ATSVR_CMUX_ENABLE
	buf		cbuf;
#endif
	buf		rbuf[2];
	buf		tbuf;
};
typedef struct atserver atsvr_t;



//External Functions
void atsvr_Init(void);
void atsvr_Config(int baud);
int atsvr_GetState(void);
void atsvr_Run(void);
int atsvr_TxPush(const void *data, size_t len);



//User Functions
void atsvr_IGT(void);
void atsvr_RxDo(void *data, size_t len);
const char *atsvr_GMR(void);
void atsvr_BDId4ATCmd(const void *data);
void atsvr_BDId4IP(const void *data);


#ifdef __cplusplus
}
#endif


#endif

