
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
			RDSS_DBGOUT("[RD<] ");
			RDSS_DBGOUT((char *)p->rbuf->p);
	
			rdss_RxParse(p, (char *)&p->rbuf->p[3]);
			buf_Remove(p->rbuf, l);
	
			p->tmo = 0;
		}
	} while (l);
}



u32 rdss_GetSIM()
{

	return 1;
}

int rdss_GetSignal()
{

//	return xRDRN.signal;
	return 7;
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








