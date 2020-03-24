#if SC16IS7X_ENABLE
#include <drivers/sc16is7x.h>



//Private Defines
#define SC16IS7X_LCR 				0x03
#define SC16IS7X_FCR 				0x02
#define SC16IS7X_IIR				0x02
#define SC16IS7X_IER 				0x01
#define SC16IS7X_MCR 				0x04
#define SC16IS7X_MSR 				0x06
#define SC16IS7X_TCR 				0x06
#define SC16IS7X_TLR 				0x07
#define SC16IS7X_RHR 				0x00  //读模式有效
#define SC16IS7X_THR 				0x00  //写模式有效
#define SC16IS7X_LSR 				0x05
#define SC16IS7X_IODir 				0x0A
#define SC16IS7X_IOState 			0x0B
#define SC16IS7X_IOIntEna 			0x0C
#define SC16IS7X_IOControl			0x0E
#define SC16IS7X_RXLVL				0x09
#define SC16IS7X_TXLVL 				0x08

#define SC16IS7X_DLL 				0x00  //仅当LCR=0x80时可访问寄存器DLL和DHL
#define SC16IS7X_DLH 				0x01

#define SC16IS7X_EFR 				0x02  //仅当LCR=0xBF时可访问增强型寄存器EFR


//Private Macros
#define sc16is7x_Rst(x)					sys_GpioSet(&tbl_bspSc16is7x.tbl[0], x)
#define sc16is7x_Nss(x)					sys_GpioSet(&tbl_bspSc16is7x.tbl[1], x)



//Private Variables
static spi_t *sc16is7x_spi;


//Internal Functions
static sys_res sc16is7x_RegWrite(int nReg, int nChl, int nCmd)
{
	sys_res res;

	nReg = (nCmd << 8) | (nReg << 3) | nChl;
	sc16is7x_Nss(0);
	res = spi_Send(sc16is7x_spi, &nReg, 2);
	sc16is7x_Nss(1);
	return res;
}

static int sc16is7x_RegRead(int nReg, int nChl)
{
	int nData = 0;

	nReg = BITMASK(7) | (nReg << 3) | nChl;
	sc16is7x_Nss(0);
	spi_Transce(sc16is7x_spi, nReg, &nData, 1);
	sc16is7x_Nss(1);
	return nData;
}



//-------------------------------------------------------------------------
//External Functions
//-------------------------------------------------------------------------
sys_res sc16is7x_Init()
{
	int i;

	for (i = 0; i < tbl_bspSc16is7x.qty; i++)
	{
		sys_GpioConf(&tbl_bspSc16is7x.tbl[i]);
	}
	
	while ((sc16is7x_spi = spi_Open(SC16IS7X_COMID, 1000)) == NULL);
	
	spi_Config(sc16is7x_spi, SPI_SCKIDLE_LOW, SPI_LATCH_1EDGE, SC16IS7X_SPI_SPEED);
	irq_ExtRegister(0, &tbl_bspSc16is7x.tbl[2], IRQ_TRIGGER_FALLING, sc16is7x_ItHandler);
	sc16is7x_Reset();
	
	return SYS_R_OK;
}



//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res sc16is7x_Reset()
{

	sc16is7x_Rst(0);
	sys_Delay(200000);
	sc16is7x_Rst(1);
	sys_Delay(500000);

	sc16is7x_RegWrite(SC16IS7X_IODir, 0, 0x1F); 		//配置GPIO5-GPIO7为输入, GPIO0 ~ GPIO4为输出
	sc16is7x_RegWrite(SC16IS7X_IOIntEna, 0, 0x00);		//禁止GPIO5-GPIO7中断
	sc16is7x_RegWrite(SC16IS7X_IOControl, 0, 0x01);	//使GPIO具有锁存功能
	return SYS_R_OK;
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res sc16is7x_UartConfig(int nChl, uart_para_t *pPara)
{
	int nTemp;

	nTemp = SC16IS7X_CRYSTAL / pPara->baud / 16;
	sc16is7x_RegWrite(SC16IS7X_LCR, nChl, BITMASK(7)); 	//除数锁存使能
	sc16is7x_RegWrite(SC16IS7X_DLL, nChl, nTemp);
	sc16is7x_RegWrite(SC16IS7X_DLH, nChl, nTemp >> 8);

#if UART_7D_ENABLE
	if (pPara->data == UART_DATA_7D)
		nTemp = BITMASK(1);
	else
#endif
		nTemp = BITMASK(1) | BITMASK(0);

	if (pPara->stop == UART_STOP_2D)
		nTemp |= BITMASK(6);
	
	switch (pPara->pari)
	{
	case UART_PARI_EVEN:
		nTemp |= BITMASK(4) | BITMASK(3);
		break;
	case UART_PARI_ODD:
		nTemp |= BITMASK(3);
		break;
	default:
		break;
	}

	sc16is7x_RegWrite(SC16IS7X_LCR, nChl, nTemp);		//关闭除数锁存器,访问通用寄存器
	sc16is7x_RegWrite(SC16IS7X_FCR, nChl, BITMASK(0));	//使能FIFO
	sc16is7x_RegWrite(SC16IS7X_IER, nChl, BITMASK(0)); 	//设置中断，允许睡眠模式，打开RX超时、RHR满、关闭RTHR中断	
	return SYS_R_OK;
}

sys_res sc16is7x_UartSend(int nChl, void *pData, size_t nLen)
{
	int i, nReg;

	nReg = (SC16IS7X_THR << 3) | nChl;
	for (; nLen; nLen -= i)
	{
		if ((i = MIN(sc16is7x_RegRead(SC16IS7X_TXLVL, nChl), nLen)) != 0)
		{
			sc16is7x_Nss(0);
			spi_Send(sc16is7x_spi, &nReg, 1);
			spi_Send(sc16is7x_spi, pData, i);
			sc16is7x_Nss(1);
		}
	}
	return SYS_R_OK;
}

void sc16is7x_ItHandler()
{
	uart_t *p;
	t_uart_def *pDef;
	int i, nLen, nLsr, nRecd, nReg;
	u8 aTemp[64];

	for (pDef = tbl_bspUartDef, i = 0; pDef < ARR_ENDADR(tbl_bspUartDef); pDef++, i++)
	{
		if (pDef->type != UART_T_SC16IS7X)
			continue;
		p = uart_Dev(i);
		//中断识别,也可读IIR实现
		nLsr = sc16is7x_RegRead(SC16IS7X_LSR, pDef->id);
		if (nLsr & BITMASK(0)) {
			//Rx Fifo Not Empty
			nRecd = sc16is7x_RegRead(SC16IS7X_RXLVL, pDef->id);
			for (nReg = BITMASK(7) | (SC16IS7X_RHR << 3) | pDef->id; nRecd; nRecd -= nLen)
			{
				nLen = MIN(nRecd, sizeof(aTemp));
				
				sc16is7x_Nss(0);
				
				if (spi_Transce(sc16is7x_spi, nReg, aTemp, nLen) == SYS_R_OK)
					dque_Push(p->parent.id | UART_DQUE_RX_CHL, aTemp, nLen);
				
				sc16is7x_Nss(1);
			}
		}
		if (nLsr & 0xF8)
			sc16is7x_RegWrite(SC16IS7X_IER, pDef->id, 0x01);
	}
}

int sc16is7x_GpioRead(int nPin)
{

	return GETBIT(sc16is7x_RegRead(SC16IS7X_IOState, 0), nPin);
}

void sc16is7x_GpioSet(int nPin, int nHL)
{
	int i;

	i = sc16is7x_RegRead(SC16IS7X_IOState, 0);
	if (nHL)
		SETBIT(i, nPin);
	else
		CLRBIT(i, nPin);
	sc16is7x_RegWrite(SC16IS7X_IOState, 0, i);
}


#endif

