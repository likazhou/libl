

#include "ppp.h"






//Private Defines
#define UART_PTR				&dev_Uart[ATSVR_UART_ID]

#ifndef HTONS
#define HTONS(n)				(u16)((((u16)(n)) << 8) | (((u16)(n)) >> 8))
#endif
#ifndef HTONL
u32 HTONL(u32 n)
{

	invert(&n, 4);
	return n;
}
#endif

#ifndef htons
#define htons					HTONS
#endif
#ifndef ntohs
#define ntohs					HTONS
#endif
#ifndef htonl
#define htonl					HTONL
#endif
#ifndef ntohl
#define ntohl					HTONL
#endif



//-------------------------------------------------------------------------
//Point-to-point
//-------------------------------------------------------------------------
#define PPP_MRU					1500

#define PPP_PROCLEN				2
#define PPP_HDRLEN				4       /* octets for code flag and len */
#define PPP_FCSLEN				2       /* octets for FCS */

#define PPP_GOODFCS				0xF0B8  /* Good final FCS value */
#define PPP_INITFCS				0x3DE3


//Const Variables
static const u8 PPP_ESCAPE = 0x7D;
static const u8 PPP_HEFLAG = 0x7E;
static const u8 PPP_HEADER4[] = {0x7E, 0xFF, 0x7D, 0x23};
static const u8 PPP_HEADER5[] = {0x7E, 0x7D, 0xDF, 0x7D, 0x23};
static const u8 PPP_MAGIC[] = {0x83, 0x23, 0x45, 0x34};
static const u8 PPP_IPADDRESS[] = {0xC0, 0xA8, 0xFE, 0xFE};

//Private Variables
static u8 ppprbuf[PPP_PROCLEN + PPP_MRU + PPP_FCSLEN];
static u8 ppptbuf[PPP_PROCLEN + PPP_MRU + PPP_FCSLEN];


struct pppcp_header
{
	u8	code;
	u8	flag;
	u16	len;
	u8	data[];
};
 
 /* Macros. */
#define PCPT					((struct pppcp_header *)&ppptbuf[PPP_PROCLEN])
#define PCPR					((struct pppcp_header *)&ppprbuf[PPP_PROCLEN])


//-------------------------------------------------------------------------
//IP
//-------------------------------------------------------------------------
#define IPH_LEN					20    /* Size of IP header */

#define IP_PROTO_ICMP			1
#define IP_PROTO_TCP			6
#define IP_PROTO_UDP			17
#define IP_PROTO_ICMP6			58

/* IPv4 header. */
struct ip_header
{
	u8	vhl,
		tos;
	u16 len;
	u16	ipid,
		ipoffset;
	u8	ttl,
		proto;
	u16 chksum;
	u32 srcipaddr,
		destipaddr;
};

/* Macros. */
#define IPT						((struct ip_header *)&ppptbuf[PPP_PROCLEN])
#define IPR						((struct ip_header *)&ppprbuf[PPP_PROCLEN])

//Private Variables
static u16 ip_id = 0x2763;
static struct ip_header ipt_header;


//-------------------------------------------------------------------------
//TCP
//-------------------------------------------------------------------------
#define TCPH_LEN				20    /* Size of TCP header */
#define IPTCPH_LEN				(IPH_LEN + TCPH_LEN)    /* Size of IP + TCP header */

#define TCP_MSS					(PPP_MRU - IPTCPH_LEN)

/* Structures and definitions. */
#define TCP_FIN					0x01
#define TCP_SYN					0x02
#define TCP_RST					0x04
#define TCP_PSH					0x08
#define TCP_ACK					0x10
#define TCP_URG					0x20
#define TCP_CTL					0x3F

#define TCP_OPT_END				0   /* End of TCP options list */
#define TCP_OPT_NOOP			1   /* "No-operation" TCP option */
#define TCP_OPT_MSS				2   /* Maximum segment size TCP option */

//Const Variables
static const u8 TCP_CONST_OPTMSS[] = {0x02, 0x04, 0x05, 0xB4};

//Private Variables
static u16 tcp_ss = TCP_MSS;
static u32 tcp_seq, tcp_ack;
static int tcp_linked = 0;

/* TCP header. */
struct tcp_header
{
	u16 srcport,
		destport;
	u32	seqno,
		ackno;
	u8	tcpoffset,
		flags;
	u16	wnd,
		chksum,
		urgp;
	u8	optdata[4];
};

/* Macros. */
#define TCPT					((struct tcp_header *)&ppptbuf[PPP_PROCLEN + IPH_LEN])
#define TCPR					((struct tcp_header *)&ppprbuf[PPP_PROCLEN + IPH_LEN])

//Private Variables
static struct tcp_header tcpt_header;


//-------------------------------------------------------------------------
//UDP
//-------------------------------------------------------------------------
#define UDPH_LEN				8    /* Size of UDP header */
#define IPUDPH_LEN				(UDPH_LEN + IPH_LEN)    /* Size of IP + UDP header */



/* UDP header. */
struct udp_header
{
	u16 srcport,
		destport;
	u16 len;
	u16 chksum;
};

/* Macros. */
#define UDPT					((struct udp_header *)&ppptbuf[PPP_PROCLEN + IPH_LEN])
#define UDPR					((struct udp_header *)&ppprbuf[PPP_PROCLEN + IPH_LEN])

//Private Variables
static struct udp_header udpt_header;



//Include Files
#include "chksum.c"
#include "ip.c"









static void cp_output(int len)
{

	if (len)
	{
		len += PPP_HDRLEN;
		PCPT->len = htons(len);
	}
	else
	{
		len = ntohs(PCPT->len);
	}

	ppp_output(len + PPP_PROCLEN);
}

static void ipcp_input()
{
	u8 *p, *end, d02[4];
	int i;

	p = PCPR->data;
	end = ((u8 *)PCPR) + ntohs(PCPR->len);

	switch (PCPR->code)
	{
	case 0x01:		//Configure-Request

		for (i = 0; p[1] && (p < end); p += p[1])
		{
			if (p[0] == 0x02)
			{
				memcpy(d02, &p[2], 4);
				i = 1;
				break;
			}
			if (p[0] == 0x03)
			{
				if (memtest(&p[2], 0, 4) == 0)
				{
					i = 2;
					break;
				}
			}
		}

		switch (i)
		{
		case 1:
			//拒绝报头压缩
			PCPT->code = 0x04;				//Configure-Reject
			
			PCPT->data[0] = 0x02;
			PCPT->data[1] = 0x06;
			memcpy(&PCPT->data[2], d02, 4);
			
			cp_output(6);
			break;

		case 2:
			//利用否认应答地址
			PCPT->code = 0x03;				//Configure-Nak

			PCPT->data[0] = 0x03;
			PCPT->data[6] = 0x81;
			PCPT->data[12] = 0x83;
			for (i = 0; i < 3; i++)
			{
				PCPT->data[1 + 6 * i] = 0x06;
				memcpy(&PCPT->data[2 + 6 * i], PPP_IPADDRESS, 4);
			}
			PCPT->data[5] = 0x02;

			cp_output(18);
			break;

 		default:
 			//确认
			PCPT->code = 0x02;				//Configure-Ack
			cp_output(0);

 			//发送本机IP
			PCPT->code = 0x01;				//Configure-Request

			PCPT->data[0] = 0x03;
			PCPT->data[1] = 0x06;
			memcpy(&PCPT->data[2], PPP_IPADDRESS, 4);

			cp_output(6);
			break;
		}
		break;

	default:
		break;
	}
}


static void lcp_input()
{

	//协商
	switch (PCPR->code)
	{
	case 0x01:		//Configure-Request
		//接受
		PCPT->code = 0x02;				//Configure-Ack
		cp_output(0);

		//发送LCP协商
		PCPT->code = 0x01;			//Configure-Request
		PCPT->data[0] = 0x02;
		PCPT->data[1] = 0x06;
		memset(&PCPT->data[2], 0xFF, 4);
		cp_output(6);
		break;

	//中止链路
	case 0x05:		//Terminate-Request
		PCPT->code = 0x06; 			//Terminate-Ack
		cp_output(0);

		AT->ppp = 0;
		break;

	//链路维持
	case 0x09:		//Echo-Request
		PCPT->code = 0x0A; 			//Echo-Reply
		memcpy(PCPT->data, PPP_MAGIC, 4);
		cp_output(0);
		break;

	default:
		break;
	}
}

static void pap_input()
{

	//鉴权
	switch (PCPR->code)
	{
	case 0x01:		//Configure-Request
		//接受
		PCPT->code = 0x02;				//Configure-Ack
		cp_output(0);

		break;

	default:
		break;
	}
}

static void ccp_input()
{

	//压缩协商
	switch (PCPR->code)
	{
	case 0x01:		//Configure-Request
		if (ntohs(PCPR->len) <= 4)
			PCPT->code = 0x02;
		else
			PCPT->code = 0x04;
		
		cp_output(0);
		break;

	default:
		break;
	}
}


static void ppp_input(int proto)
{

	switch (proto)
	{
	case 0xC021:		//LCP
		lcp_input();
		break;

	case 0xC023:		//PAP CHAP
		pap_input();
		break;

	case 0x80FD:		//CCP
		ccp_input();
		break;

	case 0x8021:		//IPCP
		ipcp_input();
		break;

	case 0x0021:		//IP
		ip_input();
		break;

	default:
		break;
	}
}











//PPP转义输出
void ppp_output(int len)
{
	u8 *p, *end, c;
	u16 fcs;
	buf b = {0};

	end = ppptbuf + len;
	fcs = ~(fcs16(PPP_INITFCS, ppptbuf, len));
	end = ptrcpy(end, &fcs, PPP_FCSLEN);

	buf_Push(b, PPP_HEADER4, 4);

	for (p = ppptbuf; p < end; )
	{
		c = *p++;
		if ((c < 0x20) || (c == PPP_ESCAPE) || (c == PPP_HEFLAG))
		{
			buf_Push(b, &PPP_ESCAPE, 1);
			c ^= 0x20;
		}
		buf_Push(b, &c, 1);
	}

	buf_Push(b, PPP_HEADER4, 1);

#if ATSVR_CMUX_ENABLE
	if (AT->cmux)
	{
		atsvr_CmuxSend(UART_PTR, ((AT->dlci + 1) << 2) | 0x03, 0xEF, b->p, b->len);
	}
	else
#endif
	{
		uart_Send(UART_PTR, b->p, b->len);
	}

	buf_Release(b);

#if 1 && ATSVR_DEBUG_ENABLE
	{
		char str[8];

		ATSVR_DBGOUT("[PPP>] ");
		for (p = ppptbuf; p < end; )
		{
			sprintf(str, "%02X ", *p++);
			ATSVR_DBGOUT(str);
		}
		ATSVR_DBGOUT(STRING_0D0A);
	}
#endif
}


void ppp_process(buf b)
{
	u8 *p, *end, c;
	int l, i;

	//接收
	while (b->len > 8)
	{
		if (memcmp(b->p, PPP_HEADER4, 4) == 0)
		{
			i = 4;
		}
		else if (memcmp(b->p, PPP_HEADER5, 5) == 0)
		{
			i = 5;
		}
		else if ((b->p[0] == PPP_HEFLAG) && (b->p[1] != PPP_HEFLAG))
		{
			i = 1;
		}
		else
		{
			buf_Remove(b, 1);
			continue;
		}

		end = b->p + b->len;
		for (p = &b->p[8]; p < end; p++)
		{
			if (*p == PPP_HEFLAG)
				break;
		}
		if (p >= end)
			break;

		//收到帧,进行转义
		end = p - 1;
		p = &b->p[i];
		for (l = 0; (l < sizeof(ppprbuf)) && (p <= end); l++)
		{
			c = *p++;
			if (c == 0x7D)
				ppprbuf[l] = *p++ ^ 0x20;
			else
				ppprbuf[l] = c;
		}

#if ATSVR_PPP_FCS_CHECK
		//CS校验
		i = fcs16(PPP_INITFCS, ppprbuf, l);
		if (i != PPP_GOODFCS)
		{
#if 1 && ATSVR_DEBUG_ENABLE
			{
				char str[32];
			
				sprintf(str, "[PPP<] ErrorFCS=%04X\r", i);
				ATSVR_DBGOUT(str);
			}
#endif
			buf_Remove(b, 1);
			continue;
		}
#endif
		
		buf_Remove(b, end - b->p);
		
		//接收正确
#if 1 && ATSVR_DEBUG_ENABLE
		{
			char str[8];
	
			ATSVR_DBGOUT("[PPP<] ");
			for (i = 0; i < l; i++)
			{
				sprintf(str, "%02X ", ppprbuf[i]);
				ATSVR_DBGOUT(str);
			}
			ATSVR_DBGOUT(STRING_0D0A);
		}
#endif
		//对协议域压缩进行补0
		if (ppprbuf[0] == 0x21)
		{
			memmove(&ppprbuf[1], ppprbuf, l);
			ppprbuf[0] = 0x00;
			l += 1;
		}
		memcpy(ppptbuf, ppprbuf, l);
		ppp_input(ntohs(*(u16 *)ppprbuf));

		AT->tmo = 0;
	}
}

void ppp_send()
{

	//发送
#if ATSVR_TCP_ENABLE
	if (tcp_linked)
	{
		tcp_send(AT->tbuf);
	}
#if ATSVR_UDP_ENABLE
	else
#endif
#endif
#if ATSVR_UDP_ENABLE
	{
		udp_send(AT->tbuf);
	}
#endif
}


