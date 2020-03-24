#include "ps_TcpIp.h>
#include "bsp_cfg.h>


//In bsp_cfgig.c
extern OS_MUT mut_SPI1;

//Internal Variables
static u8 enc28j60_nBank = 0;
static u16 enc28j60_nNextPacketPtr;


static u32 enc28j60_ReadOp(u32 op, u32 address)
{
	int nResult;
	
	os_mut_wait(mut_SPI1, 0xFFFF);
	
	ENC28J60_CS(0);
	spi_Transce(BSPI1, op | (address & ADDR_MASK));
	nResult = spi_Transce(BSPI1, 0xFF);
	//do dummy read if needed (for mac and mii, see datasheet page 29)
	if (address & 0x80)
	{
		nResult = spi_Transce(BSPI1, 0xFF);
	}
	ENC28J60_CS(1);
	
	os_mut_release(mut_SPI1);
	
	return nResult;
}

static void enc28j60_WriteOp(u32 op, u32 address, u32 data)
{
	
	os_mut_wait(mut_SPI1, 0xFFFF);
	
	ENC28J60_CS(0);
	//issue write command
	spi_Transce(BSPI1, op | (address & ADDR_MASK));
	//write data
	spi_Transce(BSPI1, data);
	ENC28J60_CS(1);
	
	os_mut_release(mut_SPI1);
}

static void enc28j60_ReadBuffer(u32 len, u8 *data)
{

	os_mut_wait(mut_SPI1, 0xFFFF);
	
	ENC28J60_CS(0);
	//issue read command
	spi_Transce(BSPI1, ENC28J60_READ_BUF_MEM);
	while (len--)
	{
		//read data
		*data++ = spi_Transce(BSPI1, 0xFF);
	}
	*data='\0';
	ENC28J60_CS(1);
	
	os_mut_release(mut_SPI1);
}

static void enc28j60_WriteBuffer(u32 len, u8 *data)
{

	os_mut_wait(mut_SPI1, 0xFFFF);
	
	ENC28J60_CS(0);
	//issue write command
	spi_Transce(BSPI1, ENC28J60_WRITE_BUF_MEM);
	while (len--)
	{
		spi_Transce(BSPI1, *data++);
	}
	ENC28J60_CS(1);
	
	os_mut_release(mut_SPI1);
}

static void enc28j60_SetBank(u32 address)
{
	//set the bank (if needed)
	if ((address & BANK_MASK) != enc28j60_nBank)
	{
		//set the bank
		enc28j60_WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1|ECON1_BSEL0));
		enc28j60_WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK)>>5);
		enc28j60_nBank = (address & BANK_MASK);
	}
}

static u32 enc28j60_Read(u32 address)
{
	//set the bank
	enc28j60_SetBank(address);
	
	//do the read
	return enc28j60_ReadOp(ENC28J60_READ_CTRL_REG, address);
}

static void enc28j60_Write(u32 address, u32 data)
{
	//set the bank
	enc28j60_SetBank(address);
	//do the write
	enc28j60_WriteOp(ENC28J60_WRITE_CTRL_REG, address, data);
}

static void enc28j60_PhyWrite(u32 address, u32 data)
{
	//set the PHY register address
	enc28j60_Write(MIREGADR, address);
	//write the PHY data
	enc28j60_Write(MIWRL, data);
	enc28j60_Write(MIWRH, data>>8);
	//wait until the PHY write completes
	while (enc28j60_ReadOp(ENC28J60_READ_CTRL_REG, MISTAT) & MISTAT_BUSY)
	{
		sys_Delay(100);
	}
}

static void enc28j60_HWReset()
{

	enc28j60_WriteOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
}

static void enc28j60_PacketSend(void *packet, size_t len)
{
	u32 nTmo = 100;

	//Set the write pointer to start of transmit buffer area
	enc28j60_Write(EWRPTL, TXSTART_INIT&0xFF);
	enc28j60_Write(EWRPTH, TXSTART_INIT>>8);

	//Set the TXND pointer to correspond to the packet size given
	enc28j60_Write(ETXNDL, (TXSTART_INIT+len+1)&0xFF);
	enc28j60_Write(ETXNDH, (TXSTART_INIT+len+1)>>8);

	//write per-packet control byte (0x00 means use macon3 settings)
	enc28j60_WriteOp(ENC28J60_WRITE_BUF_MEM, 0, 0x00);

	//copy the packet into the transmit buffer
	enc28j60_WriteBuffer(len, packet);

	//send the contents of the transmit buffer onto the network
	enc28j60_WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);

//#if !BSP_ENC28J60_INT
	//wait for send finish
	while (nTmo--)
	{
		if (enc28j60_Read(EIR) & EIR_TXIF)
		{
			//Clr EIR_TXIF
			enc28j60_WriteOp(ENC28J60_BIT_FIELD_CLR, EIR, EIR_TXIF);
			break;
		}
		sys_Delay(1000);
	}
//#endif
}

//Gets a packet from the network receive buffer, if one is available.
//The packet will by headed by an ethernet header.
//    maxlen  The maximum acceptable length of a retrieved packet.
//    packet  Pointer where packet data should be stored.
//Returns: Packet length in bytes if a packet was retrieved, zero otherwise.
static u32 enc28j60_PacketReceive()
{
	OS_FRAME *frame;
	u16 rxstat;
	u16 len;

	//Set the read pointer to the start of the received packet
	enc28j60_Write(ERDPTL, (enc28j60_nNextPacketPtr));
	enc28j60_Write(ERDPTH, (enc28j60_nNextPacketPtr)>>8);

	//read the next packet pointer
	enc28j60_nNextPacketPtr  = enc28j60_ReadOp(ENC28J60_READ_BUF_MEM, 0);
	enc28j60_nNextPacketPtr |= enc28j60_ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;

	//read the packet length (see datasheet page 43)
	len  = enc28j60_ReadOp(ENC28J60_READ_BUF_MEM, 0);
	len |= enc28j60_ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;

	len-=4; //remove the CRC count
	//read the receive status (see datasheet page 43)
	rxstat  = enc28j60_ReadOp(ENC28J60_READ_BUF_MEM, 0);
	rxstat |= enc28j60_ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;
	//limit retrieve length
	if (len>MAX_FRAMELEN-1)
		len=MAX_FRAMELEN-1;

	//check CRC and symbol errors (see datasheet page 44, table 7-3):
	//The ERXFCON.CRCEN is set by default. Normally we should not
	//need to check this.
	if ((rxstat & 0x80)==0)
	{
		//invalid
		len=0;
	}
	else
	{
		/* Flag 0x80000000 to skip sys_error() call when out of memory. */
		frame = alloc_mem(ALIGN4(len) | 0x80000000);
		/* if 'alloc_mem()' has failed, ignore this packet. */
		if (frame != NULL)
		{
			//copy the packet from the receive buffer
			enc28j60_ReadBuffer(len, frame->data);
			frame->length = len;
			put_in_queue(frame);
		}
   	}
	//Move the RX read pointer to the start of the next received packet
	//This frees the memory we just read out
	enc28j60_Write(ERXRDPTL, (enc28j60_nNextPacketPtr));
	enc28j60_Write(ERXRDPTH, (enc28j60_nNextPacketPtr)>>8);

	//decrement the packet counter indicate we are done with this packet
	enc28j60_WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
	
	return len;
}

static void enc28j60_IntHandler()
{
	u32 nState;
	
	nState = enc28j60_Read(EIR);
	if (nState & EIR_PKTIF)
	{
		//receive packet
		enc28j60_PacketReceive();
	}
	
	if (nState & EIR_TXERIF)
	{
		//Tx Reset
		enc28j60_WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRST);
		enc28j60_WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
		enc28j60_WriteOp(ENC28J60_BIT_FIELD_CLR, EIR, EIR_TXERIF);
	}
	
	if (nState & EIR_RXERIF)
	{
		//Rx Reset
		enc28j60_WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXRST);
		enc28j60_WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_RXRST);
		enc28j60_WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
		enc28j60_WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
		enc28j60_WriteOp(ENC28J60_BIT_FIELD_CLR, EIR, EIR_RXERIF);
	}
	
	if (nState & EIR_TXIF)
	{
		//Clr EIR_TXIF
		enc28j60_WriteOp(ENC28J60_BIT_FIELD_CLR, EIR, EIR_TXIF);
	}
}

void enc28j60_Init(u8 *macaddr)
{

	//initialize I/O
	GPIO_cfgig(GPIO2, BITMASK(10), GPIO_OUT_PP);	//ENC28J60  CS		2.10
	GPIO_cfgig(GPIO2, BITMASK(9), GPIO_IPUPD_WP);	//ENC28J60  INT		2.9

	ENC28J60_CS(1);

	//hardware reset
	enc28j60_HWReset();
	//enc28j60_WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
	//enc28j60_WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_RXEN);
	//check CLKRDY bit to see if reset is complete
	while (!(enc28j60_Read(ESTAT) & ESTAT_CLKRDY)) {
		os_dly_wait(200 / OS_TICK_MS);
	}

	//do bank 0 xff
	//initialize receive buffer
	//16-bit transfers, must write low byte first
	//set receive buffer start address
	enc28j60_nNextPacketPtr = RXSTART_INIT;
	//Rx start
	enc28j60_Write(ERXSTL, RXSTART_INIT&0xFF);
	enc28j60_Write(ERXSTH, RXSTART_INIT>>8);
	//set receive pointer address
	enc28j60_Write(ERXRDPTL, RXSTART_INIT&0xFF);
	enc28j60_Write(ERXRDPTH, RXSTART_INIT>>8);
	//RX end
	enc28j60_Write(ERXNDL, RXSTOP_INIT&0xFF);
	enc28j60_Write(ERXNDH, RXSTOP_INIT>>8);
	//TX start
	enc28j60_Write(ETXSTL, TXSTART_INIT&0xFF);
	enc28j60_Write(ETXSTH, TXSTART_INIT>>8);
	//TX end
	enc28j60_Write(ETXNDL, TXSTOP_INIT&0xFF);
	enc28j60_Write(ETXNDH, TXSTOP_INIT>>8);
	//do bank 1 xff, packet filter:
	//For broadcast packets we allow only ARP packtets
	//All other packets should be unicast only for our mac (MAADR)
	//
	//The pattern to match on is therefore
	//Type     ETH.DST
	//ARP      BROADCAST
	//06 08 -- ff ff ff ff ff ff -> ip checksum for theses bytes=f7f9
	//in binary these poitions are:11 0000 0011 1111
	//This is hex 303F->EPMM0=0x3f,EPMM1=0x30
#if BSP_ENC28J60_ARP_FILTER
	enc28j60_Write(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN);
	enc28j60_Write(EPMM0, 0x3F);
	enc28j60_Write(EPMM1, 0x30);
	enc28j60_Write(EPMCSL, 0xF9);
	enc28j60_Write(EPMCSH, 0xF7);
#endif
	//do bank 2 xff
	//enable MAC receive
	enc28j60_Write(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
	//bring MAC out of reset
	enc28j60_Write(MACON2, 0x00);
	//enable automatic padding to 60bytes and CRC operations
	enc28j60_WriteOp(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN|MACON3_FULDPX);
	//set inter-frame gap (non-back-to-back)
	enc28j60_Write(MAIPGL, 0x12);
	enc28j60_Write(MAIPGH, 0x0C);
	//set inter-frame gap (back-to-back)
	enc28j60_Write(MABBIPG, 0x12);
	//Set the maximum packet size which the controller will accept
	//Do not send packets longer than MAX_FRAMELEN:
	enc28j60_Write(MAMXFLL, MAX_FRAMELEN&0xFF);	
	enc28j60_Write(MAMXFLH, MAX_FRAMELEN>>8);
	//do bank 3 xff
	//write MAC address
	//NOTE: MAC address in ENC28J60 is byte-backward
	enc28j60_Write(MAADR5, macaddr[0]);
	enc28j60_Write(MAADR4, macaddr[1]);
	enc28j60_Write(MAADR3, macaddr[2]);
	enc28j60_Write(MAADR2, macaddr[3]);
	enc28j60_Write(MAADR1, macaddr[4]);
	enc28j60_Write(MAADR0, macaddr[5]);

	//full dpx
	enc28j60_PhyWrite(PHCON1, PHCON1_PDPXMD);
	//no loopback of transmitted frames
	enc28j60_PhyWrite(PHCON2, PHCON2_HDLDIS);
	//ÅäÖÃPHY×´Ì¬¼Ä´æÆ÷É¨Ãè
	enc28j60_Write(MIREGADR, PHSTAT2);
	enc28j60_WriteOp(ENC28J60_BIT_FIELD_SET, MICMD, MICMD_MIISCAN);

	//switch to bank 0
	enc28j60_SetBank(ECON1);
	//enable interrupts
#if BSP_ENC28J60_INT
	enc28j60_WriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_PKTIE|EIE_TXERIE|EIE_RXERIE);
#endif
	//enable packet reception
	enc28j60_WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
}

int enc28j60_IsPhyLinked()
{

	return (enc28j60_Read(MIRDH) & 0x04) ? 1 : 0;
}

