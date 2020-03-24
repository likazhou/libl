






static void ip_output(int len)
{
	int tmp;

	ppptbuf[0] = 0x00;
	ppptbuf[1] = 0x21;
	memcpy(IPT, &ipt_header, sizeof(ipt_header));

	tmp = ip_id++;
	IPT->ipid = htons(tmp);

	IPT->len = htons(len);
	
	IPT->chksum = 0;
	IPT->chksum = ~(ipchksum());

	tmp = IPT->proto;
	switch (tmp)
	{
#if ATSVR_TCP_ENABLE
	case IP_PROTO_TCP:
		TCPT->chksum = 0;
		TCPT->chksum = ~(upper_chksum(tmp));
		break;
#endif
#if ATSVR_UDP_ENABLE
	case IP_PROTO_UDP:
		UDPT->chksum = 0;
#if ATSVR_TCP_ENABLE
		UDPT->chksum = ~(upper_chksum(tmp));
#endif
		break;
#endif
	default:
		return;
	}
 	ppp_output(PPP_PROCLEN + len);
}

#if ATSVR_TCP_ENABLE
#include "tcp.c"
#endif

#if ATSVR_UDP_ENABLE
#include "udp.c"
#endif

static void ip_input()
{

	memcpy(&ipt_header, IPR, sizeof(ipt_header));
	ipt_header.destipaddr = IPR->srcipaddr;
	ipt_header.srcipaddr = IPR->destipaddr;

#if ATSVR_BDID_ENABLE
	atsvr_BDId4IP(&IPR->destipaddr);
#endif

	//TCP Packet
	switch (IPR->proto)
	{
#if ATSVR_TCP_ENABLE
	case IP_PROTO_TCP:
		tcp_input();
		break;
#endif
#if ATSVR_UDP_ENABLE
	case IP_PROTO_UDP:
		udp_input();
		break;
#endif
	default:
		break;
	}
}






