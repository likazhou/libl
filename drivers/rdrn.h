#ifndef __RDRN_H__
#define __RDRN_H__

#ifdef __cplusplus
extern "C" {
#endif


/* Modem State */
enum
{
	RDRN_S_RESET = 0,
	RDRN_S_WAIT,
	RDRN_S_INIT,
	RDRN_S_ONLINE,
};

//Public Defines
#define RD_MSG_LEN				79	//628bits
#define RD_SIM_MAX				8


//Public Typedefs
struct rdrn
{
	u8			ste;
	u8			tmo;
	u8			signal; 			/* 响应波束号:当前选用的信号最强的波束: 响应波束不为0，则表示GPIO_LED_RD_NET正常 */
	u8			tcnt;
	u8			tlen;
	u8			trep;
	char		bsi[25];
	u8			tmsg[RD_MSG_LEN];
	u8			rmsg[RD_MSG_LEN];
	u32 		simid[RD_SIM_MAX];			/* 本机号: 本机号不为0，则表示有IC卡插入 */
	u32 		destid; 			//指挥机卡号
	int			latitude;
	int			longitud;
	time_t		time;
	uart_t *	uart;
	buf 		rbuf;
	os_sem_t	tsem;
};
typedef struct rdrn rdrn_t;




























#ifdef __cplusplus
}
#endif

#endif

