#if LPC176X_ETH_ENABLE

#define EMAC_PHY_AUTO		0
#define EMAC_PHY_10MBIT		1
#define EMAC_PHY_100MBIT	2

#define MAX_ADDR_LEN 6


/* Local Function Prototypes */
static void write_PHY (u32 PhyReg, u32 Value);
static u16 read_PHY (u8 PhyReg) ;

/* phy write */
static void write_PHY (u32 PhyReg, u32 Value)
{
	unsigned int tout;

	LPC_EMAC->MADR = PHY_DEF_ADDR | PhyReg;
	LPC_EMAC->MWTD = Value;

	/* Wait utill operation completed */
	tout = 0;
	for (tout = 0; tout < MII_WR_TOUT; tout++)
	{
		if ((LPC_EMAC->MIND & MIND_BUSY) == 0)
		{
			break;
		}
	}
}

/* phy read */
static u16 read_PHY (u8 PhyReg)
{
	u32 tout;

	LPC_EMAC->MADR = PHY_DEF_ADDR | PhyReg;
	LPC_EMAC->MCMD = MCMD_READ;

	/* Wait until operation completed */
	tout = 0;
	for (tout = 0; tout < MII_RD_TOUT; tout++)
	{
		if ((LPC_EMAC->MIND & MIND_BUSY) == 0)
		{
			break;
		}
	}
	LPC_EMAC->MCMD = 0;
	return (LPC_EMAC->MRDD);
}

/* init rx descriptor */
__inline void rx_descr_init (void)
{
	u32 i;

	for (i = 0; i < NUM_RX_FRAG; i++)
	{
		RX_DESC_PACKET(i)  = RX_BUF(i);
		RX_DESC_CTRL(i)    = RCTRL_INT | (ETH_FRAG_SIZE-1);
		RX_STAT_INFO(i)    = 0;
		RX_STAT_HASHCRC(i) = 0;
	}

	/* Set EMAC Receive Descriptor Registers. */
	LPC_EMAC->RxDescriptor    = RX_DESC_BASE;
	LPC_EMAC->RxStatus        = RX_STAT_BASE;
	LPC_EMAC->RxDescriptorNumber = NUM_RX_FRAG-1;

	/* Rx Descriptors Point to 0 */
	LPC_EMAC->RxConsumeIndex  = 0;
}

/* init tx descriptor */
__inline void tx_descr_init (void)
{
	u32 i;

	for (i = 0; i < NUM_TX_FRAG; i++)
	{
		TX_DESC_PACKET(i) = TX_BUF(i);
		TX_DESC_CTRL(i)   = (1ul<<31) | (1ul<<30) | (1ul<<29) | (1ul<<28) | (1ul<<26) | (ETH_FRAG_SIZE-1);
		TX_STAT_INFO(i)   = 0;
	}

	/* Set EMAC Transmit Descriptor Registers. */
	LPC_EMAC->TxDescriptor    = TX_DESC_BASE;
	LPC_EMAC->TxStatus        = TX_STAT_BASE;
	LPC_EMAC->TxDescriptorNumber = NUM_TX_FRAG-1;

	/* Tx Descriptors Point to 0 */
	LPC_EMAC->TxProduceIndex  = 0;
}



//External Functions
int arch_EmacInit()
{
	/* Initialize the EMAC ethernet controller. */
	u32 regv, tout, id1, id2;

	/* Power Up the EMAC controller. */
	LPC_SC->PCONP |= 0x40000000;

	/* Enable P1 Ethernet Pins. */
	LPC_PINCON->PINSEL2 = 0x50150105;
	LPC_PINCON->PINSEL3 = (LPC_PINCON->PINSEL3 & ~0x0000000F) | 0x00000005;

	/* Reset all EMAC internal modules. */
	LPC_EMAC->MAC1 = MAC1_RES_TX | MAC1_RES_MCS_TX | MAC1_RES_RX | MAC1_RES_MCS_RX | MAC1_SIM_RES | MAC1_SOFT_RES;
	LPC_EMAC->Command = CR_REG_RES | CR_TX_RES | CR_RX_RES;

	/* A short delay after reset. */
	for (tout = 100; tout; tout--);

	/* Initialize MAC control registers. */
	LPC_EMAC->MAC1 = MAC1_PASS_ALL;
	LPC_EMAC->MAC2 = MAC2_CRC_EN | MAC2_PAD_EN;
	LPC_EMAC->MAXF = ETH_MAX_FLEN;
	LPC_EMAC->CLRT = CLRT_DEF;
	LPC_EMAC->IPGR = IPGR_DEF;

	/* PCLK=18MHz, clock select=6, MDC=18/6=3MHz */
	/* Enable Reduced MII interface. */
	LPC_EMAC->MCFG = MCFG_CLK_DIV20 | MCFG_RES_MII;
	for (tout = 100; tout; tout--);
	LPC_EMAC->MCFG = MCFG_CLK_DIV20;

	/* Enable Reduced MII interface. */
	LPC_EMAC->Command = CR_RMII | CR_PASS_RUNT_FRM | CR_PASS_RX_FILT;

	/* Reset Reduced MII Logic. */
	LPC_EMAC->SUPP = SUPP_RES_RMII | SUPP_SPEED;
	for (tout = 100; tout; tout--);
	LPC_EMAC->SUPP = SUPP_SPEED;

	/* Put the PHY in reset mode */
	write_PHY (PHY_REG_BMCR, 0x8000);
	for (tout = 1000; tout; tout--);

	/* Wait for hardware reset to end. */
	for (tout = 0; tout < 0x100000; tout++)
	{
		regv = read_PHY (PHY_REG_BMCR);
		if (!(regv & 0x8000))
		{
			/* Reset complete */
			break;
		}
	}
	if (tout >= 0x100000) return (-1); /* reset failed */

	/* Check if this is a DP83848C PHY. */
	id1 = read_PHY (PHY_REG_IDR1);
	id2 = read_PHY (PHY_REG_IDR2);

	if (((id1 << 16) | (id2 & 0xFFF0)) != PHY_ID_DP83848C)
		return (-1);

	/* Configure the PHY device */
	switch (EMAC_PHY_AUTO)
	{
	case EMAC_PHY_AUTO:
		/* Use autonegotiation about the link speed. */
		write_PHY (PHY_REG_BMCR, PHY_AUTO_NEG);
		break;
	case EMAC_PHY_10MBIT:
		/* Connect at 10MBit */
		write_PHY (PHY_REG_BMCR, PHY_FULLD_10M);
		break;
	case EMAC_PHY_100MBIT:
		/* Connect at 100MBit */
		write_PHY (PHY_REG_BMCR, PHY_FULLD_100M);
		break;
	}

	return 0;
}

void arch_EmacAddr(u8 *pAdr)
{
	uint regv = 0x0004;

	/* Configure Full/Half Duplex mode. */
	if (regv & 0x0004)
	{
		/* Full duplex is enabled. */
		LPC_EMAC->MAC2	  |= MAC2_FULL_DUP;
		LPC_EMAC->Command |= CR_FULL_DUP;
		LPC_EMAC->IPGT	   = IPGT_FULL_DUP;
	}
	else
	{
		/* Half duplex mode. */
		LPC_EMAC->IPGT = IPGT_HALF_DUP;
	}

	/* Configure 100MBit/10MBit mode. */
	if (regv & 0x0002)
	{
		/* 10MBit mode. */
		LPC_EMAC->SUPP = 0;
	}
	else
	{
		/* 100MBit mode. */
		LPC_EMAC->SUPP = SUPP_SPEED;
	}

	/* Set the Ethernet MAC Address registers */
	LPC_EMAC->SA0 = pAdr[1] << 8 | pAdr[0];
	LPC_EMAC->SA1 = pAdr[3] << 8 | pAdr[2];
	LPC_EMAC->SA2 = pAdr[5] << 8 | pAdr[4];

	/* Initialize Tx and Rx DMA Descriptors */
	rx_descr_init();
	tx_descr_init();

	/* Receive Broadcast and Perfect Match Packets */
	LPC_EMAC->RxFilterCtrl = RFC_BCAST_EN | RFC_PERFECT_EN;

	/* Reset all interrupts */
	LPC_EMAC->IntClear	= 0xFFFF;

	/* Enable EMAC interrupts. */
	LPC_EMAC->IntEnable = INT_RX_DONE;

	/* Enable receive and transmit mode of MAC Ethernet core */
	LPC_EMAC->Command  |= (CR_RX_EN | CR_TX_EN);
	LPC_EMAC->MAC1	   |= MAC1_REC_EN;

#if ETH_INT_ENABLE
	/* Enable the ENET Interrupt */
	NVIC_EnableIRQ(ENET_IRQn);
#endif
}

void arch_EmacIntEnable()
{

	/* Enable RxDone interrupt */
	LPC_EMAC->IntEnable = INT_RX_DONE;
}


void arch_EmacIntDisable()
{

	/* Disable EMAC RxDone interrupts. */
	LPC_EMAC->IntEnable = 0;
}

void arch_EmacPacketTx(const void *pData, uint nLen)
{
	u32 Index, IndexNext;
	u8 *ptr;

	/* get produce index */
	Index = LPC_EMAC->TxProduceIndex;

	/* calculate next index */
	IndexNext = LPC_EMAC->TxProduceIndex + 1;
	if (IndexNext > LPC_EMAC->TxDescriptorNumber)
		IndexNext = 0;

	/* copy pData to tx buffer */
	ptr = (u8*)TX_BUF(Index);
	memcpy(ptr, pData, nLen);

	TX_DESC_CTRL(Index) &= ~0x7ff;
	TX_DESC_CTRL(Index) |= (nLen - 1) & 0x7ff;

	/* change index to the next */
	LPC_EMAC->TxProduceIndex = IndexNext;
}



#include <net/rtxip/Net_Config.h>

void arch_EmacIsr()
{
	u32 status, size, Index;
	OS_FRAME *frame;

	while (status = LPC_EMAC->IntStatus & LPC_EMAC->IntEnable)
	{

		/* Clear the interrupt. */ 
		LPC_EMAC->IntClear = status; 
	 
		if (status & INT_RX_DONE)
		{

			/* Disable EMAC RxDone interrupts. */
			LPC_EMAC->IntEnable = 0;

			/* a frame has been received */
			Index = LPC_EMAC->RxConsumeIndex;
			if (Index != LPC_EMAC->RxProduceIndex)
			{
				size = (RX_STAT_INFO(Index) & 0x7ff) + 1;
				if (size > ETH_FRAG_SIZE) size = ETH_FRAG_SIZE;

				/* Receive packet */
				/* Flag 0x80000000 to skip sys_error() call when out of memory. */
				frame = alloc_mem(ALIGN4(size) | 0x80000000);
				/* if 'alloc_mem()' has failed, ignore this packet. */
				if (frame != NULL)
				{
					/* copy the packet from the receive buffer */
					memcpy(frame->data, (void *)RX_BUF(Index), size);
					frame->length = size;
					put_in_queue(frame);
				}
				
				/* move Index to the next */
				if(++Index > LPC_EMAC->RxDescriptorNumber)
					Index = 0;

				/* set consume index */
				LPC_EMAC->RxConsumeIndex = Index;
			}

			/* Enable RxDone interrupt */
			LPC_EMAC->IntEnable = INT_RX_DONE;
		}
	}
}





#endif

