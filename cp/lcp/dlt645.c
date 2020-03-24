


//Private Defines



//Private Typedef
struct dlt645_header
{
	u8	sc1;		//0x68
	u8	adr[6];		//电表地址
	u8	sc2;		//0x68
	u8	code : 5,	//控制码
		con : 1,	//后续帧标志
		abn : 1,	//异常标志
		dir : 1;	//传送方向
	u8	len;		//数据长度
} PACK_STRUCT_STRUCT;



//Internal Functions
#if DLT645_DEBUG_ENABLE
static void dlt645_DbgOut(int nType, const void *pBuf, size_t nLen)
{
	const u8 *pData, *pEnd;
	char str[DBG_BUF_SIZE];

	pData = (const u8 *)pBuf;
	pEnd = pData + nLen;

	if (nType)
		nLen = sprintf(str, "%s", "<645T>");
	else
		nLen = sprintf(str, "%s", "<645R>");

	while ((pData < pEnd) && (nLen < (sizeof(str) - 3)))
	{
		nLen += sprintf(&str[nLen], " %02X", *pData++);
	}

	dbg_trace(str);
}
#else
#define dlt645_DbgOut(...)
#endif




//External Functions
void dlt645_Packet2Buf(buf b, const void *pAdr, int nC, const void *pData, size_t nLen)
{
	int i;
	const u8 *pBuf = (const u8 *)pData;

	buf_PushData(b, 0x68, 1);
	buf_Push(b, pAdr, 6);
	buf_PushData(b, 0x68, 1);
	buf_PushData(b, nC, 1);
	buf_PushData(b, nLen, 1);

	//数据0x33处理
	for (i = 0; i < nLen; i++)
	{
		buf_PushData(b, *pBuf++ + 0x33, 1);
	}
	buf_PushData(b, 0x1600 | cs8(b->p, b->len), 2);
}

u8 *dlt645_PacketAnalyze(const u8 *p, size_t len)
{
	const u8 *tmp;
	size_t cslen;
	struct dlt645_header *pH;

	for (; ; p++, len--)
	{
		pH = (struct dlt645_header *)p;
		//不足报文头长度
		if (len < sizeof(struct dlt645_header))
			return NULL;
		//校验
		if ((pH->sc1 != 0x68) || (pH->sc2 != 0x68))
			continue;
		cslen = sizeof(struct dlt645_header) + pH->len;
		//不足长度
		if (len < (cslen + 2))
			continue;
		tmp = p + cslen;
		//CS
		if (cs8(p, cslen) != *tmp++)
			continue;
		//结束符
		if (*tmp != 0x16)
			continue;
		break;
	}
	return (u8 *)p;
}

static const u8 dlt645_aFE[] = {0xFE, 0xFE, 0xFE, 0xFE};
sys_res dlt645_Meter(chl c, buf b, size_t nTmo)
{
	u8 *pH, aAdr[6];
#if DLT645_DIR_CTRL
	uart_t *pUart;
#endif

#if DLT645_DIR_CTRL
	gpio_Set(2, 0);
	//稳定总线
	chl_Send(c, dlt645_aFE, 4);
	chl_Send(c, b->p, b->len);
	pUart = (uart_t *)(c->pIf);
	if (pUart->para.baud < 2400)
		sys_Delay(200000);
	else
		sys_Delay(100000);
	gpio_Set(2, 1);
#else
	chl_Send(c, dlt645_aFE, 4);
	chl_Send(c, b->p, b->len);
#endif

	dlt645_DbgOut(1, b->p, b->len);

	memcpy(aAdr, &b->p[1], 6);
	buf_Release(b);
	for (nTmo /= OS_TICK_MS; nTmo; nTmo--)
	{
		if (chl_RecData(c, b, OS_TICK_MS) != SYS_R_OK)
			continue;
		pH = dlt645_PacketAnalyze(b->p, b->len);
		if (pH == NULL)
			continue;
		buf_Remove(b, pH - b->p);

		dlt645_DbgOut(0, b->p, b->p[9] + (DLT645_HEADER_SIZE + 2));

		if (memcmp(&b->p[1], aAdr, 6))
		{
			buf_Remove(b, DLT645_HEADER_SIZE);
			continue;
		}
		buf_Remove(b, DLT645_HEADER_SIZE - 2);
		if (b->p[0] & BITMASK(7))
		{
			byteadd(&b->p[2], -0x33, b->p[1]);
			return SYS_R_OK;
		}
	}
	return SYS_R_TMO;
}

// 中继转发645帧20130906
// b - 输入发送645帧与输出接收的645帧
sys_res dlt645_Transmit(chl c, buf b, size_t nTmo)
{
	u8 *pH;

	chl_Send(c, dlt645_aFE, 4);
	chl_Send(c, b->p, b->len);

	dlt645_DbgOut(1, b->p, b->len);

	buf_Release(b);
	for (nTmo /= OS_TICK_MS; nTmo; nTmo--)
	{
		if (chl_RecData(c, b, OS_TICK_MS) != SYS_R_OK)
			continue;
		
		pH = dlt645_PacketAnalyze(b->p, b->len);
		if (pH == NULL)
			continue;
		
		buf_Remove(b, pH - b->p);
		return SYS_R_OK;
	}
	return SYS_R_ERR;
}


