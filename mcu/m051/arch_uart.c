


//Private Defines
#define M051_UART_QTY			2

//Private Typedef


//Private Const
static UART_T * const m051_tblUartBase[M051_UART_QTY] = {
	UART0,
	UART1
};

//Private Variables



static void mo51_BaudRateCalculator(u32 clk, u32 baudRate, UART_BAUD_T *baud)
{
  	int32_t tmp;
	int32_t div;

	if (((clk / baudRate) % 16) < 2)	   /* Source Clock mod 16 < 2 => Using Divider X =16 (MODE#0) */ 
	{								  
		baud->DIV_X_EN = 0;
	    baud->DIV_X_ONE = 0;
		tmp = clk / baudRate / 16 - 2;
	}
	else							  /* Source Clock mod 16 >2 => Up 5% Error BaudRate */
	{			 
	    baud->DIV_X_EN = 1;			  /* Try to Set Divider X = 1 (MODE#2)*/
	    baud->DIV_X_ONE = 1;
		tmp = clk / baudRate - 2;

		if (tmp > 0xFFFF)			  /* If Divider > Range  */
		{
			baud->DIV_X_EN = 1;		  /* Try to Set Divider X up 10 (MODE#1) */
			baud->DIV_X_ONE = 0;

			for (div = 8; div < 16; div++)
			{
				if (((clk / baudRate) % (div + 1)) < 3)
				{
					baud->DIVIDER_X = div;
					tmp = clk / baudRate / (div + 1) - 2;
					break;
				}
			}
		}
	}

	baud->BRD = tmp; 
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void arch_UartInit(uart_t *p)
{
	uart_para_t xUart;

	/* Enable UART clock and interrupt */
	if (p->def->id)
	{
		SYSCLK->APBCLK.UART1_EN = 1;
		NVIC_SetPriority(UART1_IRQn, (1<<__NVIC_PRIO_BITS) - 2);
		NVIC_EnableIRQ(UART1_IRQn);
	}
	else
	{
		SYSCLK->APBCLK.UART0_EN = 1;
		NVIC_SetPriority(UART0_IRQn, (1<<__NVIC_PRIO_BITS) - 2);
		NVIC_EnableIRQ(UART0_IRQn);
	}

	/* set UART pin */
	switch (p->def->id)
	{
	case 1:
		__raw_writel((__raw_readl(&SYS->P1_MFP) | (0x3<<10)) & ~(0x3<<2), &SYS->P1_MFP);
		break;
	default:
		__raw_writel((__raw_readl(&SYS->P3_MFP) & ~(0x3<<8)) | (0x3), &SYS->P3_MFP);
		break;
	}
	
	xUart.baud = 115200;
	xUart.pari = UART_PARI_NO;
	xUart.data = UART_DATA_8D;
	xUart.stop = UART_STOP_1D;
	arch_UartOpen(p->def->id, &xUart);
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res arch_UartOpen(uint nId, uart_para_t *pPara)
{
	UART_T *pUart = m051_tblUartBase[nId];
	uart_t *p = &dev_Uart[nId];

	/* Reset IP */
	if (nId)
	{
		SYS->IPRSTC2.UART1_RST = 1;
		SYS->IPRSTC2.UART1_RST = 0;
	}
	else
	{
		SYS->IPRSTC2.UART0_RST = 1;
		SYS->IPRSTC2.UART0_RST = 0;
	}

	/* Disable RTS/CTS */
	pUart->IER.AUTO_CTS_EN = 0;
	pUart->IER.AUTO_RTS_EN = 0;

	/* Tx FIFO Reset & Rx FIFO Reset & FIFO Mode Enable */
	pUart->FCR.TFR = 1;
	pUart->FCR.RFR = 1;

	/* Set Rx Trigger Level */
	pUart->FCR.RFITL = 0;

	/* Set Parity & Data bits & Stop bits */
	pUart->LCR.SPE = 0;
	switch (pPara->pari)
	{
	case UART_PARI_EVEN:
		pUart->LCR.EPE = 1;
		pUart->LCR.PBE = 1;
		break;
		
	case UART_PARI_ODD:
		pUart->LCR.EPE = 0;
		pUart->LCR.PBE = 1;
		break;
		
	default:
		pUart->LCR.EPE = 0;
		pUart->LCR.PBE = 0;
		break;
	}
	
	switch (pPara->stop)
	{
	case UART_STOP_2D:
		pUart->LCR.NSB = 1;
		break;
		
	default:
		pUart->LCR.NSB = 0;
		break;
	}
	
	switch (pPara->data)
	{
	case UART_DATA_7D:
		pUart->LCR.WLS = 2;
		break;
		
	default:
		pUart->LCR.WLS = 3;
		break;
	}
		
	/* Set BaudRate */
	mo51_BaudRateCalculator(XTAL, pPara->baud, &pUart->BAUD);

	/* Enable Interrupt */
	pUart->IER.THRE_IEN = 1;
	if (p->def->rxmode == UART_MODE_IRQ)
		pUart->IER.RDA_IEN = 1;

	return SYS_R_OK;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void arch_UartTxIEnable(uint nId)
{
	UART_T *pUart = m051_tblUartBase[nId];

	pUart->IER.THRE_IEN = 1;
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
	UART_T *pUart = m051_tblUartBase[nId];

	/* THRE status, contain valid data */
	while (pUart->FSR.TE_FLAG == 0);
	/* write data */
	pUart->DATA = nData;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
int arch_UartGetChar(uint nId)
{
	UART_T *pUart = m051_tblUartBase[nId];
	
	while (pUart->FSR.RX_EMPTY == 0);
	
	return pUart->DATA;
}



//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void arch_UartISR(uint nId)
{
	UART_T *pUart = m051_tblUartBase[nId];
	uart_t *p = &dev_Uart[nId];
	u8 aTemp[16], *pTemp = aTemp;
	int i;

	while (pUart->FSR.RX_EMPTY == 0)
	{
		*pTemp++ = pUart->DATA;
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

	if (pUart->IER.THRE_IEN)
	{
#if IO_BUF_TYPE == BUF_T_BUFFER
		while (pUart->FSR.TE_FLAG)
		{
			if (p->btx->len == 0)
			{
				pUart->IER.THRE_IEN = 0;
				break;
			}
			
			for (i = 0; i < p->btx->len; i++)
			{
				if ((pUart->LSR & LSR_THRE) == 0)
					break;
				pUart->DATA = p->btx->p[i];
			}
			buf_Remove(p->btx, i);
		}
#else
		while (pUart->FSR.TE_FLAG)
		{
			i = dque_PopChar(p->parent.id | UART_DQUE_TX_CHL);
			if (i < 0)
			{
				pUart->IER.THRE_IEN = 0;
				break;
			}
			pUart->DATA = i;
		}
#endif
	}
}



