
#if RDRN_ENABLE
#include <drivers/rdrn.h>

//Private Defines


//Private Macros
#if RDRN_DEBUG_ENABLE
#define RDRN_DBGOUT				dbg_trace
#else
#define RDRN_DBGOUT(...)
#endif





//Private Variables
static rdrn_t xRDRN = {0};




static void rdss_Output(rdrn_t *p, char *msg)
{
	int l;

	l = strlen(msg);
	sprintf(&msg[l], "*%02X\r\n", xor8(&msg[1], l - 1));

	uart_SendStr(p->uart, msg);

	RDRN_DBGOUT("[RD>] ");
	RDRN_DBGOUT(msg);
	RDRN_DBGOUT(STRING_0D0A);
}


static void rdss_Cmd(rdrn_t *p, const char *str)
{
	char msg[60];

	sprintf(msg, "$BD%s", str);
	rdss_Output(p, msg);
}


static void rdrn_RxParse(rdrn_t *p, const char *cmd)
{
	char *tmp;
	int i;
	u32 nt;
	u8 buf[RD_MSG_LEN];
	
	//位置信息
	if (memscmp(cmd, "GGA") == 0)
	{
		tmp = saftercomma(cmd, 6);
		p->fixed = *tmp - '0';

		if (p->fixed)
		{
			tmp = saftercomma(cmd, 2);
			p->latitude = rnsftoi(tmp);

			tmp = saftercomma(tmp, 2);
			p->longitud = rnsftoi(tmp);
		}
	}

	//RTC
	if (memscmp(cmd, "RMC") == 0)
	{
		if (isdigit(cmd[5]))
		{
			//time
			nt = bin2bcd32(atoi(&cmd[5]));
			memcpy(buf, &nt, 3);

			//date
			tmp = saftercomma(cmd, 9);
			if (isdigit(*tmp))
			{
				nt = bin2bcd32(atoi(tmp));
				cpyinvert(&buf[3], &nt, 3);

				//UTC + 8:00
				p->time = array2timet(buf, 1) + (8 * 3600);
			}
		}
	}

	//获取卡信息
	if (memscmp(cmd, "ICI,") == 0)
		p->simid[0] = bin2bcd32(atoi(&cmd[4]));

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

					//接收报文处理
					rdss_RxDo(buf, i);
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






void rdrn_Init()
{
	rdrn_t *p = &xRDRN;

	os_sem_init(&p->tsem, 1);

	p->uart = uart_Open(RDRN_UART_ID, OS_TMO_FOREVER);
	uart_Config(p->uart, RDRN_UART_BAUD, UART_PARI_NO, UART_DATA_8D, UART_STOP_1D);
}


void rdrn_Run()
{
	rdrn_t *p = &xRDRN;
	int l;

	//接受并回应
	do
	{
		uart_Recive(p->uart, p->rbuf);
		
		l = buffbd(p->rbuf, 200);
		if (l)
		{
			RDRN_DBGOUT("[RD<] ");
			RDRN_DBGOUT((char *)p->rbuf->p);
	
			rdrn_RxParse(p, (char *)&p->rbuf->p[3]);
			buf_Remove(p->rbuf, l);
	
			p->tmo = 0;
		}
	} while (l);
}



u32 rdss_GetSIM()
{

	return xRDRN.simid[0];
}

int rdss_GetSignal()
{

	return xRDRN.signal;
}

char *rdss_GetBSI()
{

	return xRDRN.bsi;
}

u32 rdss_GetDest()
{

	return xRDRN.destid;
}

void rdss_SetDest(u32 id)
{

	xRDRN.destid = id;
}

int rdss_TxNow(const void *data, size_t len, int rep)
{
	rdrn_t *p = &xRDRN;
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

void rnss_GetLocation(int *latitude, int *longitud)
{
	
	*latitude = xRDRN.latitude;
	*longitud = xRDRN.longitud;
}

time_t rnss_GetTimet()
{	

	return xRDRN.time;
}



#endif








