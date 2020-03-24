#if UART_ENABLE



//Private Defines
#define LM3S_UART_QTY		3

//Private Typedef



//Private Const
static uint const lm3s_tblUartBase[LM3S_UART_QTY] = {
	UART0_BASE,
	UART1_BASE,
	UART2_BASE
};
static uint const lm3s_tblUartSysCtl[LM3S_UART_QTY] = {
	SYSCTL_PERIPH_UART0,
	SYSCTL_PERIPH_UART1,
	SYSCTL_PERIPH_UART2
};
static uint const lm3s_tblUartInt[LM3S_UART_QTY] = {
	INT_UART0,
	INT_UART1,
	INT_UART2
};

//Private Variables




//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void arch_UartInit(uart_t *p)
{
	uart_para_t xUart;
	t_uart_def *pDef = p->def;

	/* enable UART clock */
    MAP_SysCtlPeripheralEnable(lm3s_tblUartSysCtl[pDef->id]);

	/* enable UART interrupt */
	MAP_IntEnable(lm3s_tblUartInt[pDef->id]);

	/* set UART pin */
	arch_GpioClockEnable(pDef->rxport);
	arch_GpioClockEnable(pDef->txport);
	switch (pDef->id)
	{
	case 1:
		switch (pDef->rxport)
		{
		case GPIO_P1:
			if (pDef->rxpin == 0)
			{
				MAP_GPIOPinConfigure(GPIO_PB0_U1RX);
				MAP_GPIOPinConfigure(GPIO_PB1_U1TX);
			}
			else
			{
				MAP_GPIOPinConfigure(GPIO_PB4_U1RX);
				MAP_GPIOPinConfigure(GPIO_PB5_U1TX);
			}
			break;
			
		case GPIO_P2:
			MAP_GPIOPinConfigure(GPIO_PC6_U1RX);
			MAP_GPIOPinConfigure(GPIO_PC7_U1TX);
			break;
			
		case GPIO_P3:
			if (pDef->rxpin == 0)
			{
				MAP_GPIOPinConfigure(GPIO_PD0_U1RX);
				MAP_GPIOPinConfigure(GPIO_PD1_U1TX);
			}
			else
			{
				MAP_GPIOPinConfigure(GPIO_PD2_U1RX);
				MAP_GPIOPinConfigure(GPIO_PD3_U1TX);
			}
			break;
		default:
			break;
		}
		break;
		
	case 2:
		switch (pDef->rxport)
		{
		case GPIO_P3:
#if ARCH_TYPE == ARCH_T_LM3S5X
			MAP_GPIOPinConfigure(GPIO_PD0_U2RX);
			MAP_GPIOPinConfigure(GPIO_PD1_U2TX);
#elif ARCH_TYPE == ARCH_T_LM3S9X
			if (pDef->rxpin == 0)
			{
				MAP_GPIOPinConfigure(GPIO_PD0_U2RX);
				MAP_GPIOPinConfigure(GPIO_PD1_U2TX);
			}
			else
			{
				MAP_GPIOPinConfigure(GPIO_PD5_U2RX);
				MAP_GPIOPinConfigure(GPIO_PD6_U2TX);
			}
#endif
			break;

#if ARCH_TYPE == ARCH_T_LM3S9X
		case GPIO_P6:
			MAP_GPIOPinConfigure(GPIO_PG0_U2RX);
			MAP_GPIOPinConfigure(GPIO_PG1_U2TX);
			break;
#endif

		default:
			break;
		}
		break;
	default:
		break;
	}
	
	MAP_GPIOPinTypeUART(arch_GpioPortBase(pDef->rxport), BITMASK(pDef->rxpin));
	MAP_GPIOPinTypeUART(arch_GpioPortBase(pDef->txport), BITMASK(pDef->txpin));
	
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
	uart_t *p = &dev_Uart[nId];
	uint nBase, nMode = 0;

	nBase = lm3s_tblUartBase[p->def->id];
	switch (pPara->stop)
	{
	case UART_STOP_2D:
		nMode |= UART_CONFIG_STOP_TWO;
		break;
		
	default:
		nMode |= UART_CONFIG_STOP_ONE;
		break;
	}
	
	switch (pPara->pari)
	{
	case UART_PARI_EVEN:
		nMode |= UART_CONFIG_PAR_EVEN;
		break;
		
	case UART_PARI_ODD:
		nMode |= UART_CONFIG_PAR_ODD;
		break;
		
	default:
		nMode |= UART_CONFIG_PAR_NONE;
		break;
	}
	
	switch (pPara->data)
	{
	case UART_DATA_7D:
		nMode |= UART_CONFIG_WLEN_7;
		break;
		
	default:
		nMode |= UART_CONFIG_WLEN_8;
		break;
	}
	
	MAP_UARTConfigSetExpClk(nBase, MAP_SysCtlClockGet(), pPara->baud, nMode);
	MAP_UARTFIFOLevelSet(nBase, UART_FIFO_TX2_8, UART_FIFO_RX6_8);
	if (p->def->rxmode == UART_MODE_IRQ)
		MAP_UARTIntEnable(nBase, UART_INT_RX | UART_INT_RT);
	MAP_UARTEnable(nBase);

	return SYS_R_OK;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void arch_UartTxIEnable(uint nId)
{

	MAP_UARTIntEnable(lm3s_tblUartBase[nId], UART_INT_TX);
	arch_UartISR(nId);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void arch_UartScReset(uint nId, uint nHL)
{

#if 0
	if (nHL)
		GPIO_SetBits(arch_GpioPortBase(p->def->rxport), BITMASK(p->def->rxpin));
	else
		GPIO_ResetBits(arch_GpioPortBase(p->def->rxport), BITMASK(p->def->rxpin));
#endif
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void arch_UartSendChar(uint nId, const int nData)
{

	MAP_UARTCharPut(lm3s_tblUartBase[nId], nData);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
int arch_UartGetChar(uint nId)
{

	return MAP_UARTCharGet(lm3s_tblUartBase[nId]);
}



//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void arch_UartISR(uint nId)
{
	uint nBase = lm3s_tblUartBase[nId];
	uart_t *p = &dev_Uart[nId];
	u8 aTemp[16], *pTemp = aTemp;
	int i;

	p = lm3s_uart_dev[nId];

	MAP_UARTRxErrorClear(nBase);

	while (MAP_UARTCharsAvail(nBase) == true)
	{
        *pTemp++ = MAP_UARTCharGetNonBlocking(nBase);
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

	if (__raw_readl(nBase + UART_O_IM) & UART_INT_TX)
	{
#if IO_BUF_TYPE == BUF_T_BUFFER
		while (MAP_UARTSpaceAvail(nBase) == true)
		{
			if (p->btx->len == 0)
			{
				MAP_UARTIntDisable(nBase, UART_INT_TX);
				break;
			}
			
			for (i = 0; i < p->btx->len; i++)
			{
				if (MAP_UARTSpaceAvail(nBase) == false)
					break;
				
				MAP_UARTCharPutNonBlocking(nBase, p->btx->p[i]);
			}
			
			buf_Remove(p->btx, i);
		}
#else
		while (MAP_UARTSpaceAvail(nBase) == true)
		{
			i = dque_PopChar(p->parent.id | UART_DQUE_TX_CHL);
			if (i < 0)
			{
				MAP_UARTIntDisable(nBase, UART_INT_TX);
				break;
			}
			
			MAP_UARTCharPutNonBlocking(nBase, i);
		}
#endif
	}
}

#endif
