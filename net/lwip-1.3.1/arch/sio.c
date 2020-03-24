
static volatile uint_t sio_counter;

/**
 * Opens a serial device for communication.
 * 
 * @param devnum device number
 * @return handle to serial device if successful, NULL otherwise
 */
sio_fd_t sio_open(u8_t devnum)
{

	return uart_Get(devnum, OS_TICK_MS);
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

	uart_Send(fd, &c, 1);
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

	for (sio_counter = 1; ((c = uart_GetRxChar(fd)) < 0) && sio_counter; )
		os_thd_Slp1Tick();
	return c;
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
	uart_dev_t *p = (p_dev_uart)fd;
	uint_t i;
#if IO_BUF_TYPE == BUF_T_BUFFER

	for (i = 0, sio_counter = 1; sio_counter; ) {
		i = MIN(p->bufrx->len, len);
		if (i) {
			rt_memcpy(data, p->bufrx->p, i);
			buf_Remove(p->bufrx, i);
			break;
		}
		os_thd_Slp1Tick();
	}
	return i;
#else
	int c;

	for (i = 0, sio_counter = 1; (i < len) && sio_counter; ) {
		c = uart_GetRxChar(p);
		if (c < 0) {
			if (i)
				break;
			os_thd_Slp1Tick();
			continue;
		}
		data[i++] = c;
	}
	return i;
#endif
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
	uint_t i;
	int c;

	for (i = 0; ((c = uart_GetRxChar(fd)) >= 0) && (i < len); i++)
		*data++ = c;
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

	if (uart_Send(fd, data, len) == SYS_R_OK)
		return len;
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



