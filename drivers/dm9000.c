#include <drivers/dm9000.h>



#define DM9000_DEBUG		0
#if DM9000_DEBUG
#define DM9000_DBGOUT		printf
#else
#define DM9000_DBGOUT(...)
#endif

//Private Defines
#define DM9000_PHY			0x40    /* PHY address 0x01 */

#define MAX_ADDR_LEN 		6

#define DM9000_ID			0x90000A46  /* DM9000AEP ID */
#define DM9000_PKT_MAX		1536	    /* Received packet max size */
#define DM9000_PKT_RDY		0x01	    /* Packet ready to receive */

#define DM9000_NCR			0x00
#define DM9000_NSR			0x01
#define DM9000_TCR			0x02
#define DM9000_TSR1			0x03
#define DM9000_TSR2			0x04
#define DM9000_RCR			0x05
#define DM9000_RSR			0x06
#define DM9000_ROCR			0x07
#define DM9000_BPTR			0x08
#define DM9000_FCTR			0x09
#define DM9000_FCR			0x0A
#define DM9000_EPCR			0x0B
#define DM9000_EPAR			0x0C
#define DM9000_EPDRL		0x0D
#define DM9000_EPDRH		0x0E
#define DM9000_WCR			0x0F

#define DM9000_PAR			0x10
#define DM9000_MAR			0x16

#define DM9000_GPCR			0x1E
#define DM9000_GPR			0x1F
#define DM9000_TRPAL		0x22
#define DM9000_TRPAH		0x23
#define DM9000_RWPAL		0x24
#define DM9000_RWPAH		0x25

#define DM9000_VIDL			0x28
#define DM9000_VIDH			0x29
#define DM9000_PIDL			0x2A
#define DM9000_PIDH			0x2B

#define DM9000_CHIPR		0x2C
#define DM9000_TCR2			0x2D
#define DM9000_OTCR			0x2E
#define DM9000_SMCR			0x2F

#define DM9000_ETCR			0x30	/* early transmit control/status register */
#define DM9000_CSCR			0x31	/* check sum control register */
#define DM9000_RCSSR		0x32	/* receive check sum status register */

#define DM9000_LEDCR		0x34
#define DM9000_BUSCR		0x38
#define DM9000_INTCR		0x39
#define DM9000_SCCR			0x05

#define DM9000_MRCMDX		0xF0
#define DM9000_MRCMD		0xF2
#define DM9000_MRRL			0xF4
#define DM9000_MRRH			0xF5
#define DM9000_MWCMDX		0xF6
#define DM9000_MWCMD		0xF8
#define DM9000_MWRL			0xFA
#define DM9000_MWRH			0xFB
#define DM9000_TXPLL		0xFC
#define DM9000_TXPLH		0xFD
#define DM9000_ISR			0xFE
#define DM9000_IMR			0xFF

#define CHIPR_DM9000A		0x19
#define CHIPR_DM9000C		0x1A
#define CHIPR_DM9000B		0x1B

#define NCR_EXT_PHY         (1<<7)
#define NCR_WAKEEN          (1<<6)
#define NCR_FCOL            (1<<4)
#define NCR_FDX             (1<<3)
#define NCR_LBK             (3<<1)
#define NCR_RST             (1<<0)

#define NSR_SPEED           (1<<7)
#define NSR_LINKST          (1<<6)
#define NSR_WAKEST          (1<<5)
#define NSR_TX2END          (1<<3)
#define NSR_TX1END          (1<<2)
#define NSR_RXOV            (1<<1)

#define TCR_TJDIS           (1<<6)
#define TCR_EXCECM          (1<<5)
#define TCR_PAD_DIS2        (1<<4)
#define TCR_CRC_DIS2        (1<<3)
#define TCR_PAD_DIS1        (1<<2)
#define TCR_CRC_DIS1        (1<<1)
#define TCR_TXREQ           (1<<0)

#define TSR_TJTO            (1<<7)
#define TSR_LC              (1<<6)
#define TSR_NC              (1<<5)
#define TSR_LCOL            (1<<4)
#define TSR_COL             (1<<3)
#define TSR_EC              (1<<2)

#define RCR_WTDIS           (1<<6)
#define RCR_DIS_LONG        (1<<5)
#define RCR_DIS_CRC         (1<<4)
#define RCR_ALL             (1<<3)
#define RCR_RUNT            (1<<2)
#define RCR_PRMSC           (1<<1)
#define RCR_RXEN            (1<<0)

#define RSR_RF              (1<<7)
#define RSR_MF              (1<<6)
#define RSR_LCS             (1<<5)
#define RSR_RWTO            (1<<4)
#define RSR_PLE             (1<<3)
#define RSR_AE              (1<<2)
#define RSR_CE              (1<<1)
#define RSR_FOE             (1<<0)

#define FCTR_HWOT(ot)		(((ot) & 0x0F ) << 4)
#define FCTR_LWOT(ot)		((ot) & 0x0F )

#define IMR_PAR             (1<<7)
#define IMR_ROOM            (1<<3)
#define IMR_ROM             (1<<2)
#define IMR_PTM             (1<<1)
#define IMR_PRM             (1<<0)

#define ISR_ROOS            (1<<3)
#define ISR_ROS             (1<<2)
#define ISR_PTS             (1<<1)
#define ISR_PRS             (1<<0)
#define ISR_CLR_STATUS      (ISR_ROOS | ISR_ROS | ISR_PTS | ISR_PRS)

#define EPCR_REEP           (1<<5)
#define EPCR_WEP            (1<<4)
#define EPCR_EPOS           (1<<3)
#define EPCR_ERPRR          (1<<2)
#define EPCR_ERPRW          (1<<1)
#define EPCR_ERRE           (1<<0)

#define GPCR_GEP_CNTL       (1<<0)


enum DM9000_PHY_mode
{
    DM9000_10MHD = 0, DM9000_100MHD = 1,
    DM9000_10MFD = 4, DM9000_100MFD = 5,
    DM9000_AUTO  = 8, DM9000_1M_HPNA = 0x10
};

enum DM9000_TYPE
{
    TYPE_DM9000E,
    TYPE_DM9000A,
    TYPE_DM9000B
};


//Private Macros
#define dm9000_Rst(x)			sys_GpioSet(gpio_node(tbl_bspDm9000, 0), (x))

//Private Variables


/* Read a byte from I/O port */
static u16 dm9000_RegRead(u16 reg)
{

	__raw_writew(reg, DM9000_ADR_CMD);
	return __raw_readw(DM9000_ADR_DATA);
}

/* Write a byte to I/O port */
static void dm9000_RegWrite(u16 reg, u16 value)
{

	__raw_writew(reg, DM9000_ADR_CMD);
	__raw_writew(value, DM9000_ADR_DATA);
}

/* Read a word from phyxcer */
static u16 phy_read(u16 reg)
{
	size_t i = 100000;
	int val;

	/* Fill the phyxcer register into REG_0C */
	dm9000_RegWrite(DM9000_EPAR, DM9000_PHY | reg);
	dm9000_RegWrite(DM9000_EPCR, 0x0C);	/* Issue phyxcer read command */

	while ((dm9000_RegRead(DM9000_EPCR) & BITMASK(0)) && i--);/* Wait read complete */

	val = (dm9000_RegRead(DM9000_EPDRH) << 8) | dm9000_RegRead(DM9000_EPDRL);
	dm9000_RegWrite(DM9000_EPCR, 0x08);	/* Clear phyxcer read command */

	return val;
}

/* Write a word to phyxcer */
static void phy_write(u16 reg, u16 value)
{
	size_t i = 100000;

	/* Fill the phyxcer register into REG_0C */
	dm9000_RegWrite(DM9000_EPAR, DM9000_PHY | reg);

	/* Fill the written data into REG_0D & REG_0E */
	dm9000_RegWrite(DM9000_EPDRL, (value & 0xff));
	dm9000_RegWrite(DM9000_EPDRH, ((value >> 8) & 0xff));
	dm9000_RegWrite(DM9000_EPCR, 0x0A);	/* Issue phyxcer write command */

	while ((dm9000_RegRead(DM9000_EPCR) & BITMASK(0)) && i--);/* Wait write complete */

	dm9000_RegWrite(DM9000_EPCR, 0x08);	/* Clear phyxcer write command */
}

/* Set PHY operationg mode */
static void phy_mode_set(int media_mode)
{
	u16 phy_reg4 = 0x01e1, phy_reg0 = 0x1000;

	if (!(media_mode & DM9000_AUTO))
	{
		switch (media_mode)
		{
		case DM9000_10MHD:
			phy_reg4 = 0x21;
			phy_reg0 = 0x0000;
			break;
		case DM9000_10MFD:
			phy_reg4 = 0x41;
			phy_reg0 = 0x1100;
			break;
		case DM9000_100MHD:
			phy_reg4 = 0x81;
			phy_reg0 = 0x2000;
			break;
		case DM9000_100MFD:
			phy_reg4 = 0x101;
			phy_reg0 = 0x3100;
			break;
		}
		phy_write(4, phy_reg4);	/* Set PHY media mode */
		phy_write(0, phy_reg0);	/*  Tmp */
	}

	dm9000_RegWrite(DM9000_GPR, 0x00);	/* Enable PHY */
}

#if TCPPS_TYPE == TCPPS_T_KEILTCP
static void dm9000_Isr(void *args)
{
	u16 nReg;

	/* Got DM9000 interrupt status */
	nReg = dm9000_RegRead(DM9000_ISR);               /* Got ISR */
	dm9000_RegWrite(DM9000_ISR, nReg);    /* Clear ISR status */

	DM9000_DBGOUT("dm9000 isr: int status %04x\n", nReg);

#if DM9000_DEBUG
	/* receive overflow */
	if (nReg & ISR_ROS)
		DM9000_DBGOUT("overflow\n");

	if (nReg & ISR_ROOS)
		DM9000_DBGOUT("overflow counter overflow\n");

	/* Transmit Interrupt check */
	if (nReg & ISR_PTS)
		DM9000_DBGOUT("dm9000 isr: tx packet\n");
#endif

	/* Received the coming packet */
	if (nReg & ISR_PRS)
		while (dm9000_PacketReceive() == SYS_R_OK);
}

sys_res dm9000_Reset()
{
	size_t i;
	u32 value;

	/* Reset DM9000,
	   see DM9000 Application Notes V1.22 Jun 11, 2004 page 29 */

	/* Hardware Reset */
	dm9000_Rst(0);
	os_thd_slp1tick();
	dm9000_Rst(1);
	os_thd_sleep(10);
	/* DEBUG: Make all GPIO pins outputs */
	dm9000_RegWrite(DM9000_GPCR, 0x0F);
	/* Step 1: Power internal PHY by writing 0 to GPIO0 pin */
	dm9000_RegWrite(DM9000_GPR, 0);
	/* Step 2: Software reset */
	dm9000_RegWrite(DM9000_NCR, 3);

	for (i = 10000; dm9000_RegRead(DM9000_NCR) & NCR_RST; i--)
	{
		sys_Delay(100);		/* delay 10us */
	}
	
	os_thd_sleep(100);

	/* identify DM9000 */
	value  = dm9000_RegRead(DM9000_VIDL);
	value |= dm9000_RegRead(DM9000_VIDH) << 8;
	value |= dm9000_RegRead(DM9000_PIDL) << 16;
	value |= dm9000_RegRead(DM9000_PIDH) << 24;
	if (value != DM9000_ID)
	{
		DM9000_DBGOUT("dm9000 id: 0x%x\n", value);
	    return SYS_R_ERR;
	}
	
	/* Set PHY */
	phy_mode_set(DM9000_AUTO);

	/* Program operating register */
	dm9000_RegWrite(DM9000_NCR, 0x00);	/* only intern phy supported by now */
	dm9000_RegWrite(DM9000_TCR, 0x00);	    /* TX Polling clear */
	dm9000_RegWrite(DM9000_BPTR, 0x3F);	/* Less 3Kb, 200us */
	dm9000_RegWrite(DM9000_FCTR, FCTR_HWOT(3) | FCTR_LWOT(8));	/* Flow Control : High/Low Water */
	dm9000_RegWrite(DM9000_FCR, 0x00);	/* SH FIXME: This looks strange! Flow Control */
	dm9000_RegWrite(DM9000_SMCR, 0x00);	/* Special Mode */
	dm9000_RegWrite(DM9000_NSR, NSR_WAKEST | NSR_TX2END | NSR_TX1END);	/* clear TX status */
	dm9000_RegWrite(DM9000_ISR, 0x0F);	/* Clear interrupt status */

	/* set multicast address */
	for (i = 0x16; i < 0x1E; i++)
	{
		dm9000_RegWrite(i, 0xFF);
	}

	/* Activate DM9000 */
	dm9000_RegWrite(DM9000_RCR, RCR_DIS_LONG | RCR_DIS_CRC | RCR_RXEN);	/* RX enable */

#if ETH_INT_ENABLE
	dm9000_RegWrite(DM9000_IMR, IMR_PAR | IMR_PRM);	/* Enable RX interrupt mask */

	return irq_ExtRegister(gpio_node(tbl_bspDm9000, 1)->port, gpio_node(tbl_bspDm9000, 1)->pin, IRQ_TRIGGER_FALLING, dm9000_Isr, NULL, IRQ_MODE_HALF);
#else
	dm9000_RegWrite(DM9000_IMR, IMR_PAR); /* Disable RX interrupt mask */

	return SYS_R_OK;
#endif
}


void dm9000_SetMAC(const u8 *pMac)
{
	int i;

	/* set mac address */
	for (i = 0x10; i < 0x16; i++)
	{
		dm9000_RegWrite(i, pMac[i - 0x10]);
	}
}

sys_res dm9000_PacketSend(const void *pBuf, size_t nLen)
{
	size_t i = 10000;
	u16 *pData = (u16 *)pBuf;

	DM9000_DBGOUT("dm9000 tx: %d\n", nLen);

	/* lock DM9000 device */
	
	/* Wait transmit done */
	while ((dm9000_RegRead(DM9000_TCR) & TCR_TXREQ) && i--);	  /* Got TX status */

	/* Move data to DM9000 TX RAM */
	__raw_writew(DM9000_MWCMD, DM9000_ADR_CMD);
	
	/* Write data into dm9000a, two bytes at a time
	* Handling pbuf's with odd number of bytes correctly
	* No attempt to optimize for speed has been made */
	for (i = ALIGN2(nLen); i; i -= 2)
	{
		__raw_writew(*pData++, DM9000_ADR_DATA);
	}
		
	dm9000_RegWrite(DM9000_TXPLL, nLen & 0xFF);
	dm9000_RegWrite(DM9000_TXPLH, (nLen >> 8) & 0xFF);
	
	/* Issue TX polling command */
	dm9000_RegWrite(DM9000_TCR, TCR_TXREQ);	/* Cleared after TX complete */

	/* unlock DM9000 device */

	DM9000_DBGOUT("dm9000 tx done\n");
	return SYS_R_OK;
}

sys_res dm9000_Control(int nCmd, void *args)
{
	u16 nTemp;

	switch (nCmd)
	{
	case 0:
		dm9000_RegWrite(DM9000_IMR, IMR_PAR); /* Disable RX interrupt mask */
		break;
	case 1:
		dm9000_RegWrite(DM9000_IMR, IMR_PAR | IMR_PRM); /* Enable RX interrupt mask */
		break;
	case 2:
		nTemp = dm9000_RegRead(DM9000_NSR);
		*(int *)args = (nTemp & NSR_LINKST) ? 1 : 0;
		break;
	default :
		break;
	}
	return SYS_R_OK;
}

sys_res dm9000_PacketReceive()
{
	OS_FRAME *frame;
	u16 *pData;
	int nSte, nLen;

	/* Check packet ready or not */
	dm9000_RegRead(DM9000_MRCMDX);				/* Dummy read */
	nSte = __raw_readw(DM9000_ADR_DATA) & 0xFF;		/* Got most updated pData */
	if (nSte == 0)
		return SYS_R_ERR;
	
	if (nSte > 1)
	{
		DM9000_DBGOUT("dm9000 rx: rx error, stop device\n");

		dm9000_RegWrite(DM9000_RCR, 0x00);	/* Stop Device */
		dm9000_RegWrite(DM9000_ISR, 0x80);	/* Stop INT request */
	}

	/* A packet ready now  & Get status/length */
	__raw_writew(DM9000_MRCMD, DM9000_ADR_CMD);

	nSte = __raw_readw(DM9000_ADR_DATA);
	nLen = __raw_readw(DM9000_ADR_DATA);

	DM9000_DBGOUT("dm9000 rx: status %04x nLen %d\n", nSte, nLen);

	if ((nSte & 0xbf00) || (nLen < 0x40) || (nLen > DM9000_PKT_MAX))
	{
#if DM9000_DEBUG
		if (nSte & 0x100)
			DM9000_DBGOUT("rx fifo error\n");
		if (nSte & 0x200)
			DM9000_DBGOUT("rx crc error\n");
		if (nSte & 0x8000)
			DM9000_DBGOUT("rx length error\n");
		if (nLen > DM9000_PKT_MAX)
			DM9000_DBGOUT("rx length too big\n");
#endif
		/* RESET device */
		dm9000_RegWrite(DM9000_NCR, NCR_RST);
		os_thd_slp1tick();
		
		return SYS_R_ERR;
	}
	
	/* Flag 0x80000000 to skip sys_error() call when out of memory. */
	frame = alloc_mem(ALIGN4(nLen) | 0x80000000);
	/* if 'alloc_mem()' has failed, ignore this packet. */
	if (frame != NULL)
	{
		//copy the packet from the receive buffer
		pData = (u16 *)frame->data;
		frame->length = nLen;
		for (nLen = ALIGN2(nLen); nLen; nLen -= 2) {
			*pData++ = __raw_readw(DM9000_ADR_DATA);
		}
		put_in_queue(frame);
	}
	else
	{
		DM9000_DBGOUT("dm9000 rx: no pbuf\n");
		/* no pbuf, discard pData from DM9000 */
		for (nLen = ALIGN2(nLen); nLen; nLen -= 2)
		{
			__raw_readw(DM9000_ADR_DATA);
		}
	}

	return SYS_R_OK;
}

void dm9000_Init()
{

	/* GPIO Initial */
	sys_GpioConf(gpio_node(tbl_bspDm9000, 0));
	sys_GpioConf(gpio_node(tbl_bspDm9000, 1));
}

#endif



#ifdef RT_USING_FINSH
void dm9000reg(void)
{

    rt_kprintf("\n");
    rt_kprintf("NCR   (0x00): %02x\n", dm9000_RegRead(DM9000_NCR));
    rt_kprintf("NSR   (0x01): %02x\n", dm9000_RegRead(DM9000_NSR));
    rt_kprintf("TCR   (0x02): %02x\n", dm9000_RegRead(DM9000_TCR));
    rt_kprintf("TSRI  (0x03): %02x\n", dm9000_RegRead(DM9000_TSR1));
    rt_kprintf("TSRII (0x04): %02x\n", dm9000_RegRead(DM9000_TSR2));
    rt_kprintf("RCR   (0x05): %02x\n", dm9000_RegRead(DM9000_RCR));
    rt_kprintf("RSR   (0x06): %02x\n", dm9000_RegRead(DM9000_RSR));
    rt_kprintf("ORCR  (0x07): %02x\n", dm9000_RegRead(DM9000_ROCR));
    rt_kprintf("CRR   (0x2C): %02x\n", dm9000_RegRead(DM9000_CHIPR));
    rt_kprintf("CSCR  (0x31): %02x\n", dm9000_RegRead(DM9000_CSCR));
    rt_kprintf("RCSSR (0x32): %02x\n", dm9000_RegRead(DM9000_RCSSR));
    rt_kprintf("ISR   (0xFE): %02x\n", dm9000_RegRead(DM9000_ISR));
    rt_kprintf("IMR   (0xFF): %02x\n", dm9000_RegRead(DM9000_IMR));
    rt_kprintf("\n");
}
FINSH_FUNCTION_EXPORT(dm9000reg, dm9000 register dump);
#endif




