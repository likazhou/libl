#if STM32_ETH_ENABLE


/* The following macro definitions may be used to select the speed
   of the physical link:

  _10MBIT_   - connect at 10 MBit only
  _100MBIT_  - connect at 100 MBit only

  By default an autonegotiation of the link speed is used. This may take 
  longer to connect, but it works for 10MBit and 100MBit physical links.     */

/* Net_Config.c */
extern u8 own_hw_adr[];

/* Local variables */
static u8 TxBufIndex;
static u8 RxBufIndex;

/* ENET local DMA Descriptors. */
static RX_Desc Rx_Desc[NUM_RX_BUF];
static TX_Desc Tx_Desc[NUM_TX_BUF];

/* ENET local DMA buffers. */
static u32 rx_buf[NUM_RX_BUF][ETH_BUF_SIZE>>2];
static u32 tx_buf[NUM_TX_BUF][ETH_BUF_SIZE>>2];

/*----------------------------------------------------------------------------
 *      ENET Ethernet Driver Functions
 *----------------------------------------------------------------------------
 *  Required functions for Ethernet driver module:
 *  a. Polling mode: - void init_ethernet ()
 *                   - void send_frame (OS_FRAME *frame)
 *                   - void poll_ethernet (void)
 *  b. Interrupt mode: - void init_ethernet ()
 *                     - void send_frame (OS_FRAME *frame)
 *                     - void int_enable_eth ()
 *                     - void int_disable_eth ()
 *                     - interrupt function 
 *---------------------------------------------------------------------------*/

/* Local Function Prototypes */
static void rx_descr_init(void);
static void tx_descr_init(void);
static void write_PHY(u32 PhyReg, u16 Value);
static u16 read_PHY(u32 PhyReg);

/*--------------------------- init_ethernet ---------------------------------*/


/* ------------ RCC registers bit address in the alias region ----------------*/
#define AFIO_OFFSET                 (AFIO_BASE - PERIPH_BASE)

/* --- EVENTCR Register -----*/

/* Alias word address of EVOE bit */
#define EVCR_OFFSET                 (AFIO_OFFSET + 0x00)
#define EVOE_BitNumber              ((u8)0x07)
#define EVCR_EVOE_BB                (PERIPH_BB_BASE + (EVCR_OFFSET * 32) + (EVOE_BitNumber * 4))


/* ---  MAPR Register ---*/ 
/* Alias word address of MII_RMII_SEL bit */ 
#define MAPR_OFFSET                 (AFIO_OFFSET + 0x04) 
#define MII_RMII_SEL_BitNumber      ((u8)0x17) 
#define MAPR_MII_RMII_SEL_BB        (PERIPH_BB_BASE + (MAPR_OFFSET * 32) + (MII_RMII_SEL_BitNumber * 4))


int arch_EmacInit()
{
	/* Initialize the ETH ethernet controller. */
	u32 regv,tout,id1,id2;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable clock for Port A,B,C,D and Alternate function */
	RCC->APB2ENR |= 0x0000003D;

	/* ETHERNET pins remapp: RX_DV and RxD[3:0] */
	GPIO_PinRemapConfig(GPIO_Remap_ETH, ENABLE);

	/* MII/RMII Media interface selection */
#if ETH_MAC_MODE_MII /* Mode MII */
	/* Configure MII_RMII selection bit */ 
	*(__IO u32 *)MAPR_MII_RMII_SEL_BB = GPIO_ETH_MediaInterface_MII; 
#if ETH_CLK4CPU_ENABLE
	/* Get HSE clock = 25MHz on PA8 pin(MCO) */
	RCC_MCOConfig(RCC_MCO_HSE);
#endif
#else  /* Mode RMII */
	/* Configure MII_RMII selection bit */ 
	*(__IO u32 *)MAPR_MII_RMII_SEL_BB = GPIO_ETH_MediaInterface_RMII; 
#if ETH_CLK4CPU_ENABLE
	/* set PLL3 clock output to 50MHz (25MHz /5 *10 =50MHz) */
	RCC_PLL3Config(RCC_PLL3Mul_10);
	/* Enable PLL3 */
	RCC_PLL3Cmd(ENABLE);
	/* Wait till PLL3 is ready */
	for (tout = 0; tout < 0x10000; tout++) {
		if (RCC_GetFlagStatus(RCC_FLAG_PLL3RDY) != RESET)
			break;
	}

	/* Get clock PLL3 clock on PA8 pin */
	RCC_MCOConfig(RCC_MCO_PLL3CLK);
#endif
#endif

	/* ETHERNET pins configuration */
	/* AF Output Push Pull:
	- ETH_MII_MDIO / ETH_RMII_MDIO: PA2
	- ETH_MII_MDC / ETH_RMII_MDC: PC1
	- ETH_MII_TXD2: PC2
	- ETH_MII_TX_EN / ETH_RMII_TX_EN: PB11
	- ETH_MII_TXD0 / ETH_RMII_TXD0: PB12
	- ETH_MII_TXD1 / ETH_RMII_TXD1: PB13
	- ETH_MII_PPS_OUT / ETH_RMII_PPS_OUT: PB5
	- ETH_MII_TXD3: PB8 */

	/* Configure PA2 and MCO (PA8) as alternate function push-pull */
#if ETH_MAC_MODE_MII
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
#else
#if ETH_CLK4CPU_ENABLE
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_8;
#else
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
#endif
#endif
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure PC1, PC2 and PC3 as alternate function push-pull */
#if ETH_MAC_MODE_MII
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
#else
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
#endif
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure PB5, PB8, PB11, PB12 and PB13 as alternate function push-pull */
#if ETH_MAC_MODE_MII
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
#else
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
#endif
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/**************************************************************/
	/*				 For Remapped Ethernet pins 				  */
	/*************************************************************/
	/* Input (Reset Value):
	- ETH_MII_CRS CRS: PA0
	- ETH_MII_RX_CLK / ETH_RMII_REF_CLK: PA1
	- ETH_MII_COL: PA3
	- ETH_MII_RX_DV / ETH_RMII_CRS_DV: PD8
	- ETH_MII_TX_CLK: PC3
	- ETH_MII_RXD0 / ETH_RMII_RXD0: PD9
	- ETH_MII_RXD1 / ETH_RMII_RXD1: PD10
	- ETH_MII_RXD2: PD11
	- ETH_MII_RXD3: PD12
	- ETH_MII_RX_ER: PB10 */

	/* Configure PA0, PA1 and PA3 as input */
#if ETH_MAC_MODE_MII
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3;
#else
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
#endif
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure PB10 as input */
#if ETH_MAC_MODE_MII
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure PC3 as input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif

	/* Configure PD8, PD9, PD10, PD11 and PD12 as input */
#if ETH_MAC_MODE_MII
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
#else
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
#endif
	GPIO_Init(GPIOD, &GPIO_InitStructure); /**/

	/* Enable clock for MAC. */
	RCC->AHBENR  |= 0x0001C000;

	/* Reset Ethernet MAC */
	RCC->AHBRSTR |= 0x00004000;
	RCC->AHBRSTR &=~0x00004000;

	ETH->DMABMR |= DBMR_SR;
	for (tout = 0; tout < 0x10000; tout++) {
		if ((ETH->DMABMR & DBMR_SR) == 0)
			break;
	}
	if (tout >= 0x10000)
		return (-1);

	/* MDC Clock range 60-72MHz. */
	ETH->MACMIIAR = 0x00000000;

	/* Put the PHY in reset mode */
	write_PHY(PHY_REG_BMCR, 0x8000);
	/* Wait for hardware reset to end. */
	for (tout = 0; tout < 0x8000; tout++) {
		regv = read_PHY(PHY_REG_BMCR);
		if (!(regv & 0x8800)) {
			/* Reset complete, device not Power Down. */
			break;
		}
	}

	/* Check if this is a DM9161 PHY. */
	id1 = read_PHY(PHY_REG_IDR1);
	id2 = read_PHY(PHY_REG_IDR2);

	/* Configure the PHY device */
	/* Use autonegotiation about the link speed. */
	write_PHY (PHY_REG_BMCR, PHY_AUTO_NEG);
	/* Wait to complete Auto_Negotiation. */
	for (tout = 0; tout < 0x10000; tout++) {
		regv = read_PHY (PHY_REG_BMSR);
		if (regv & 0x0024) {
			/* Autonegotiation Complete and Link is on. */
			break;
		}
	}

	/* Initialize MAC control register */
	ETH->MACCR = MCR_ROD;

	/* Configure Full/Half Duplex mode. */
	if (regv & 0x0004) {
		/* Full duplex is enabled. */
		ETH->MACCR |= MCR_DM;
	}

#if 1
	/* Configure 100MBit/10MBit mode. */
	if ((regv & 0x0002) == 0) {
		/* 100MBit mode. */
		ETH->MACCR |= MCR_FES;
	}
#endif

	/* MAC address filtering, accept multicast packets. */
	ETH->MACFFR = MFFR_HPF | MFFR_PAM;
	ETH->MACFCR = MFCR_ZQPD;

	/* Initialize Tx and Rx DMA Descriptors */
	rx_descr_init();
	tx_descr_init();

	/* Flush FIFO, start DMA Tx and Rx */
	ETH->DMAOMR = DOMR_FTF | DOMR_ST | DOMR_SR;

	/* Enable receiver and transmiter */
	ETH->MACCR |= MCR_TE | MCR_RE;

	/* Reset all interrupts */
	ETH->DMASR = 0xFFFFFFFF;

#if ETH_INT_ENABLE
	/* Enable Rx and NIS interrupts. */
	ETH->DMAIER = INT_NISE | INT_RIE;
#endif
	return 0;
}

void arch_EmacAddr(u8 *pAdr)
{

	/* Set the Ethernet MAC Address registers */
	ETH->MACA0HR =	((u32)pAdr[5] <<	8) | (u32)pAdr[4];
	ETH->MACA0LR =	((u32)pAdr[3] << 24) | (u32)pAdr[2] << 16 |
					((u32)pAdr[1] <<  8) | (u32)pAdr[0];
}


void arch_EmacIntEnable()
{

	/* Ethernet Interrupt Enable function. */
	NVIC->ISER[1] = 1 << 29;
}

void arch_EmacIntDisable()
{

	/* Ethernet Interrupt Disable function. */
	NVIC->ICER[1] = 1 << 29;
}


void arch_EmacPacketTx(const void *pData, size_t nLen)
{
	/* Send frame to ETH ethernet controller */
	u32 *sp,*dp;
	u32 i,j;

	j = TxBufIndex;
	/* Wait until previous packet transmitted. */
	while (Tx_Desc[j].CtrlStat & DMA_TX_OWN);

	sp = (u32 *)pData;
	dp = (u32 *)(Tx_Desc[j].Addr & ~3);

	/* Copy frame data to ETH IO buffer. */
	for (i = (nLen + 3) >> 2; i; i--) {
		*dp++ = *sp++;
	}
	Tx_Desc[j].Size      = nLen;
	Tx_Desc[j].CtrlStat |= DMA_TX_OWN;
	if (++j == NUM_TX_BUF)
		j = 0;
	TxBufIndex = j;
	/* Start frame transmission. */
	ETH->DMASR   = DSR_TPSS;
	ETH->DMATPDR = 0;
}

#include <net/rtxip/Net_Config.h>

void arch_EmacIsr()
{
	/* Ethernet Controller Interrupt function. */
	OS_FRAME *frame;
	u32 i,RxLen;
	u32 *sp,*dp;

	i = RxBufIndex;
	do {
		/* Valid frame has been received. */
		if (Rx_Desc[i].Stat & DMA_RX_ERROR_MASK) {
			goto rel;
		}
		if ((Rx_Desc[i].Stat & DMA_RX_SEG_MASK) != DMA_RX_SEG_MASK) {
			goto rel;
		}
		RxLen = ((Rx_Desc[i].Stat >> 16) & 0x3FFF) - 4;
		if (RxLen > ETH_MTU) {
			/* Packet too big, ignore it and free buffer. */
			goto rel;
		}
		/* Flag 0x80000000 to skip sys_error() call when out of memory. */
		frame = alloc_mem (RxLen | 0x80000000);
		/* if 'alloc_mem()' has failed, ignore this packet. */
		if (frame != NULL) {
			sp = (u32 *)(Rx_Desc[i].Addr & ~3);
			dp = (u32 *)&frame->data[0];
			for (RxLen = (RxLen + 3) >> 2; RxLen; RxLen--) {
				*dp++ = *sp++;
			}
			put_in_queue (frame);
		}
		/* Release this frame from ETH IO buffer. */
		rel:Rx_Desc[i].Stat = DMA_RX_OWN;

		if (++i == NUM_RX_BUF)
			i = 0;
		RxBufIndex = i;
	}
	while (!(Rx_Desc[i].Stat & DMA_RX_OWN));
	RxBufIndex = i;

	if (ETH->DMASR & INT_RBUIE) {
		/* Rx DMA suspended, resume DMA reception. */
		ETH->DMASR   = INT_RBUIE;
		ETH->DMARPDR = 0;
	}
	/* Clear the interrupt pending bits. */
	ETH->DMASR = INT_NISE | INT_RIE;
}


/*--------------------------- rx_descr_init ---------------------------------*/

static void rx_descr_init(void)
{
  /* Initialize Receive DMA Descriptor array. */
  u32 i,next;

  RxBufIndex = 0;
  for (i = 0, next = 0; i < NUM_RX_BUF; i++) {
    if (++next == NUM_RX_BUF) next = 0;
    Rx_Desc[i].Stat = DMA_RX_OWN;
    Rx_Desc[i].Ctrl = DMA_RX_RCH | ETH_BUF_SIZE;
    Rx_Desc[i].Addr = (u32)&rx_buf[i];
    Rx_Desc[i].Next = (u32)&Rx_Desc[next];
  }
  ETH->DMARDLAR = (u32)&Rx_Desc[0];
}



/*--------------------------- tx_descr_init ---------------------------------*/

static void tx_descr_init(void)
{
  /* Initialize Transmit DMA Descriptor array. */
  u32 i,next;

  TxBufIndex = 0;
  for (i = 0, next = 0; i < NUM_TX_BUF; i++) {
    if (++next == NUM_TX_BUF) next = 0;
    Tx_Desc[i].CtrlStat = DMA_TX_TCH | DMA_TX_LS | DMA_TX_FS;
    Tx_Desc[i].Addr     = (u32)&tx_buf[i];
    Tx_Desc[i].Next     = (u32)&Tx_Desc[next];
  }
  ETH->DMATDLAR = (u32)&Tx_Desc[0];
}


/*--------------------------- write_PHY -------------------------------------*/

static void write_PHY(u32 PhyReg, u16 Value)
{
  /* Write a data 'Value' to PHY register 'PhyReg'. */
  u32 tout;

  ETH->MACMIIDR = Value;
  ETH->MACMIIAR = PHY_DEF_ADDR << 11 | PhyReg << 6 | MMAR_MW | MMAR_MB;

  /* Wait utill operation completed */
  tout = 0;
  for (tout = 0; tout < MII_WR_TOUT; tout++) {
    if ((ETH->MACMIIAR & MMAR_MB) == 0) {
      break;
    }
  }
}


/*--------------------------- read_PHY --------------------------------------*/

static u16 read_PHY(u32 PhyReg)
{
  /* Read a PHY register 'PhyReg'. */
  u32 tout;

  ETH->MACMIIAR = PHY_DEF_ADDR << 11 | PhyReg << 6 | MMAR_MB;

  /* Wait until operation completed */
  tout = 0;
  for (tout = 0; tout < MII_RD_TOUT; tout++) {
    if ((ETH->MACMIIAR & MMAR_MB) == 0) {
      break;
    }
  }
  return (ETH->MACMIIDR & MMDR_MD);
}




#endif



