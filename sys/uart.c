



//Private Defines



//Public Variables
uart_t dev_Uart[BSP_UART_QTY];



//-------------------------------------------------------------------------
//Internal Functions
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
static int uart_RxLen(uart_t *p)
{

	return dque_GetLen(p->parent.id | UART_DQUE_RX_CHL);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
#if UART_IRQ_TX_EN
static int uart_TxLen(uart_t *p)
{

	return dque_GetLen(p->parent.id | UART_DQUE_TX_CHL);
}
#endif





//-------------------------------------------------------------------------
//External Functions
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void uart_Init(uart_t *p)
{

	switch (p->def->type)
	{
#if SC16IS7X_ENABLE
	case UART_T_SC16IS7X:
		break;
#endif
#if VK321X_ENABLE
	case UART_T_VK321X:
		break;
#endif
#if SWUART_ENABLE
	case UART_T_TIMER:
		swuart_Init(p);
		break;
#endif
	default:
		arch_UartInit(p);
		break;
	}
}

//-------------------------------------------------------------------------
//Function Name  : 
//Description    : 获得串口指针,串口号,超时
//Input          : - GPIOx:
//                - GPIO_InitStruct: 
//Output         : None
//Return         : None
//-------------------------------------------------------------------------
uart_t *uart_Open(int nId, size_t nTmo)
{
	uart_t *p;

#if OS_TYPE
	p = &dev_Uart[nId];
	if (dev_Open(&p->parent, nTmo) == SYS_R_OK)
		return p;
#endif

	return NULL;
}

//-------------------------------------------------------------------------
//Function Name  : 
//Description    : 
//Input          : - GPIOx:
//                - GPIO_InitStruct: 
//Output         : None
//Return         : None
//-------------------------------------------------------------------------
sys_res uart_Close(uart_t *p)
{

#if OS_TYPE
	
	return dev_Close(&p->parent);
#else

	return SYS_R_OK;
#endif
}


//-------------------------------------------------------------------------
//Function Name  : 
//Description    : 
//Input          : - GPIOx:
//                - GPIO_InitStruct: 
//Output         : None
//Return         : None
//-------------------------------------------------------------------------
sys_res uart_Config(uart_t *p, int nBaud, int nPari, int nData, int nStop)
{
	sys_res res;
	uart_para_t xPara;
	t_uart_def *pDef;
	
	xPara.baud = nBaud;
	xPara.pari = nPari;
	xPara.data = nData;
	xPara.stop = nStop;

	if (memcmp(&p->para, &xPara, sizeof(p->para)))
	{
		memcpy(&p->para, &xPara, sizeof(p->para));
		pDef = p->def;
		switch (pDef->type)
		{
#if SC16IS7X_ENABLE
		case UART_T_SC16IS7X:
			res = sc16is7x_UartConfig(pDef->id, &p->para);
			break;
#endif
#if VK321X_ENABLE
		case UART_T_VK321X:
			res = vk321x_UartConfig(pDef->id, &p->para);
			break;
#endif
#if SWUART_ENABLE
		case UART_T_TIMER:
			res = swuart_Open(pDef->id, &p->para);
			break;
#endif
		default:
			res = arch_UartOpen(pDef->id, &p->para);
			break;
		}
		
		return res;
	}

	return SYS_R_OK;
}


//-------------------------------------------------------------------------
//Function Name  : 
//Description    : 
//Input          : - GPIOx:
//                - GPIO_InitStruct: 
//Output         : None
//Return         : None
//-------------------------------------------------------------------------
sys_res uart_Send(uart_t *p, const void *pData, size_t nLen)
{
	const u8 *pd = (const u8 *)pData, *end = pd + nLen;

	switch (p->def->type)
	{
#if SC16IS7X_ENABLE
	case UART_T_SC16IS7X:
		sc16is7x_UartSend(p->def->id, pData, nLen);
		break;
#endif

#if VK321X_ENABLE
	case UART_T_VK321X:
		vk321x_UartSend(p->def->id, pData, nLen);
		break;
#endif

#if SWUART_ENABLE
	case UART_T_TIMER:
#if SWUART_RX_MODE == SWUART_RX_M_EINT
		while (pd < end)
		{
			dque_Push(p->parent.id | UART_DQUE_TX_CHL, *pd++);
		}
		swuart_TxStart(p->def->id);
#else
		swuart_Send(p->def->id, pData, nLen);
#endif
		break;
#endif

	default:
#if UART_IRQ_TX_EN
		if (p->def->txmode == UART_MODE_IRQ)
		{
			while (pd < end)
			{
				dque_Push(p->parent.id | UART_DQUE_TX_CHL, *pd++);
			}
			arch_UartTxIEnable(p->def->id);
		}
		else
#endif
		{
			while (pd < end)
			{
				arch_UartSendChar(p->def->id, *pd++);
			}
		}
		break;
	}	

	return SYS_R_OK;
}

//-------------------------------------------------------------------------
//Function Name  : 
//Description    : 
//Input          : - GPIOx:
//                - GPIO_InitStruct: 
//Output         : None
//Return         : None
//-------------------------------------------------------------------------
sys_res uart_SendStr(uart_t *p, const char *str)
{

	return uart_Send(p, str, strlen(str));
}

//-------------------------------------------------------------------------
//Function Name  : 
//Description    : 
//Input          : - GPIOx:
//                - GPIO_InitStruct: 
//Output         : None
//Return         : None
//-------------------------------------------------------------------------
int uart_Recive(uart_t *p, buf b)
{
	
	return dque_Pop2Buf(p->parent.id | UART_DQUE_RX_CHL, b);
}


//-------------------------------------------------------------------------
//Function Name  : 
//Description    : 
//Input          : - GPIOx:
//                - GPIO_InitStruct: 
//Output         : None
//Return         : None
//-------------------------------------------------------------------------
sys_res uart_RecTmo(uart_t *p, buf b, size_t nTmo)
{

	for (nTmo /= OS_TICK_MS; ; nTmo--)
	{
		if (uart_Recive(p, b) > 0)
			return SYS_R_OK;

		if (nTmo == 0)
			return SYS_R_TMO;

		os_thd_slp1tick();
	}
}


//-------------------------------------------------------------------------
//Function Name  : 
//Description    : 
//Input          : - GPIOx:
//                - GPIO_InitStruct: 
//Output         : None
//Return         : None
//-------------------------------------------------------------------------
int uart_RecLen(uart_t *p, buf b, size_t nLen, size_t nTmo)
{

	for (nTmo /= OS_TICK_MS; nTmo; nTmo--)
	{
		if (uart_RxLen(p) >= nLen)
			return uart_Recive(p, b);

		os_thd_slp1tick();
	}

	return 0;
}


//-------------------------------------------------------------------------
//Function Name  : 
//Description    : 
//Input          : - GPIOx:
//                - GPIO_InitStruct: 
//Output         : None
//Return         : None
//-------------------------------------------------------------------------
int uart_Read(uart_t *p, void *pData, size_t nLen)
{

	return dque_Pop(p->parent.id | UART_DQUE_RX_CHL, pData, nLen);
}


//-------------------------------------------------------------------------
//Function Name  : 
//Description    : 
//Input          : - GPIOx:
//                - GPIO_InitStruct: 
//Output         : None
//Return         : None
//-------------------------------------------------------------------------
int uart_GetChar(uart_t *p)
{

	return dque_PopChar(p->parent.id | UART_DQUE_RX_CHL);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
#if SMARTCARD_ENABLE
sys_res uart_ScReset(uart_t *p, int nHL)
{
	t_uart_def *pDef;

	pDef = p->def;
	if (pDef->fun != UART_FUN_SC)
		return SYS_R_ERR;

	switch (pDef->type)
	{
#if SWUART_SC_ENABLE
	case UART_T_TIMER:
		arch_GpioSet(pDef->fport, pDef->fpin, nHL);
		break;
#endif
	default:
		arch_GpioSet(pDef->rxport, pDef->rxpin, nHL);
		break;
	}

	return SYS_R_OK;
}
#endif



//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
#if UART_IRQ_TX_EN || DEV_IDLECNT_ENABLE
void uart_Maintian()
{
	uart_t *p;
	
	for (p = dev_Uart; p < ARR_ENDADR(dev_Uart); p++)
	{

#if UART_IRQ_TX_EN
		if (uart_TxLen(p))
		{
			t_uart_def *pDef = p->def;
			
			if (pDef->txmode == UART_MODE_IRQ)
			{
				switch (pDef->type)
				{
#if SWUART_ENABLE
#if SWUART_RX_MODE == SWUART_RX_M_EINT
				case UART_T_TIMER:
					swuart_TxStart(pDef->id);
					break;
#endif
#endif
				default:
					arch_UartTxIEnable(pDef->id);
					break;
				}
			}
		}
#endif

#if DEV_IDLECNT_ENABLE
		if (uart_RxLen(p))
		{
			if (p->parent.idlecnt++ > 100)
			{
				p->parent.idlecnt = 0;
				dque_Clear(p->parent.id | UART_DQUE_RX_CHL);
			}
		}
		else
		{
			p->parent.idlecnt = 0;
		}
#endif
	}
}
#else
#define uart_Maintian(...)
#endif

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void uart_Reopen()
{
	uart_t *p;
	uart_para_t xPara;

	for (p = dev_Uart; p < ARR_ENDADR(dev_Uart); p++)
	{
		memcpy(&xPara, &p->para, sizeof(uart_para_t));
		memset(&p->para, 0, sizeof(uart_para_t));
		uart_Config(p, xPara.baud, xPara.pari, xPara.data, xPara.stop);
	}
}


