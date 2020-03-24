


//Private Defines
#define SERIAL_USARTX				LPC_UART0


/*----------------------------------------------------------------------------
  Write character to Serial Port
 *----------------------------------------------------------------------------*/
int sendchar(int ch)
{

	/* THRE status, contain valid data */
	while ((SERIAL_USARTX->LSR & BITMASK(5)) == 0);
	/* write data */
	SERIAL_USARTX->THR = ch;

	return ch;
}





