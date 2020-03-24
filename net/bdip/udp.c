











void udp_send(buf b)
{
	int len;

	len = MIN(PPP_MRU - IPUDPH_LEN, b->len);
	if (len)
	{
		memcpy(UDPT, &udpt_header, sizeof(udpt_header));
		UDPT->len = htons(len + UDPH_LEN);
		memcpy((u8 *)UDPT + UDPH_LEN, b->p, len);

		ip_output(len + IPUDPH_LEN);

		buf_Remove(b, len);
	}
}


static void udp_input()
{
	int len;

	udpt_header.destport = UDPR->srcport;
	udpt_header.srcport = UDPR->destport;

	len = ntohs(UDPR->len) - UDPH_LEN;
	if (len)
		atsvr_RxDo((u8 *)UDPR + UDPH_LEN, len);
}

