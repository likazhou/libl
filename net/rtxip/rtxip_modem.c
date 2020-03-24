#if TCPPS_TYPE == TCPPS_T_KEILTCP

#if MODEM_ENABLE
#include <drivers/modem.h>
#define UART_PTR				&dev_Uart[MODEM_UART_ID]
#endif
#if ATSVR_ENABLE
#include <drivers/atserver.h>
#define UART_PTR				&dev_Uart[ATSVR_UART_ID]
#endif


static BOOL tx_active;
/*----------------------------------------------------------------------------
 *      Serial Driver Functions
 *----------------------------------------------------------------------------
 *  Required functions for Serial driver module:
 *   - void init_serial ()
 *   - int  com_getchar ()
 *   - BOOL com_putchar (u8 c)
 *   - BOOL com_tx_active ()
 *   - interrupt function(s)
 *---------------------------------------------------------------------------*/


/*--------------------------- init_serial -----------------------------------*/
void init_serial()
{

	/* Initialize the serial interface */
	tx_active = __FALSE;
}

/*--------------------------- com_getchar -----------------------------------*/
int com_getchar()
{
	int c;

	if (modem_online() != __TRUE)
		return -1;
	
	/* Read a byte from serial interface */
	c = uart_GetChar(UART_PTR);
	
	/* Serial receive buffer is empty. */
	if (c == -1)
		return -1;
	
#if MODEM_FLOWCTL_ENABLE
	modem_FlowStep(0);
#endif

	return c;
}

/*--------------------------- com_putchar -----------------------------------*/
BOOL com_putchar(u8 c)
{

	tx_active = __TRUE;
	uart_Send(UART_PTR, &c, 1);
	tx_active = __FALSE;
	
#if MODEM_FLOWCTL_ENABLE
	modem_FlowStep(1);
#endif

	return __TRUE;
}

/*--------------------------- com_tx_active ---------------------------------*/
BOOL com_tx_active()
{

	/* Return status Transmitter active/not active.         */
	/* When transmit buffer is empty, 'tx_active' is __FALSE. */
	return tx_active;
}

void modem_init()
{

}

void modem_dial(u8 *dialnum)
{

}

void modem_listen()
{

}

void modem_hangup()
{

}

BOOL modem_online()
{

	return modem_IsDataMode() ? __TRUE : __FALSE;
}

BOOL modem_process(u8 ch)
{

	return modem_online();
}

void modem_run()
{

}
#endif

