
#if RDSS_ENABLE
#include <drivers/rdss.h>

//Private Defines
#define RDSS_STAT_ENABLE		0


//Private Macros
#if RDSS_DEBUG_ENABLE
#define RDSS_DBGOUT				dbg_trace
#else
#define RDSS_DBGOUT(...)
#endif


/* Modem Operation */
#define rdss_Rst(x)				sys_GpioSet(&tbl_bspRdss.rst, x)
#define rdss_Pwr(x)				sys_GpioSet(&tbl_bspRdss.pwr, x)
#define rdss_En(x)				sys_GpioSet(&tbl_bspRdss.en, x)
#if RDSS_RF_ENABLE
#define rdss_Rfen(x)			sys_GpioSet(&tbl_bspRdss.rfen, x)
#endif
#if RDSS_TX_ENABLE
#define rdss_Txen(x)			sys_GpioSet(&tbl_bspRdss.txen, x)
#endif
#if RDSS_LEDTX_ENABLE
#define rdss_LedTx(x)			sys_GpioSet(&tbl_bspRdss.ledtx, x)
#endif
#if RDSS_LEDRX_ENABLE
#define rdss_LedRx(x)			sys_GpioSet(&tbl_bspRdss.ledrx, x)
#endif


//Private Variables
static rdss_t xRDSS = {0};

/* Macros. */
#define RD						(&xRDSS)

#if RDSS_STAT_ENABLE
static size_t rd_tcnt = 0, rd_rcnt = 0;
#endif





static void rdss_Output(rdss_t *p, char *msg)
{
	int l;

	l = strlen(msg);
	sprintf(&msg[l], "*%02X\r\n", xor8(&msg[1], l - 1));

	uart_SendStr(p->uart, msg);

	RDSS_DBGOUT("[RD>] ");
	RDSS_DBGOUT(msg);
	RDSS_DBGOUT(STRING_0D0A);
}


static void rdss_Cmd(rdss_t *p, const char *str)
{
	char msg[60];

	sprintf(msg, "$BD%s", str);
	rdss_Output(p, msg);
}



static void rdss_RxParse(rdss_t *p, const char *cmd)
{
	char *tmp;
	int i;
	u8 buf[RD_MSG_LEN];

	//获取卡信息
	if (memscmp(cmd, "ICI,") == 0)
		p->simid = bin2bcd32(atoi(&cmd[4]));

	//北斗波束信息
	if (memscmp(cmd, "BSI,") == 0)
	{
#if 0	//For Debug 不接天线调试AT通讯时用
#warning "BD BSI bypassed........................."
		p->signal = 4;
#else
		i = atoi(&cmd[4]);
		if (i)
		{
			p->signal = atoi(saftercomma(cmd, 2 + i));
			memcpy(p->bsi, &cmd[4], sizeof(p->bsi));
		}
		else
		{
			p->signal = 0;
		}
#endif
	}

	//报文通信信息
	if (memscmp(cmd, "TXR,") == 0)
	{
		tmp = saftercomma(cmd, 5);
		if (tmp)
		{
			buf[0] = chartoi(*tmp++);
			for (i = 1; isxdigit(*tmp) && (i < RD_MSG_LEN); i++)
			{
				buf[i] = chartoi(*tmp++) << 4;
				buf[i] |= chartoi(*tmp++);
			}
			if ((p->tlen == i) && (memcmp(p->tmsg, buf, i) == 0))
			{
				//自发自收的报文
				if (p->destid == 0)
					p->tlen = 0;
			}
			else
			{
				if (memcmp(p->rmsg, buf, i))
				{
					memcpy(p->rmsg, buf, i);

#if RDSS_LEDRX_ENABLE
					rdss_LedRx(0);
#endif
					//接收报文处理
					rdss_RxDo(buf, i);
	
#if RDSS_STAT_ENABLE
					sprintf((char *)buf, "[RD<] TXR Count : %d\r", ++rd_rcnt);
					RDSS_DBGOUT((char *)buf);
#endif
				}
			}
		}
	}

	//发送反馈信息
	if (memscmp(cmd, "FKI,TXA,Y,Y") == 0)
	{
		//收到反馈关闭发送
#if RDSS_TX_ENABLE
//		rdss_Txen(0);
#else
		rdss_Cmd(p, "JMS,E");
#endif

		p->tcnt = 61;
		if (p->destid)
		{
			if (p->trep)
				p->trep -= 1;
			else
				p->tlen = 0;
		}
	}
}

static void rdss_TxData(rdss_t *p)
{
	int i;
	char msg[200];

	if (p->tcnt)
	{
#if 1 && RDSS_DEBUG_ENABLE
		if (((p->tcnt & 0x07) == 0) || (p->tcnt < 11))
		{
			sprintf(msg, "[RD ] TXA Countdown: %d\r", p->tcnt);
			RDSS_DBGOUT(msg);
		}
#endif
		p->tcnt -= 1;
		return;
	}

	if (p->signal == 0)
		return;

	os_sem_wait(&p->tsem);

	if (p->tlen == 0)
		p->tlen = rdss_ReadMsg(p->tmsg, &p->trep);

	os_sem_signal(&p->tsem);

	if (p->tlen == 0)
		return;

#if RDSS_LEDTX_ENABLE
	rdss_LedTx(0);
#endif

	//打开发送
#if RDSS_TX_ENABLE
//	rdss_Txen(1);
//	os_thd_sleep(100);
#else
	rdss_Cmd(p, "JMS,N");
#endif

	os_sem_wait(&p->tsem);

	if (p->destid)
		i = p->destid;
	else
		i = p->simid;
	sprintf(msg, "$BDTXA,%07X,1,1,%1X", i, p->tmsg[0]);
	for (i = 1; i < p->tlen; i++)
	{
		sprintf(&msg[18 + 2 * i], "%02X", p->tmsg[i]);
	}

	os_sem_signal(&p->tsem);

	rdss_Output(p, msg);

	//收不到反馈10秒重发
	p->tcnt = 10;

#if RDSS_STAT_ENABLE
	sprintf(msg, "[RD>] TXA Count : %d\r", ++rd_tcnt);
	RDSS_DBGOUT(msg);
#endif
}



void rdss_Init()
{
	rdss_t *p = &xRDSS;
	rdss_def_t *pDef = &tbl_bspRdss;

	os_sem_init(&p->tsem, 1);
	
	sys_GpioConf(&pDef->rst);
	sys_GpioConf(&pDef->pwr);
	sys_GpioConf(&pDef->en);
#if RDSS_RF_ENABLE
	sys_GpioConf(&pDef->rfen);
#endif
#if RDSS_TX_ENABLE
	sys_GpioConf(&pDef->txen);
#endif
#if RDSS_LEDTX_ENABLE
	sys_GpioConf(&pDef->ledtx);
#endif
#if RDSS_LEDRX_ENABLE
	sys_GpioConf(&pDef->ledrx);
#endif
#if RDSS_1PPS_ENABLE
	sys_GpioConf(&pDef->onepps);
#endif
	p->uart = uart_Open(RDSS_UART_ID, OS_TMO_FOREVER);
}



void rdss_Run()
{
	rdss_t *p = &xRDSS;
	int l;

#if RDSS_ADC_THRESHOLD
	int ad;
	ad = rdss_PwrADC();
#if 0 && RDSS_DEBUG_ENABLE
	{
		char str[32];

		sprintf(str, "[RD ] ADC = %d\r", ad);
		RDSS_DBGOUT(str);
	}
#endif
#endif

#if RDSS_LEDTX_ENABLE
	rdss_LedTx(1);
#endif
#if RDSS_LEDRX_ENABLE
	rdss_LedRx(1);
#endif

	p->tmo += 1;
	switch (p->ste)
	{
	default:
		RDSS_DBGOUT("[RD ] Reset!!\r");
	
		uart_Config(p->uart, RDSS_UART_BAUD, UART_PARI_NO, UART_DATA_8D, UART_STOP_1D);
		p->simid = 0;
		p->signal = 0;
		p->tmo = 0;
		p->pwren = 0;

		rdss_Pwr(0);

		p->ste = RDRN_S_WAIT;
		break;

	case RDRN_S_WAIT:
#if RDSS_ADC_THRESHOLD
		if (ad < RDSS_ADC_THRESHOLD)
			break;
#else
		os_thd_sleep(20000);
#endif

		if (p->pwren == 0)
			break;

		rdss_Pwr(1);
		os_thd_sleep(800);
		rdss_Rst(0);
		os_thd_sleep(100);
		rdss_Rst(1);
		
		p->ste = RDRN_S_INIT;

		break;

	case RDRN_S_INIT:
		
		//关闭发送
#if RDSS_TX_ENABLE
//		rdss_Txen(0);
#else
		rdss_Cmd(p, "JMS,E");
#endif
		//更新卡信息
		rdss_Cmd(p, "ICA,0,");
		//打开波束信息
		rdss_Cmd(p, "RMO,BSI,2,3");

		p->ste = RDRN_S_ONLINE;
		break;

	case RDRN_S_ONLINE:

		//接受并回应
		do
		{
			uart_Recive(p->uart, p->rbuf);
			
			l = buffbd(p->rbuf, 200);
			if (l)
			{
				RDSS_DBGOUT("[RD<] ");
				RDSS_DBGOUT((char *)p->rbuf->p);

				rdss_RxParse(p, (char *)&p->rbuf->p[3]);
				buf_Remove(p->rbuf, l);

				p->tmo = 0;
			}
		} while (l);

		//20秒收不到信息复位
		if (p->tmo > 20)
			sys_Reset();

		//发送
		rdss_TxData(p);

		break;
	}
}



void rdss_PowerEn()
{
	
	RD->pwren = 1;
}

u32 rdss_GetSIM()
{

	return RD->simid;
}

int rdss_GetSignal()
{

	return RD->signal;
}

char *rdss_GetBSI()
{

	return RD->bsi;
}

u32 rdss_GetDest()
{

	return RD->destid;
}

void rdss_SetDest(u32 id)
{

	RD->destid = id;
}

int rdss_TxNow(const void *data, size_t len, int rep)
{
	rdss_t *p = &xRDSS;
	int send = 0;

	if (p->signal == 0)
		return 0;

	if (p->tcnt)
		return 0;

	if (rdss_MsgQty())
		return 0;

	os_sem_wait(&p->tsem);

	if (p->tlen == 0)
	{
		p->tlen = MIN(len, RD_MSG_LEN);
		memcpy(p->tmsg, data, p->tlen);
		p->trep = rep;
		send = 1;
	}

	os_sem_signal(&p->tsem);

	return send;
}
#endif














