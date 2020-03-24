


//Private Defines
#define SERIAL_USARTX				UART1


/*----------------------------------------------------------------------------
  Write character to Serial Port
 *----------------------------------------------------------------------------*/
int sendchar(int ch)
{

	while (SERIAL_USARTX->FSR.TE_FLAG != 1);					/* Wait Tx empty */
	SERIAL_USARTX->DATA = ch;									/* Send UART Data from buffer */

	return ch;
}



