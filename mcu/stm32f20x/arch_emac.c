#if STM32_ETH_ENABLE

#define ETH_SPEED_MODE_10MB		0

/* The following macro definitions may be used to select the speed
   of the physical link:

  _10MBIT_   - connect at 10 MBit only
  _100MBIT_  - connect at 100 MBit only

  By default an autonegotiation of the link speed is used. This may take 
  longer to connect, but it works for 10MBit and 100MBit physical links.
*/

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

/*-----------------------------------------------------------------------------
 *      ENET Ethernet Driver Functions
 *-----------------------------------------------------------------------------
 *  Required functions for Ethernet driver module:
 *  a. Polling mode: - void init_ethernet ()
 *                   - void send_frame (OS_FRAME *frame)
 *                   - void poll_ethernet (void)
 *  b. Interrupt mode: - void init_ethernet ()
 *                     - void send_frame (OS_FRAME *frame)
 *                     - void int_enable_eth ()
 *                     - void int_disable_eth ()
 *                     - interrupt function 
 *----------------------------------------------------------------------------*/

/* Local Function Prototypes */
static void rx_descr_init (void);
static void tx_descr_init (void);
static void write_PHY (u32 PhyReg, u16 Value);
static u16  read_PHY (u32 PhyReg);


/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
static void ETH_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOs clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC |
							RCC_AHB1Periph_GPIOG |RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOI
							, ENABLE);

	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);  

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

#if ETH_CLK4CPU_ENABLE
	/* Configure MCO (PA8) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);

#if ETH_MAC_MODE_MII
	/* Output PLL clock divided by 2 (25MHz) on MCO pin (PA8) to clock the PHY */
	RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_1);
#else
	/* Output PLL clock divided by 2 (50MHz) on MCO pin (PA8) to clock the PHY */
	RCC_MCO1Config(RCC_MCO1Source_PLLCLK, RCC_MCO2Div_2);
#endif
#endif

#if ETH_MAC_MODE_MII
	SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_MII);
#else
	SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);
#endif

	/* Ethernet pins configuration ************************************************/

	/*
	    ETH_MDIO -------------------------> PA2
	    ETH_MDC --------------------------> PC1
	    ETH_MII_RX_CLK/ETH_RMII_REF_CLK---> PA1
	    ETH_MII_RX_DV/ETH_RMII_CRS_DV ----> PA7
	    ETH_MII_RXD0/ETH_RMII_RXD0 -------> PC4
	    ETH_MII_RXD1/ETH_RMII_RXD1 -------> PC5
	    ETH_MII_TX_EN/ETH_RMII_TX_EN -----> PG11
	    ETH_MII_TXD0/ETH_RMII_TXD0 -------> PG13
	    ETH_MII_TXD1/ETH_RMII_TXD1 -------> PG14

		**** Not necessary ****
		ETH_PPS_OUT ----------------------> PB8

		**** Just for MII Mode ****
		ETH_MII_CRS ----------------------> PH2
		ETH_MII_COL ----------------------> PH3
		ETH_MII_TX_CLK -------------------> PC3
		ETH_MII_RX_ER --------------------> PI10
		ETH_MII_RXD2 ---------------------> PH6
		ETH_MII_RXD3 ---------------------> PH7
		ETH_MII_TXD2 ---------------------> PC2
		ETH_MII_TXD3 ---------------------> PB8
	                                              */
	/* Configure PC1, PC4 and PC5 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 |GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);

	/* Configure PG11, PG13 and PG14 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource11, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource13, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_ETH);

	/* Configure PA1, PA2 and PA7 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);

#if ETH_MAC_MODE_MII
	/* Configure PC2, PC3 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 |GPIO_Pin_3;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_ETH);

	/* Configure PH2, PH3, PH6 and PH7 */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 | GPIO_Pin_3, GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOH, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOH, GPIO_PinSource2, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOH, GPIO_PinSource3, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOH, GPIO_PinSource6, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOH, GPIO_PinSource7, GPIO_AF_ETH);

	/* Configure PB8 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_ETH);

	/* Configure PI10 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOI, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOI, GPIO_PinSource10, GPIO_AF_ETH);
#endif
}

int arch_EmacInit()
{
	/* Initialize the ETH ethernet controller. */
	u32 regv, tout;
//	u32 id1, id2;

	ETH_GPIO_Config();

	/* Enable clock for MAC. */
	RCC->AHB1ENR  |= 0x1E000000;

	/* Reset Ethernet MAC */
	RCC->AHB1RSTR |=  0x02000000;
	RCC->AHB1RSTR &= ~0x02000000;

	ETH->DMABMR  |= DBMR_SR;
	for (tout = 0; tout < 0x10000; tout++) {
		if ((ETH->DMABMR & DBMR_SR) == 0)
			break;
	}
	if (tout >= 0x10000)
		return (-1);

	/* HCLK Clock range 100-120MHz. */
	ETH->MACMIIAR = 0x00000004;

	/* Put the DP83848C in reset mode */
	write_PHY (PHY_REG_BMCR, 0x8000);

	/* Wait for hardware reset to end. */
	for (tout = 0; tout < 0x8000; tout++) {
		regv = read_PHY (PHY_REG_BMCR);
		if (!(regv & 0x8800)) {
			/* Reset complete, device not Power Down. */
			break;
		}
	}

	/* Read PHY ID */
//	id1 = read_PHY (PHY_REG_IDR1);
//	id2 = read_PHY (PHY_REG_IDR2);

	/* Configure the PHY device */
#if ETH_SPEED_MODE_10MB
	/* Connect at 10MBit */
	write_PHY (PHY_REG_BMCR, PHY_FULLD_10M);
#else
	/* Use autonegotiation about the link speed. */
	write_PHY (PHY_REG_BMCR, PHY_AUTO_NEG);
#if 0
	/* Wait to complete Auto_Negotiation. */
	for (tout = 0; tout < 0x10000; tout++) {
		regv = read_PHY (PHY_REG_BMSR);
		if (regv & 0x0024) {
			/* Autonegotiation Complete and Link is on. */
			break;
		}
	}
#endif
#endif

	/* Initialize MAC control register */
	ETH->MACCR = MCR_ROD;

	/* Full duplex is enabled. */
	ETH->MACCR |= MCR_DM;

#if ETH_SPEED_MODE_10MB == 0
	/* 100MBit mode. */
	ETH->MACCR |= MCR_FES;
#endif
	
	/* MAC address filtering, accept multicast packets. */
	ETH->MACFFR = MFFR_HPF | MFFR_PAM;
	ETH->MACFCR = MFCR_ZQPD;

	/* Initialize Tx and Rx DMA Descriptors */
	rx_descr_init ();
	tx_descr_init ();

	/* Flush FIFO, start DMA Tx and Rx */
	ETH->DMAOMR = DOMR_FTF | DOMR_ST | DOMR_SR;

	/* Enable receiver and transmiter */
	ETH->MACCR |= MCR_TE | MCR_RE;

	/* Reset all interrupts */
	ETH->DMASR  = 0xFFFFFFFF;

#if ETH_INT_ENABLE
	/* Enable Rx and Tx interrupts. */
	ETH->DMAIER = INT_NISE | INT_AISE | INT_RBUIE | INT_RIE;
#endif
	return 0;
}

void arch_EmacAddr(const u8 *pAdr)
{

	/* Set the Ethernet MAC Address registers */
	ETH->MACA0HR =	((u32)pAdr[5] << 8) | (u32)pAdr[4];
	ETH->MACA0LR =	((u32)pAdr[3] << 24) | (u32)pAdr[2] << 16 |
					((u32)pAdr[1] << 8) | (u32)pAdr[0];
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


void arch_EmacIsr()
{
	/* Ethernet Controller Interrupt function. */
	OS_FRAME *frame;
	u32 i, RxLen;
	u32 *sp, *dp;

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
rel:	Rx_Desc[i].Stat = DMA_RX_OWN;

		if (++i == NUM_RX_BUF)
			i = 0;
		RxBufIndex = i;
	}
	while (!(Rx_Desc[i].Stat & DMA_RX_OWN));
	RxBufIndex = i;

	if (ETH->DMASR & INT_RBUIE) {
		/* Rx DMA suspended, resume DMA reception. */
		ETH->DMASR = INT_RBUIE;
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


