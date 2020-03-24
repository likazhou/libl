
#include <drivers/swuart.h>


//Private Defines
#define SWUART_ISO7816_BAUD		2400

#define SWUART_IDLE				0
#define SWUART_START_BIT		1
#define SWUART_DATA_1			2
#define SWUART_DATA_2			3
#define SWUART_DATA_3			4
#define SWUART_DATA_4			5
#define SWUART_DATA_5			6
#define SWUART_DATA_6			7
#define SWUART_DATA_7			8
#define SWUART_DATA_8			9
#define SWUART_DATA_9			10
#define SWUART_STOP_1			11
#define SWUART_STOP_2			12
#define SWUART_WAIT_1			13
#define SWUART_WAIT_2			14
#define SWUART_WAIT_3			15
#define SWUART_WAIT_4			16
#define SWUART_WAIT_5			17
#define SWUART_WAIT_6			18
#define SWUART_WAIT_7			19
#define SWUART_WAIT_8			20
#define SWUART_WAIT_9			21
#define SWUART_WAIT_10			22
#define SWUART_WAIT_11			23
#define SWUART_WAIT_12			24
#define SWUART_WAIT_13			25
#define SWUART_WAIT_14			26
#define SWUART_WAIT_15			27
#define SWUART_WAIT_16			28


 


//Private Typedef
typedef struct {
	uart_t 	   *parent;
	u8		rxint;
	u8		rxste;
	u8		rxdata;
	u8		txste;
	u8		txdata;
	u8		txpari;
	u8		txv;
	uint16_t	tick;
}swuart_t;



//Private Variables
static swuart_t swuart_aDev[SWUART_QTY];


//Internal Functions
#if SWUART_RX_MODE == SWUART_RX_M_CAP
extern void swuart_Rx(void *args);
#else
static void swuart_RxTx(void *args);

static void swuart_RxStart(void *args)
{
	swuart_t *p = (swuart_t *)args;
	t_uart_def *pDef = p->parent->def;

	irq_ExtDisable(p->rxint);
	if ((p->rxste < SWUART_DATA_1) || (p->rxste > SWUART_DATA_9))
	{
		p->rxste = SWUART_DATA_1;
		irq_TimerStart(pDef->id, p->tick + (p->tick >> SWUART_DELAY));
	}
}

static void swuart_IrdaTimer(void *args)
{
	swuart_t *p = (swuart_t *)args;
	t_uart_def *pDef = p->parent->def;

	p->txv ^= 1;
	arch_GpioSet(pDef->txport, pDef->txpin, p->txv);
}

static void swuart_TxOut(t_uart_def *pDef, int nValue)
{
	int nPort, nPin;

#if SWUART_SC_ENABLE
	if (pDef->fun == UART_FUN_SC)
	{
		nPort = pDef->rxport;
		nPin = pDef->rxpin;
	}
	else
#endif
	{
		nPort = pDef->txport;
		nPin = pDef->txpin;
	}
#if IRDA_ENABLE
	if (pDef->fun == UART_FUN_IRDA)
	{
#if IRDA_MODE == IRDA_MODE_TIM
		if (nValue)
		{
			irq_TimerStop(nPort);
			arch_GpioSet(nPort, nPin, pDef->fpin);
		}
		else
		{
			irq_TimerStart(nPort, TIMER_CLOCK / 76000);
		}
#else
		if (nValue)
			arch_PwmStop(nPort, nPin);
		else
			arch_PwmStart(nPort, nPin);
#endif
	}
	else
#endif
	{
		arch_GpioSet(nPort, nPin, nValue);
	}
}

static void swuart_RxTx(void *args)
{
	swuart_t *p = (swuart_t *)args;
	uart_t *pUart = p->parent;
	t_uart_def *pDef = pUart->def;
	int nData;
	int nPort, nPin;

	if (p->rxste != SWUART_IDLE)
	{
		nPort = pDef->rxport;
		nPin = pDef->rxpin;
		
		switch (p->rxste)
		{
		case SWUART_DATA_1:
			irq_TimerStart(pDef->id, p->tick);
			if (arch_GpioRead(nPort, nPin))
				p->rxdata = 1;
			else
				p->rxdata = 0;
			p->rxste += 1;
			break;
			
		case SWUART_DATA_7:
			if (arch_GpioRead(nPort, nPin))
				p->rxdata |= BITMASK(p->rxste - SWUART_DATA_1);
			
#if UART_7D_ENABLE
			if (pUart->para.data == UART_DATA_7D)
			{
				if (pUart->para.pari == UART_PARI_NO)
				{
					p->rxste = SWUART_STOP_1;
					break;
				}
			}
#endif
			p->rxste += 1;
			break;
			
		case SWUART_DATA_8:
#if UART_7D_ENABLE
			if (pUart->para.data == UART_DATA_7D)
			{
				p->rxste = SWUART_STOP_1;
				break;
			}
#endif
			if (arch_GpioRead(nPort, nPin))
				p->rxdata |= BITMASK(p->rxste - SWUART_DATA_1);
			if (pUart->para.pari == UART_PARI_NO)
			{
				p->rxste = SWUART_STOP_1;
				break;
			}
			p->rxste += 1;
			break;
			
		case SWUART_DATA_9:
			p->rxste = SWUART_STOP_1;
			break;
			
		case SWUART_STOP_1:
			if (p->txste == SWUART_IDLE)
				irq_ExtEnable(p->rxint);
			
			if (pUart->para.stop == UART_STOP_1D)
				p->rxste = SWUART_WAIT_1;
			else
				p->rxste = SWUART_STOP_2;
			
			dque_Push(pUart->parent.id | UART_DQUE_RX_CHL, p->rxdata);
			break;

		case SWUART_STOP_2:
			p->rxste = SWUART_WAIT_1;
			break;
			
		case SWUART_WAIT_1:
		case SWUART_WAIT_2:
		case SWUART_WAIT_3:
			p->rxste += 1;
			break;

		case SWUART_WAIT_4:
			if (p->txste == SWUART_IDLE)
				irq_TimerStop(pDef->id);
			
			p->rxste = SWUART_IDLE;
			break;

		default:
			if (arch_GpioRead(nPort, nPin))
				p->rxdata |= BITMASK(p->rxste - SWUART_DATA_1);
			
			p->rxste += 1;
			break;
		}
		return;
	}
	
	if (p->txste != SWUART_IDLE)
	{
		switch (p->txste)
		{
		case SWUART_START_BIT:
			nData = dque_PopChar(pUart->parent.id | UART_DQUE_TX_CHL);
			if (nData < 0)
			{
				p->txste = SWUART_IDLE;
				if (p->rxste == SWUART_IDLE)
				{
					irq_ExtEnable(p->rxint);
					irq_TimerStop(pDef->id);
				}
				break;
			}
			p->txdata = nData;
			p->txpari = 0;
			swuart_TxOut(pDef, 0);
			p->txste = SWUART_DATA_1;
			break;
			
		case SWUART_DATA_7:
			if (p->txdata & BITMASK(6))
			{
				swuart_TxOut(pDef, 1);
				p->txpari ^= 1;
			}
			else
			{
				swuart_TxOut(pDef, 0);
			}

#if UART_7D_ENABLE
			if (pUart->para.data == UART_DATA_7D)
			{
				if (pUart->para.pari == UART_PARI_NO)
				{
					p->txste = SWUART_STOP_1;
					break;
				}
			}
#endif
			p->txste += 1;
			break;
			
		case SWUART_DATA_8:
#if UART_7D_ENABLE
			if (pUart->para.data == UART_DATA_7D)
			{
				if (pUart->para.pari == UART_PARI_EVEN)
				{
					swuart_TxOut(pDef, p->txpari);
				}
				else
				{
					if (p->txpari)
						swuart_TxOut(pDef, 0);
					else
						swuart_TxOut(pDef, 1);
				}
				p->txste = SWUART_STOP_1;
				break;
			}
#endif
			if (p->txdata & BITMASK(7))
			{
				swuart_TxOut(pDef, 1);
				p->txpari ^= 1;
			}
			else
			{
				swuart_TxOut(pDef, 0);
			}
			
			if (pUart->para.pari == UART_PARI_NO)
			{
				p->txste = SWUART_STOP_1;
				break;
			}
			p->txste += 1;
			break;
			
		case SWUART_DATA_9:
			if (pUart->para.pari == UART_PARI_EVEN)
			{
				swuart_TxOut(pDef, p->txpari);
			}
			else
			{
				if (p->txpari)
					swuart_TxOut(pDef, 0);
				else
					swuart_TxOut(pDef, 1);
			}
			p->txste = SWUART_STOP_1;
			break;
			
		case SWUART_STOP_1:
			swuart_TxOut(pDef, 1);
			if (pUart->para.stop == UART_STOP_1D)
				p->txste = SWUART_WAIT_1;
			else
				p->txste = SWUART_STOP_2;
			break;
			
		case SWUART_STOP_2:
		case SWUART_WAIT_1:
		case SWUART_WAIT_2:
		case SWUART_WAIT_3:
			p->txste += 1;
			break;
			
		case SWUART_WAIT_4:
			p->txste = SWUART_START_BIT;
			break;
			
		default:
			if (p->txdata & BITMASK(p->txste - SWUART_DATA_1))
			{
				swuart_TxOut(pDef, 1);
				p->txpari ^= 1;
			}
			else
			{
				swuart_TxOut(pDef, 0);
			}
			p->txste += 1;
			break;
		}
		return;
	}
	
	irq_TimerStop(pDef->id);
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void swuart_TxStart(int nId)
{
	swuart_t *p = &swuart_aDev[nId];

	if (p->txste == SWUART_IDLE)
	{
		p->txste = SWUART_START_BIT;
		if (p->rxste == SWUART_IDLE)
		{
			irq_ExtDisable(p->rxint);
			irq_TimerStart(nId, p->tick);
			swuart_RxTx(p);
		}
	}
}
#endif




//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void swuart_Init(uart_t *p)
{
	int nTxMode, nRxMode;
	uart_para_t xUart;
	t_uart_def *pDef = p->def;

	swuart_aDev[pDef->id].parent = p;

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

	//Rx Pin Init
#if SWUART_SC_ENABLE
	if (pDef->fun == UART_FUN_SC)
	{
		arch_GpioConf(pDef->rxport, pDef->rxpin, GPIO_M_OUT_OD, GPIO_INIT_HIGH);
	}
	else
#endif
	{
		arch_GpioConf(pDef->rxport, pDef->rxpin, nRxMode, GPIO_INIT_NULL);
#if SWUART_RX_MODE == SWUART_RX_M_EINT
		arch_ExtIrqRxConf(pDef->rxport, pDef->rxpin);
#else
		arch_TimerCapConf(pDef->rxport, pDef->rxpin);
#endif
	}

	//Tx Pin Init
	switch (pDef->fun)
	{
#if SWUART_SC_ENABLE
	case UART_FUN_SC:
		arch_PwmConf(pDef->txport, pDef->txpin, nTxMode, SWUART_ISO7816_BAUD * 372);
		arch_PwmStart(pDef->txport, pDef->txpin);
		//Reset Pin Init
		arch_GpioConf(pDef->fport, pDef->fpin, nTxMode, GPIO_INIT_HIGH);
		xUart.baud = SWUART_ISO7816_BAUD;
		break;
#endif

#if IRDA_ENABLE
	case UART_FUN_IRDA:
#if IRDA_MODE == IRDA_MODE_TIM
		if (pDef->fpin)
			arch_GpioConf(pDef->txport, pDef->txpin, nTxMode, GPIO_INIT_HIGH);
		else
			arch_GpioConf(pDef->txport, pDef->txpin, nTxMode, GPIO_INIT_LOW);
		irq_TimerRegister(pDef->txport, swuart_IrdaTimer, &swuart_aDev[pDef->id]);
#else
		arch_PwmConf(pDef->txport, pDef->txpin, nTxMode, 38000);
#endif
		xUart.baud = 1200;
		break;
#endif

	default:
		arch_GpioConf(pDef->txport, pDef->txpin, nTxMode, GPIO_INIT_HIGH);
		xUart.baud = 4800;
		break;
	}
	xUart.pari = UART_PARI_EVEN;
	xUart.data = UART_DATA_8D;
	xUart.stop = UART_STOP_2D;
	swuart_Open(pDef->id, &xUart);
}





//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res swuart_Open(int nId, uart_para_t *pPara)
{
	swuart_t *pSW = &swuart_aDev[nId];
#if SWUART_RX_MODE == SWUART_RX_M_EINT
	uart_t *p = pSW->parent;
	t_uart_def *pDef = p->def;
	int nIntId;
#endif

	pSW->tick = TIMER_CLOCK / pPara->baud;
#if SWUART_RX_MODE == SWUART_RX_M_EINT

	irq_TimerRegister(nId, swuart_RxTx, pSW);
	nIntId = irq_ExtRegister(pDef->rxport, pDef->rxpin, IRQ_TRIGGER_FALLING, swuart_RxStart, pSW, IRQ_MODE_NORMAL);
	if (nIntId == -1)
		return SYS_R_ERR;
	
	pSW->rxint = nIntId;
	irq_ExtEnable(nIntId);
#else

	irq_TimerRegister(nId, swuart_Rx, pSW);
	arch_TimerCapStart(nId, pSW->tick);
#endif

	return SYS_R_OK;
}

