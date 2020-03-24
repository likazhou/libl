
//Private Defines
#define SERIAL_USARTX				UART1


/*----------------------------------------------------------------------------
  Write character to Serial Port
 *----------------------------------------------------------------------------*/
//Defined in STR71X.LIB
/*void sendchar(char *ch)
{

	while ((SERIAL_USARTX->SR & UART_TxEmpty) == 0);
	SERIAL_USARTX->TxBUFR = (*ch & 0x1FF);
}*/

/*----------------------------------------------------------------------------
  Read character from Serial Port   (blocking read)
 *----------------------------------------------------------------------------*/
int getkey()
{

	while ((SERIAL_USARTX->SR & UART_RxBufFull) == 0);
	return (int)(SERIAL_USARTX->RxBUFR & 0x1FF);
}


