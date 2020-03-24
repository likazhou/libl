


//Private Defines
#define SERIAL_USARTX				UART0_BASE


/*----------------------------------------------------------------------------
  Write character to Serial Port
 *----------------------------------------------------------------------------*/
int sendchar(int ch)
{

	while (MAP_UARTCharPutNonBlocking(SERIAL_USARTX, ch) == false);
	return ch;
}


