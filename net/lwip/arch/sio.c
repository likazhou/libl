

#if MODEM_ENABLE
#define UART_PTR				&dev_Uart[MODEM_UART_ID]
#endif
#if ATSVR_ENABLE
#define UART_PTR				&dev_Uart[ATSVR_UART_ID]
#endif


static volatile u32_t sio_counter;

/**
 * Opens a serial device for communication.
 * 
 * @param devnum device number
 * @return handle to serial device if successful, NULL otherwise
 */
sio_fd_t sio_open(u8_t devnum)
{
	return 0;
}


/**
 * Sends a single character to the serial device.
 * 
 * @param c character to send
 * @param fd serial device handle
 * 
 * @note This function will block until the character can be sent.
 */
void sio_send(u8_t c, sio_fd_t fd)
{

	uart_Send(UART_PTR, &c, 1);
#if MODEM_FLOWCTL_ENABLE
	modem_FlowStep(1);
#endif
}


/**
 * Receives a single character from the serial device.
 * 
 * @param fd serial device handle
 * 
 * @note This function will block until a character is received.
 */
u8_t sio_recv(sio_fd_t fd)
{
	int c;

	for (sio_counter = 1; sio_counter; )
	{
		c = uart_GetChar(UART_PTR);

		if (c >= 0)
		{
#if MODEM_FLOWCTL_ENABLE
			modem_FlowStep(0);
#endif
			return c;
		}
		
		os_thd_slp1tick();
	}

	return 0;
}


/**
 * Reads from the serial device.
 * 
 * @param fd serial device handle
 * @param data pointer to data buffer for receiving
 * @param len maximum length (in bytes) of data to receive
 * @return number of bytes actually received - may be 0 if aborted by sio_read_abort
 * 
 * @note This function will block until data can be received. The blocking
 * can be cancelled by calling sio_read_abort().
 */
u32_t sio_read(sio_fd_t fd, u8_t *data, u32_t len)
{
	int c;

	for (sio_counter = 1; sio_counter; os_thd_slp1tick())
	{
		c = uart_Read(UART_PTR, data, len);
		if (c > 0)
			break;
	}
#if MODEM_FLOWCTL_ENABLE
	modem_FlowStep(0);
#endif
	
	return c;
}


/**
 * Tries to read from the serial device. Same as sio_read but returns
 * immediately if no data is available and never blocks.
 * 
 * @param fd serial device handle
 * @param data pointer to data buffer for receiving
 * @param len maximum length (in bytes) of data to receive
 * @return number of bytes actually received
 */
u32_t sio_tryread(sio_fd_t fd, u8_t *data, u32_t len)
{
	u32_t i;
	int c;
	
	for (i = 0; i < len; i++)
	{
		c = uart_GetChar(UART_PTR);
		if (c < 0)
			break;
		*data++ = c;
	}
	
#if MODEM_FLOWCTL_ENABLE
	modem_FlowStep(0);
#endif
	
	return i;
}


/**
 * Writes to the serial device.
 * 
 * @param fd serial device handle
 * @param data pointer to data to send
 * @param len length (in bytes) of data to send
 * @return number of bytes actually sent
 * 
 * @note This function will block until all data can be sent.
 */
u32_t sio_write(sio_fd_t fd, u8_t *data, u32_t len)
{

	if (uart_Send(UART_PTR, data, len) == SYS_R_OK)
	{
#if MODEM_FLOWCTL_ENABLE
		modem_FlowStep(1);
#endif
		return len;
	}
	
	return 0;
}

/**
 * Aborts a blocking sio_read() call.
 * 
 * @param fd serial device handle
 */
void sio_read_abort(sio_fd_t fd)
{

	sio_counter = 0;
}



