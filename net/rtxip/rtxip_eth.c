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

//Header Files
#include <drivers/dm9000.h>
#include <drivers/enc28j60.h>



//Local Variables
static int eth_inited;

void init_ethernet()
{

	eth_inited = 0;
#if ETH_EMAC_ENABLE
	sys_GpioConf(gpio_node(tbl_ethEmac, 0));
	sys_GpioSet(gpio_node(tbl_ethEmac, 0), 0);
	os_thd_slp1tick();
	sys_GpioSet(gpio_node(tbl_ethEmac, 0), 1);
	os_thd_sleep(500);
	if (arch_EmacInit() == 0)
		eth_inited = 1;
#endif
#if DM9000_ENABLE
	if (dm9000_Reset() == SYS_R_OK)
		eth_inited = 1;
#endif
}

void send_frame(OS_FRAME *frame)
{

	/* Send frame to ENET ethernet controller */
	if (eth_inited)
	{
#if ETH_EMAC_ENABLE
		arch_EmacPacketTx(&frame->data[0], frame->length);
#endif
#if DM9000_ENABLE
		dm9000_PacketSend(&frame->data[0], frame->length);
#endif
#if ENC28J60_ENABLE
		enc28j60_PacketSend(&frame->data[0], frame->length);
#endif
	}
}

#if ETH_INT_ENABLE
void int_enable_eth()
{

	if (eth_inited)
	{
#if ETH_EMAC_ENABLE
		arch_EmacIntEnable();
#endif
#if DM9000_ENABLE
		dm9000_Control(1, NULL);
#endif
#if ENC28J60_ENABLE
		enc28j60_WriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE);
#endif
	}
}

void int_disable_eth()
{

	if (eth_inited)
	{
#if ETH_EMAC_ENABLE
		arch_EmacIntDisable();
#endif
#if DM9000_ENABLE
		dm9000_Control(0, NULL);
#endif
#if ENC28J60_ENABLE
		enc28j60_WriteOp(ENC28J60_BIT_FIELD_CLR, EIE, EIE_INTIE);
#endif
	}
}

#else

void poll_ethernet()
{

	if (eth_inited)
	{
#if ETH_EMAC_ENABLE
		arch_EmacIsr();
#endif
#if DM9000_ENABLE
		dm9000_Isr(NULL);
#endif
#if ENC28J60_ENABLE
		enc28j60_IntHandler();
#endif
	}
}

#endif


