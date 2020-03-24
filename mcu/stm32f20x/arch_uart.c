#if UART_ENABLE





//Private Variables
static USART_TypeDef * const stm32_tblUartId[] =
{
	USART1,
	USART2,
	USART3,
	UART4,
	UART5,
	USART6
};
static u8 uartid_mcu2dev[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};



//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void arch_UartInit(uart_t *p)
{
	uart_para_t xUart;
	t_uart_def *pDef = p->def;
	int nirq;
	GPIO_InitTypeDef xGpio;
	USART_ClockInitTypeDef xUartClock;
	USART_TypeDef *pUart;

	uartid_mcu2dev[pDef->id] = p - &dev_Uart[0];

	pUart = stm32_tblUartId[pDef->id];
	switch (pDef->id)
	{
	case 0:
		/* Enalbe the GPIO Clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		/* Enalbe the USART1  Clock*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		/* Enable the USART1 Interrupt */
		nirq = USART1_IRQn;
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
		if (pDef->fun == UART_FUN_SC)
			GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_USART1);
		else
			GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
		break;
		
	case 1:
		/* Enalbe the GPIO Clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		/* Enalbe the USART1  Clock*/
		RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		/* Enable the USART1 Interrupt */
		nirq = USART2_IRQn;
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
		if (pDef->fun == UART_FUN_SC)
			GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_USART2);
		else
			GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
		break;
		
	case 2:
		/* Enalbe the GPIO Clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		/* Enalbe the USART3  Clock*/
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		/* Enable the USART3 Interrupt */
		nirq = USART3_IRQn;
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
		if (pDef->fun == UART_FUN_SC)
			GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_USART3);
		else
			GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
		break;
		
	case 3:
		/* Enalbe the GPIO Clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		/* Enalbe the USART4  Clock*/
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
		/* Enable the USART4 Interrupt */
		nirq = UART4_IRQn;
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);
		break;
		
	case 4:
		/* Enalbe the GPIO Clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE);
		/* Enalbe the USART5  Clock*/
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
		/* Enable the USART5 Interrupt */
		nirq = UART5_IRQn;
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);
		break;
		
	case 5:
		/* Enalbe the GPIO Clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		/* Enalbe the USART5  Clock*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
		/* Enable the USART5 Interrupt */
		nirq = USART6_IRQn;
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
		if (pDef->fun == UART_FUN_SC)
			GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_USART6);
		else
			GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);
		break;
		
	default:
		return;
	}
	arch_IrqPri(nirq, 0);

	xGpio.GPIO_Speed = GPIO_Speed_50MHz;
	xGpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	//Tx
	xGpio.GPIO_Pin = BITMASK(pDef->txpin);
	xGpio.GPIO_Mode = GPIO_Mode_AF;
#if SMARTCARD_ENABLE
	if (pDef->fun == UART_FUN_SC)
	{
		xGpio.GPIO_OType = GPIO_OType_OD;
	}
	else
#endif
	{
		if (pDef->pinmode == DEV_PIN_OD)
			xGpio.GPIO_OType = GPIO_OType_OD;
		else
			xGpio.GPIO_OType = GPIO_OType_PP;
	}
	stm32_GpioClockEnable(pDef->txport);
	GPIO_Init(arch_GpioPortBase(pDef->txport), &xGpio);
	
	//Rx
	xGpio.GPIO_Pin = BITMASK(pDef->rxpin);
#if SMARTCARD_ENABLE
	if (pDef->fun == UART_FUN_SC)
	{
		xGpio.GPIO_Mode = GPIO_Mode_OUT;
	}
	else
#endif
	{
		xGpio.GPIO_Mode = GPIO_Mode_AF;
	}

	if (pDef->pinmode == DEV_PIN_OD)
		xGpio.GPIO_OType = GPIO_OType_OD;
	else
		xGpio.GPIO_OType = GPIO_OType_PP;

	stm32_GpioClockEnable(pDef->rxport);
	GPIO_Init(arch_GpioPortBase(pDef->rxport), &xGpio);
	
	//Clock
#if SMARTCARD_ENABLE
	if (pDef->fun == UART_FUN_SC)
	{
		//SmartCard时钟372倍波特率,1为PCLK2分频,2为4分频......
#if MCU_FREQUENCY == MCU_SPEED_LOW
		USART_SetPrescaler(pUart, 1);//PCLK=8M
#elif MCU_FREQUENCY == MCU_SPEED_HALF
		USART_SetPrescaler(pUart, 4);//PCLK=30M
#else
		USART_SetPrescaler(pUart, 9);//PCLK=60M
#endif
		USART_SetGuardTime(pUart, 16);

		//Fun
		xGpio.GPIO_Pin = BITMASK(pDef->fpin);
		stm32_GpioClockEnable(pDef->fport);
		xGpio.GPIO_Mode = GPIO_Mode_AF;
		
		if (pDef->pinmode == DEV_PIN_OD)
			xGpio.GPIO_OType = GPIO_OType_OD;
		else
			xGpio.GPIO_OType = GPIO_OType_PP;
		
		GPIO_Init(arch_GpioPortBase(pDef->fport), &xGpio);
		xUartClock.USART_Clock = USART_Clock_Enable;
	}
	else
#endif
	{
		xUartClock.USART_Clock = USART_Clock_Disable;
	}
	xUartClock.USART_CPOL = USART_CPOL_Low;
	xUartClock.USART_CPHA = USART_CPHA_1Edge;
	xUartClock.USART_LastBit = USART_LastBit_Enable;
	USART_ClockInit(pUart, &xUartClock);

	xUart.baud = 9677;
	xUart.pari = UART_PARI_EVEN;
	xUart.data = UART_DATA_8D;
	xUart.stop = UART_STOP_1_5D;
	arch_UartOpen(pDef->id, &xUart);

#if SMARTCARD_ENABLE
	if (pDef->fun == UART_FUN_SC)
	{
		USART_SmartCardNACKCmd(pUart, ENABLE);
		USART_SmartCardCmd(pUart, ENABLE);
	}
#endif
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res arch_UartOpen(int nId, uart_para_t *pPara)
{
	USART_InitTypeDef xUartPara;
	USART_TypeDef *pUart = stm32_tblUartId[nId];

	switch (pPara->stop)
	{
	case UART_STOP_0_5D:
		xUartPara.USART_StopBits = USART_StopBits_0_5;
		break;
	case UART_STOP_1_5D:
		xUartPara.USART_StopBits = USART_StopBits_1_5;
		break;
	case UART_STOP_2D:
		xUartPara.USART_StopBits = USART_StopBits_2;
		break;
	default:
		xUartPara.USART_StopBits = USART_StopBits_1;
		break;
	}
	
	switch (pPara->pari)
	{
	case UART_PARI_EVEN:
		xUartPara.USART_Parity = USART_Parity_Even;
		break;
	case UART_PARI_ODD:
		xUartPara.USART_Parity = USART_Parity_Odd;
		break;
	default:
		xUartPara.USART_Parity = USART_Parity_No;
		break;
	}
	
	switch (pPara->data)
	{
#if UART_7D_ENABLE
	case UART_DATA_7D:
		xUartPara.USART_WordLength = USART_WordLength_8b;
		break;
#endif
	default:
		if (pPara->pari == UART_PARI_NO)
			xUartPara.USART_WordLength = USART_WordLength_8b;
		else
			xUartPara.USART_WordLength = USART_WordLength_9b;
		break;
	}
	
	xUartPara.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	xUartPara.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	xUartPara.USART_BaudRate = pPara->baud;
	USART_Init(pUart, &xUartPara);
	
	USART_ITConfig(pUart, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(pUart, ENABLE);

	return SYS_R_OK;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
#if UART_IRQ_TX_EN
void arch_UartTxIEnable(int nId)
{

	stm32_tblUartId[nId]->CR1 |= USART_FLAG_TXE | USART_FLAG_TC;
	arch_UartISR(nId);
}
#endif

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void arch_UartSendChar(int nId, const int nData)
{
	USART_TypeDef *pUart = stm32_tblUartId[nId];

	while ((pUart->SR & USART_FLAG_TXE) == 0);
	pUart->DR = nData;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
int arch_UartGetChar(int nId)
{
	USART_TypeDef *pUart = stm32_tblUartId[nId];

	while ((pUart->SR & USART_FLAG_RXNE) == 0);
	
	return ((int)(pUart->DR & 0x1FF));
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void arch_UartISR(int nId)
{
	USART_TypeDef *pUart = stm32_tblUartId[nId];
	uart_t *p;
	int c, opened;

	if (uartid_mcu2dev[nId] == 0xFF)
		return;
	p = &dev_Uart[uartid_mcu2dev[nId]];
	opened = dev_IsOpened(&p->parent);

	while (pUart->SR & (USART_FLAG_RXNE | USART_FLAG_ORE))
	{
#if UART_7D_ENABLE
		if (p->para.data == UART_DATA_7D)
			c = pUart->DR & 0x7F;
		else
#endif
			c = pUart->DR;
		
		if (opened)
			dque_Push(p->parent.id | UART_DQUE_RX_CHL, c);
	}
	
#if UART_IRQ_TX_EN
	if (pUart->CR1 & USART_FLAG_TXE)
	{
		while (pUart->SR & USART_FLAG_TXE)
		{
			int i;
			i = dque_PopChar(p->parent.id | UART_DQUE_TX_CHL);
			if (i < 0)
			{
				pUart->CR1 &= ~(USART_FLAG_TXE | USART_FLAG_TC);
				break;
			}
			pUart->DR = i;
		}
	}
#endif
}


#endif

