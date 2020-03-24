#ifndef __RDSS_H__
#define __RDSS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <drivers/rdrn.h>



//Public Typedefs
struct rdss
{
	u8			ste;
	u8			tmo;
	u8			pwren;
	u8			signal;			/* 响应波束号:当前选用的信号最强的波束: 响应波束不为0，则表示GPIO_LED_RD_NET正常 */
	u8			tcnt;
	u8			tlen;
	u8			trep;
	char		bsi[25];
	u8			tmsg[RD_MSG_LEN];
	u8			rmsg[RD_MSG_LEN];
	u32			simid;			/* 本机号: 本机号不为0，则表示有IC卡插入 */
	u32			destid;			//指挥机卡号
	uart_t *	uart;
	buf			rbuf;
	os_sem_t	tsem;
};
typedef struct rdss rdss_t;


struct rdss_def
{
	t_gpio_def	rst;
	t_gpio_def	pwr;
	t_gpio_def	en;
#if RDSS_RF_ENABLE
	t_gpio_def	rfen;
#endif
#if RDSS_TX_ENABLE
	t_gpio_def	txen;
#endif
#if RDSS_LEDTX_ENABLE
	t_gpio_def	ledtx;
#endif
#if RDSS_LEDRX_ENABLE
	t_gpio_def	ledrx;
#endif
#if RDSS_1PPS_ENABLE
	t_gpio_def	onepps;
#endif
};
typedef const struct rdss_def rdss_def_t;


//External Functions
void rdss_PowerEn(void);
u32 rdss_GetSIM(void);
int rdss_GetSignal(void);
char *rdss_GetBSI(void);
u32 rdss_GetDest(void);
void rdss_SetDest(u32 id);
int rdss_TxNow(const void *data, size_t len, int rep);

//User Functions
int rdss_ReadMsg(void *p, u8 *rep);
int rdss_MsgQty(void);
void rdss_RxDo(const u8 *p, size_t len);
int rdss_PwrADC(void);


#ifdef __cplusplus
}
#endif

#endif

