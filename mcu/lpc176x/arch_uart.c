


//Private Defines
#define IER_RBR		0x01
#define IER_THRE	0x02
#define IER_RLS		0x04

#define IIR_PEND	0x01

#define LSR_RDR		0x01
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_RXFE	0x80


//Private Typedef



//Private Const
static void * const lpc176x_tblUartBase[] = {
	LPC_UART0,
	LPC_UART1,
	LPC_UART2,
	LPC_UART3
};

//Private Variables




//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void arch_UartInit(uart_t *p)
{
	uart_para_t xUart;
	uint nTxMode, nRxMode, nSel = 0;
	t_uart_def *pDef = p->def;

	/* enable UART clock and interrupt*/
	switch (pDef->id)
	{
	case 0:
		SETBIT(LPC_SC->PCONP, 3);
		NVIC_EnableIRQ(UART0_IRQn);
		break;
	case 1:
		SETBIT(LPC_SC->PCONP, 4);
		NVIC_EnableIRQ(UART1_IRQn);
		break;
	case 2:
		SETBIT(LPC_SC->PCONP, 24);
		NVIC_EnableIRQ(UART2_IRQn);
		break;
	case 3:
		SETBIT(LPC_SC->PCONP, 25);
		NVIC_EnableIRQ(UART3_IRQn);
		break;
	default:
		break;
	}

	/* set UART pin */
	if (pDef->pinmode == DEV_PIN_PP)
	{
		nRxMode = GPIO_M_IN_PU;
		nTxMode = GPIO_M_OUT_PP;
	}
	else
	{
		nRxMode = GPIO_M_IN_FLOAT;
		nTxMode = GPIO_M_OUT_OD;
	}
	arch_GpioConf(pDef->rxport, pDef->rxpin, nRxMode, GPIO_INIT_HIGH);
	arch_GpioConf(pDef->txport, pDef->txpin, nTxMode, GPIO_INIT_HIGH);

	switch (pDef->id)
	{
	case 0:
		switch (pDef->rxport)
		{
		case GPIO_P0:
			nSel = 1;
			break;
		default:
			break;
		}
		break;
		
	case 1:
	case 2:
		switch (pDef->rxport)
		{
		case GPIO_P0:
			nSel = 1;
			break;
		case GPIO_P2:
			nSel = 2;
			break;
		default:
			break;
		}
		break;
		
	case 3:
		switch (pDef->rxpin)
		{
		case 1:
			nSel = 2;
			break;
		case 26:
		case 29:
			nSel = 3;
			break;
		default:
			break;
		}
		break;
		
	default:
		break;
	}
	
	arch_GpioSel(pDef->rxport, pDef->rxpin, nSel);
	arch_GpioSel(pDef->txport, pDef->txpin, nSel);
	
	xUart.baud = 115200;
	xUart.pari = UART_PARI_NO;
	xUart.data = UART_DATA_8D;
	xUart.stop = UART_STOP_1D;
	arch_UartOpen(pDef->id, &xUart);
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res arch_UartOpen(uint nId, uart_para_t *pPara)
{
	LPC_UART_TypeDef *pUart = lpc176x_tblUartBase[nId];
	uart_t *p = &dev_Uart[nId];
	uint nDiv, nMode = 0;

	switch (pPara->stop)
	{
	case UART_STOP_2D:
		SETBIT(nMode, 2);
		break;
	default:
		break;
	}
	
	switch (pPara->pari)
	{
	case UART_PARI_EVEN:
		nMode |= (BITMASK(3) | BITMASK(4));
		break;
	case UART_PARI_ODD:
		nMode |= BITMASK(3);
		break;
	default:
		break;
	}
	
	switch (pPara->data)
	{
	case UART_DATA_7D:
		nMode |= BITMASK(1);
		break;
	default:
		nMode |= (BITMASK(0) | BITMASK(1));
		break;
	}
	
	nDiv = (PERI_CLOCK / 16) / pPara->baud;	/*baud rate */
	pUart->LCR = 0x80;		/* DLAB = 1*/
	pUart->DLM = nDiv / 256;
	pUart->DLL = nDiv % 256;
	
	if (pPara->baud > 57600)
		pUart->FDR = 0xF1;
	else
		pUart->FDR = 0x10;
	pUart->LCR = nMode; 	/* DLAB = 0*/
	pUart->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */

	while ((pUart->LSR & (LSR_THRE | LSR_TEMT)) != (LSR_THRE | LSR_TEMT));
	pUart->IER = IER_RLS;	/* Enable UART interrupt */
	if (p->def->rxmode == UART_MODE_IRQ)
		pUart->IER |= IER_RBR;

	return SYS_R_OK;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void arch_UartTxIEnable(uint nId)
{
	LPC_UART_TypeDef *pUart = lpc176x_tblUartBase[nId];

	pUart->IER |= IER_THRE;
	arch_UartISR(nId);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void arch_UartScReset(uint nId, uint nHL)
{

}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void arch_UartSendChar(uint nId, const int nData)
{
	LPC_UART_TypeDef *pUart = lpc176x_tblUartBase[nId];

	/* THRE status, contain valid data */
	while ((pUart->LSR & LSR_THRE) == 0);
	/* write data */
	pUart->THR = nData;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
int arch_UartGetChar(uint nId)
{
	LPC_UART_TypeDef *pUart = lpc176x_tblUartBase[nId];

	while ((pUart->LSR & LSR_RDR) == 0);
	return pUart->RBR;
}



//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void arch_UartISR(uint nId)
{
	LPC_UART_TypeDef *pUart = lpc176x_tblUartBase[nId];
	uart_t *p = &dev_Uart[nId];
	u8 aTemp[16], *pTemp = aTemp;
	int i;

	if ((pUart->IIR & IIR_PEND) == 0)
	{
		while (pUart->LSR & LSR_RDR)
		{
	        *pTemp++ = pUart->RBR;
			if (pTemp >= ARR_ENDADR(aTemp))
			{
#if IO_BUF_TYPE == BUF_T_BUFFER
				buf_Push(p->brx, aTemp, sizeof(aTemp));
#else
				dque_Push(p->parent.id | UART_DQUE_RX_CHL, aTemp, sizeof(aTemp));
#endif
				pTemp = aTemp;
			}
	    }
		
		if (pTemp > aTemp)
		{
#if IO_BUF_TYPE == BUF_T_BUFFER
			buf_Push(p->brx, aTemp, pTemp - aTemp);
#else
			dque_Push(p->parent.id | UART_DQUE_RX_CHL, aTemp, pTemp - aTemp);
#endif
		}
	}

	if (pUart->IER & IER_THRE)
	{
#if IO_BUF_TYPE == BUF_T_BUFFER
		while (pUart->LSR & LSR_THRE)
		{
			if (p->btx->len == 0)
			{
				pUart->IER &= ~IER_THRE;
				break;
			}
			for (i = 0; i < p->btx->len; i++)
			{
				if ((pUart->LSR & LSR_THRE) == 0)
					break;
				pUart->THR = p->btx->p[i];
			}
			buf_Remove(p->btx, i);
		}
#else
		while (pUart->LSR & LSR_THRE)
		{
			i = dque_PopChar(p->parent.id | UART_DQUE_TX_CHL);
			if (i < 0)
			{
				pUart->IER &= ~IER_THRE;
				break;
			}
			pUart->THR = i;
		}
#endif
	}
}



