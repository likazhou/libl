#if UART_ENABLE





//Private Variables
static USART_TypeDef * const stm32_tbl_uartid[] =
{
	USART1,
	USART2,
	USART3,
	UART4,
	UART5,
	USART6,
#if defined(STM32F427xx) || defined(STM32F429xx)
	UART7,
	UART8,
#endif
};

static u8 uartid_mcu2dev[] =
{
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
#if defined(STM32F427xx) || defined(STM32F429xx)
	0xFF,
	0xFF,
#endif
};


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void arch_UartInit(uart_t *p)
{
	t_uart_def *pDef = p->def;
	int nirq;
	LL_GPIO_InitTypeDef xGpio;
	LL_USART_ClockInitTypeDef xUartClock;
	USART_TypeDef *pUart;

	uartid_mcu2dev[pDef->id] = p - &dev_Uart[0];

	pUart = stm32_tbl_uartid[pDef->id];
	switch (pDef->id)
	{
	case 0:
		/* Enalbe the USART1  Clock*/
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
		/* Enable the USART1 Interrupt */
		nirq = USART1_IRQn;
		xGpio.Alternate = LL_GPIO_AF_7;
		break;
		
	case 1:
		/* Enalbe the USART2  Clock*/
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
		/* Enable the USART2 Interrupt */
		nirq = USART2_IRQn;
		xGpio.Alternate = LL_GPIO_AF_7;
		break;
		
	case 2:
		/* Enalbe the USART3  Clock*/
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);
		/* Enable the USART3 Interrupt */
		nirq = USART3_IRQn;
		xGpio.Alternate = LL_GPIO_AF_7;
		break;
		
	case 3:
		/* Enalbe the UART4  Clock*/
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART4);
		/* Enable the UART4 Interrupt */
		nirq = UART4_IRQn;
		xGpio.Alternate = LL_GPIO_AF_8;
		break;
		
	case 4:
		/* Enalbe the UART5  Clock*/
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART5);
		/* Enable the UART5 Interrupt */
		nirq = UART5_IRQn;
		xGpio.Alternate = LL_GPIO_AF_8;
		break;
		
	case 5:
		/* Enalbe the USART6  Clock*/
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART6);
		/* Enable the USART6 Interrupt */
		nirq = USART6_IRQn;
		xGpio.Alternate = LL_GPIO_AF_8;
		break;

#if defined(STM32F427xx) || defined(STM32F429xx)
	case 6:
		/* Enalbe the UART7  Clock*/
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART7);
		/* Enable the UART7 Interrupt */
		nirq = UART7_IRQn;
		xGpio.Alternate = LL_GPIO_AF_8;
		break;

	case 7:
		/* Enalbe the UART8  Clock*/
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART8);
		/* Enable the UART8 Interrupt */
		nirq = UART8_IRQn;
		xGpio.Alternate = LL_GPIO_AF_8;
		break;
#endif

	default:
		return;
	}
	arch_IrqPri(nirq, 0);

	xGpio.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	xGpio.Pull = LL_GPIO_PULL_UP;
	
	//Tx
	xGpio.Pin = BITMASK(pDef->txpin);
	xGpio.Mode = LL_GPIO_MODE_ALTERNATE;
	xGpio.OutputType = pDef->pinmode == DEV_PIN_OD ? LL_GPIO_OUTPUT_OPENDRAIN : LL_GPIO_OUTPUT_PUSHPULL;
	arch_GpioClockEnable(pDef->txport);
	LL_GPIO_Init(arch_GpioPortBase(pDef->txport), &xGpio);
	
	//Rx
	xGpio.Pin = BITMASK(pDef->rxpin);
#if SMARTCARD_ENABLE
	if (pDef->fun == UART_FUN_SC)
	{
		//SmartCard时钟372倍波特率,1为PCLK2分频,2为4分频......
#if MCU_FREQUENCY == MCU_SPEED_LOW
		LL_USART_SetSmartcardPrescaler(pUart, 1);//PCLK=8M
#elif MCU_FREQUENCY == MCU_SPEED_HALF
		LL_USART_SetSmartcardPrescaler(pUart, 4);//PCLK=30M
#else
		LL_USART_SetSmartcardPrescaler(pUart, 9);//PCLK=60M
#endif
		LL_USART_SetSmartcardGuardTime(pUart, 16);

		//Fun
		xGpio.Pin = BITMASK(pDef->fpin);
		arch_GpioClockEnable(pDef->fport);
		LL_GPIO_Init(arch_GpioPortBase(pDef->fport), &xGpio);

		xGpio.Mode = LL_GPIO_MODE_OUTPUT;
	}
#endif
	arch_GpioClockEnable(pDef->rxport);
	LL_GPIO_Init(arch_GpioPortBase(pDef->rxport), &xGpio);
	
	//Clock
#if SMARTCARD_ENABLE
	if (pDef->fun == UART_FUN_SC)
	{
		xUartClock.ClockOutput = LL_USART_CLOCK_ENABLE;
	}
	else
#endif
	{
		xUartClock.ClockOutput = LL_USART_CLOCK_DISABLE;
	}
	xUartClock.ClockPolarity = LL_USART_POLARITY_LOW;
	xUartClock.ClockPhase = LL_USART_PHASE_1EDGE;
	xUartClock.LastBitClockPulse = LL_USART_LASTCLKPULSE_OUTPUT;
	LL_USART_ClockInit(pUart, &xUartClock);

#if SMARTCARD_ENABLE
	if (pDef->fun == UART_FUN_SC)
	{
		LL_USART_EnableSmartcardNACK(pUart);
		LL_USART_EnableSmartcard(pUart);
	}
#endif
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res arch_UartOpen(int nId, uart_para_t *pPara)
{
	LL_USART_InitTypeDef xUartPara;
	USART_TypeDef *pUart = stm32_tbl_uartid[nId];

	switch (pPara->stop)
	{
	case UART_STOP_0_5D:
		xUartPara.StopBits = LL_USART_STOPBITS_0_5;
		break;
	case UART_STOP_1_5D:
		xUartPara.StopBits = LL_USART_STOPBITS_1_5;
		break;
	case UART_STOP_2D:
		xUartPara.StopBits = LL_USART_STOPBITS_2;
		break;
	default:
		xUartPara.StopBits = LL_USART_STOPBITS_1;
		break;
	}
	
	switch (pPara->pari)
	{
	case UART_PARI_EVEN:
		xUartPara.Parity = LL_USART_PARITY_EVEN;
		break;
	case UART_PARI_ODD:
		xUartPara.Parity = LL_USART_PARITY_ODD;
		break;
	default:
		xUartPara.Parity = LL_USART_PARITY_NONE;
		break;
	}
	
	switch (pPara->data)
	{
#if UART_7D_ENABLE
	case UART_DATA_7D:
		xUartPara.DataWidth = LL_USART_DATAWIDTH_8B;
		break;
#endif
	default:
		xUartPara.DataWidth = pPara->pari == UART_PARI_NO ? LL_USART_DATAWIDTH_8B : LL_USART_DATAWIDTH_9B;
		break;
	}
	
	xUartPara.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	xUartPara.TransferDirection = LL_USART_DIRECTION_TX_RX;
	xUartPara.OverSampling = LL_USART_OVERSAMPLING_16;
	xUartPara.BaudRate = pPara->baud;
	LL_USART_DeInit(pUart);
	LL_USART_Init(pUart, &xUartPara);
	
	LL_USART_EnableIT_RXNE(pUart);
	
	LL_USART_Enable(pUart);

	return SYS_R_OK;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
#if UART_IRQ_TX_EN
void arch_UartTxIEnable(int nId)
{

	stm32_tbl_uartid[nId]->CR1 |= (LL_USART_CR1_TXEIE | LL_USART_CR1_TCIE);
	arch_UartISR(nId);
}
#endif

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void arch_UartSendChar(int nId, const int nData)
{
	USART_TypeDef *pUart = stm32_tbl_uartid[nId];

	while ((pUart->SR & LL_USART_SR_TXE) == 0);
	pUart->DR = nData;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
int arch_UartGetChar(int nId)
{
	USART_TypeDef *pUart = stm32_tbl_uartid[nId];

	while ((pUart->SR & LL_USART_SR_RXNE) == 0);
	
	return ((int)(pUart->DR & 0x1FF));
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void arch_UartISR(int nId)
{
	USART_TypeDef *pUart = stm32_tbl_uartid[nId];
	uart_t *p;
	int c, opened;

	if (uartid_mcu2dev[nId] == 0xFF)
		return;
	p = &dev_Uart[uartid_mcu2dev[nId]];
	opened = dev_IsOpened(&p->parent);

	while (pUart->SR & (LL_USART_SR_RXNE | LL_USART_SR_ORE))
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
	if (pUart->CR1 & LL_USART_CR1_TXEIE)
	{
		while (pUart->SR & LL_USART_SR_TXE)
		{
			int i;
			i = dque_PopChar(p->parent.id | UART_DQUE_TX_CHL);
			if (i < 0)
			{
				pUart->CR1 &= ~(LL_USART_CR1_TXEIE | LL_USART_CR1_TCIE);
				break;
			}
			pUart->DR = i;
		}
	}
#endif
}


void USART1_IRQHandler(void)
{

	os_irq_enter();

	arch_UartISR(0);

	os_irq_leave();
}

void USART2_IRQHandler(void)
{

	os_irq_enter();

	arch_UartISR(1);

	os_irq_leave();
}

void USART3_IRQHandler(void)
{

	os_irq_enter();

	arch_UartISR(2);

	os_irq_leave();
}

void UART4_IRQHandler(void)
{

	os_irq_enter();

	arch_UartISR(3);

	os_irq_leave();
}

void UART5_IRQHandler(void)
{

	os_irq_enter();

	arch_UartISR(4);

	os_irq_leave();
}

void USART6_IRQHandler(void)
{

	os_irq_enter();

	arch_UartISR(5);

	os_irq_leave();
}

#if defined(STM32F427xx) || defined(STM32F429xx)
void UART7_IRQHandler(void)
{

	os_irq_enter();

	arch_UartISR(6);

	os_irq_leave();
}

void UART8_IRQHandler(void)
{

	os_irq_enter();

	arch_UartISR(7);

	os_irq_leave();
}
#endif

#endif


