
#if RNSS_ENABLE
#include <drivers/rnss.h>

//Private Defines



//Private Macros
#if RNSS_DEBUG_ENABLE
#define RNSS_DBGOUT				dbg_trace
#else
#define RNSS_DBGOUT(...)
#endif


/* Modem Operation */
#if RNSS_RST_ENABLE
#define rnss_Rst(x)				sys_GpioSet(&tbl_bspRnss.rst, x)
#endif



//Private Variables
static rnss_t xRNSS = {0};



static int rnss_sftoi(const char *s)
{
	int i;
	char str[16];
	
	for (i = 0; i < sizeof(str); s++)
	{
		if (isdigit(*s))
		{
			str[i++] = *s;
		}
		else
		{
			if (*s != '.')
				break;
		}
	}
	str[i] = 0;

	return atoi(str);
}


static void rnss_RxParse(rnss_t *p, const char *cmd)
{
	char *tmp;
	u32 nt;
	u8 buf[6];
	
	//位置信息
	if (memscmp(cmd, "GGA") == 0)
	{
		tmp = saftercomma(cmd, 6);
		p->fixed = *tmp - '0';

		if (p->fixed)
		{
			tmp = saftercomma(cmd, 2);
			p->latitude = rnss_sftoi(tmp);

			tmp = saftercomma(tmp, 2);
			p->longitud = rnss_sftoi(tmp);
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
}


void rnss_Init()
{
	rnss_t *p = &xRNSS;
	rnss_def_t *pDef = &tbl_bspRnss;

#if RNSS_RST_ENABLE
	sys_GpioConf(&pDef->rst);
#endif
#if RNSS_1PPS_ENABLE
	sys_GpioConf(&pDef->onepps);
#endif
	p->uart = uart_Open(RNSS_UART_ID, OS_TMO_FOREVER);
}



void rnss_Run()
{
	rnss_t *p = &xRNSS;
	int len;

	p->tmo += 1;

	switch (p->ste)
	{
	default:
		uart_Config(p->uart, RNSS_UART_BAUD, UART_PARI_NO, UART_DATA_8D, UART_STOP_1D);

#if RNSS_RST_ENABLE
		rnss_Rst(0);
		os_thd_slp1tick();
		rnss_Rst(1);
#endif

		p->ste = RDRN_S_ONLINE;
		break;
		
	case RDRN_S_ONLINE:

		//接受定位数据
		do
		{
			uart_Recive(p->uart, p->rbuf);

			len = buffbd(p->rbuf, 300);
			if (len)
			{
				rnss_RxParse(p, (char *)&p->rbuf->p[3]);
				buf_Remove(p->rbuf, len);
				
				p->tmo = 0;
			}
		} while (len);
			
		if (p->tmo > 60)
			p->ste = RDRN_S_RESET;

		break;
	}
}

void rnss_GetLocation(int *latitude, int *longitud)
{
	
	*latitude = xRNSS.latitude;
	*longitud = xRNSS.longitud;
}

time_t rnss_GetTimet()
{	

	return xRNSS.time;
}

#endif








