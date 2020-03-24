#include <string.h>

#if UART_ENABLE


static UART_TypeDef * const str7_tblUartId[] =
{
	UART0,
	UART1,
	UART2,
	UART3
};



//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void arch_UartInit(uart_t *p)
{
	uart_para_t xUart;
	GpioPinMode_TypeDef nPinMode;
	UART_TypeDef *pUart;

	pUart = str7_tblUartId[p->def->id];
	/* Enable the UART Interrupt */
	switch (p->def->id)
	{
	case 0:
		EIC->SIR[UART0_IRQChannel] = ((int)UART0_IRQHandler << 16) | 7;
		EIC_IRQChannelConfig(UART0_IRQChannel, ENABLE);
		break;
	case 1:		
		EIC->SIR[UART1_IRQChannel] = ((int)UART1_IRQHandler << 16) | 6;
		EIC_IRQChannelConfig(UART1_IRQChannel, ENABLE);
		break;
	case 2:
		EIC->SIR[UART2_IRQChannel] = ((int)UART2_IRQHandler << 16) | 5;
		EIC_IRQChannelConfig(UART2_IRQChannel, ENABLE);
		break;
	case 3:
		EIC->SIR[UART3_IRQChannel] = ((int)UART3_IRQHandler << 16) | 4;
		EIC_IRQChannelConfig(UART3_IRQChannel, ENABLE);
		break;
	default:
		break;
	}

	//Tx
	if (p->def->pinmode == DEV_PIN_OD)
		nPinMode = GPIO_AF_OD;
	else
		nPinMode = GPIO_AF_PP;
	GPIO_Config(str7_GpioPortDef(p->def->txport), BITMASK(p->def->txpin), nPinMode);
	//Rx
	GPIO_Config(str7_GpioPortDef(p->def->rxport), BITMASK(p->def->rxpin), GPIO_IN_TRI_CMOS);

	//Config
	UART_FifoConfig(pUart, ENABLE);
	UART_LoopBackConfig(pUart, DISABLE);
	UART_RxConfig(pUart, ENABLE);
	UART_TimeOutPeriodConfig(pUart, 0xFF);
	xUart.baud = 115200;
	xUart.pari = UART_PARI_EVEN;
	xUart.data = UART_DATA_8D;
	xUart.stop = UART_STOP_1_5D;
	arch_UartOpen(p, &xUart);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res arch_UartOpen(uart_t *p, uart_para_t *pPara)
{
	UARTParity_TypeDef nParity;
	UARTStopBits_TypeDef nStopbits;
	UARTMode_TypeDef nMode;
	UART_TypeDef *pUart = str7_tblUartId[p->def->id];

	switch (pPara->stop)
	{
	case UART_STOP_0_5D:
		nStopbits = UART_0_5_StopBits;
		break;
	case UART_STOP_1_5D:
		nStopbits = UART_1_5_StopBits;
		break;
	case UART_STOP_2D:
		nStopbits = UART_2_StopBits;
		break;
	case UART_STOP_1D:
	default:
		nStopbits = UART_1_StopBits;
		break;
	}
	
	switch (pPara->pari)
	{
	case UART_PARI_EVEN:
		nParity = UART_EVEN_PARITY;
		break;
	case UART_PARI_ODD:
		nParity = UART_ODD_PARITY;
		break;
	case UART_PARI_NO:
	default:
		nParity = UART_NO_PARITY;
		break;
	}
	
	switch (pPara->data)
	{
	case UART_DATA_7D:
		nMode = UARTM_7D_P;
		break;
	case UART_DATA_8D:
	default:
		if (pPara->pari == UART_PARI_NO)
			nMode = UARTM_8D;
		else
			nMode = UARTM_8D_P;
		break;
	}
	
	UART_Config(pUart, pPara->baud, nParity, nStopbits, nMode);
	//Enable USART Receive interrupts
	UART_ItConfig(pUart, UART_RxHalfFull | UART_TimeOutNotEmpty, ENABLE);
	//Enable the USART
	UART_OnOffConfig(pUart, ENABLE);

	return SYS_R_OK;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void arch_UartTxIEnable(uart_t *p)
{

	str7_tblUartId[p->def->id]->IER |= UART_TxHalfEmpty | UART_TxEmpty;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void arch_UartIoHandler(uint nId)
{
	UART_TypeDef *pUart = str7_tblUartId[nId];
	u8 arrTemp[16], *pTemp = arrTemp;
	int i;

	while (pUart->SR & UART_RxBufFull)
	{
		*pTemp++ = pUart->RxBUFR;
		if (pTemp >= ARR_ENDADR(arrTemp))
		{
#if IO_BUF_TYPE == BUF_T_BUFFER
			buf_Push(&p->bufrx, arrTemp, sizeof(arrTemp));
#else
			dque_Push(p->parent.id | UART_DQUE_RX_CHL, arrTemp, sizeof(arrTemp));
#endif
			pTemp = arrTemp;
		}
	}
	
	if (pTemp > arrTemp)
	{
#if IO_BUF_TYPE == BUF_T_BUFFER
		buf_Push(&p->bufrx, arrTemp, pTemp - arrTemp);
#else
		dque_Push(p->parent.id | UART_DQUE_RX_CHL, arrTemp, pTemp - arrTemp);
#endif
	}
	if (pUart->IER & UART_TxEmpty)
	{
#if IO_BUF_TYPE == BUF_T_BUFFER
		while (pUart->SR & UART_TxEmpty)
		{
			if (p->buftx->len == 0)
			{
				pUart->IER &= ~(UART_TxHalfEmpty | UART_TxEmpty);
				break;
			}
			
			for (i = 0; i < p->buftx->len; i++)
			{
				if ((pUart->SR & UART_TxEmpty) == 0)
					break;
				pUart->TxBUFR = p->buftx->p[i];
			}
			buf_Remove(p->buftx, i);
		}
#else
		i = dque_PopChar(p->parent.id | UART_DQUE_TX_CHL);
		if (i < 0)
		{
			pUart->IER &= ~(UART_TxHalfEmpty | UART_TxEmpty);
			break;
		}
		pUart->TxBUFR = i;
#endif
	}
}

void arch_UartScReset(uart_t *p, uint nHL)
{

}

#endif


