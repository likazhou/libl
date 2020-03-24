


static u16 chksum(u16 sum, const void *data, size_t len)
{
	u16 t;
	const u8 *dataptr;
	const u8 *last_byte;

	dataptr = (u8 *)data;
	last_byte = (u8 *)data + len - 1;
  
	while (dataptr < last_byte)	/* At least two more bytes */
	{
		t = (dataptr[0] << 8) + dataptr[1];
		sum += t;
		if (sum < t)
			sum++;		/* carry */
		dataptr += 2;
	}

	if (dataptr == last_byte)
	{
		t = (dataptr[0] << 8);
		sum += t;
		if (sum < t)
			sum++;		/* carry */
	}

	/* Return sum in host byte order. */
	return (u16)sum;
}

static u16 ipchksum()
{
	u16 sum;

	sum = chksum(0, IPT, IPH_LEN);
	return (sum == 0) ? 0xFFFF : htons(sum);
}

#if ATSVR_TCP_ENABLE
static u16 upper_chksum(int proto)
{
	int uplen;
	u16 sum;
  
	uplen = ntohs(IPT->len) - IPH_LEN;

	/* IP protocol and length fields. This addition cannot carry. */
	sum = uplen + proto;
	/* Sum IP source and destination addresses. */
	sum = chksum(sum, &IPT->srcipaddr, 8);

	/* Sum TCPUDP header and data. */
	sum = chksum(sum, (u8 *)IPT + IPH_LEN, uplen);

	return (sum == 0) ? 0xFFFF : htons(sum);
}
#endif


