

static void tcp_output(int flag, const void *opt, const void *data, int len)
{
	int ol;

	memcpy(TCPT, &tcpt_header, sizeof(tcpt_header));

	TCPT->flags = flag;
	TCPT->seqno = htonl(tcp_seq);
	TCPT->ackno = htonl(tcp_ack);
	TCPT->wnd = htons(TCP_MSS);

	if (opt)
	{
		ol = 4;
		TCPT->tcpoffset = (TCPH_LEN + 4) << 2;
		memcpy(TCPT->optdata, opt, 4);
	}
	else
	{
		ol = 0;
		TCPT->tcpoffset = TCPH_LEN << 2;
	}
	if (len)
		memcpy(&TCPT->optdata[ol], data, len);

	ip_output(IPTCPH_LEN + ol + len);
}





static int tcp_send(buf b)
{
	int len;

	len = MIN(tcp_ss, b->len);
	if (len)
	{
		tcp_output(TCP_PSH | TCP_ACK, NULL, b->p, len);
		buf_Remove(b, len);
	}
	
	return len;
}

static void tcp_input()
{
	int len, opt, flag;
	u32 seq, ack;

	flag = TCPR->flags;

	if (flag & TCP_RST)
	{
		tcp_linked = 0;
		return;
	}
	
	memcpy(&tcpt_header, TCPR, sizeof(tcpt_header));
	tcpt_header.destport = TCPR->srcport;
	tcpt_header.srcport = TCPR->destport;

	seq = ntohl(TCPR->seqno);
	ack = ntohl(TCPR->ackno);

	tcp_seq = ack;
	opt = (TCPR->tcpoffset >> 2) - TCPH_LEN;

	switch (flag)
	{
	case TCP_SYN:	//连接		
		//更新tcp_ss
		if ((opt >= 4) && (TCPR->optdata[0] == TCP_OPT_MSS))
			tcp_ss = MIN(ntohs(*(u16 *)&TCPR->optdata[2]), TCP_MSS);

		tcp_ack = seq + 1;
		tcp_seq = 0x1001;
		tcp_output(TCP_SYN | TCP_ACK, TCP_CONST_OPTMSS, NULL, 0);

		tcp_linked = 1;

		break;
	
	case TCP_PSH | TCP_ACK:
		//处理接收数据
		len = ntohs(IPR->len) - (IPTCPH_LEN + opt);

		atsvr_RxDo(&TCPR->optdata[opt], len);
		
		tcp_ack = seq + len;
		tcp_output(TCP_ACK, NULL, NULL, 0);

		break;

	default:
		if (flag != TCP_ACK)
		{
			tcp_ack = seq + 1;
			tcp_output(TCP_ACK, NULL, NULL, 0);
		}

		break;
	}
}

