

//Private Defines
#define NW12_DATA_SIZE				4096

//固定帧头长度
#define NW12_FIXHEADER_SIZE			6

//传送方向定义
#define NW12_DIR_RECV				0	//主站发出
#define NW12_DIR_SEND				1	//终端发出



//Private Typedef
struct nw12_header
{
	u8	sc1;
	u16	len1;
	u16	len2;
	u8	sc2;
	struct nw12_c	c;
	u8	adr[6];
	u8	msa;
	u8	afn;
	struct nw12_seq	seq;
} PACK_STRUCT_STRUCT;

static int nw12_IsPW(int nAfn)
{

	switch (nAfn)
	{
	case 0x04:
	case 0x05:
		return 1;
	default:
		return 0;
	}
}


//Private Macros
#if NW12_DEBUG_ENABLE
static void NW12_DBGTX(const void *pHeader, const void *pBuf, size_t nTxLen)
{
	const u8 *pData, *pEnd;
	char str[DBG_BUF_SIZE];
	size_t nLen;

	nLen = sprintf(str, "%s", "<NWT>");

	pData = (const u8 *)pHeader;
	pEnd = pData + sizeof(struct nw12_header);

	while (pData < pEnd)
	{
		nLen += sprintf(&str[nLen], " %02X", *pData++);
	}

	pData = (const u8 *)pBuf;
	pEnd = pData + nTxLen;

	while ((pData < pEnd) && (nLen < (sizeof(str) - 3)))
	{
		nLen += sprintf(&str[nLen], " %02X", *pData++);
	}

	dbg_trace(str);
}
static void NW12_DBGRX(const void *pBuf, size_t nLen)
{
	const u8 *pData, *pEnd;
	char str[DBG_BUF_SIZE];

	pData = (const u8 *)pBuf;
	pEnd = pData + nLen;

	nLen = sprintf(str, "%s", "<NWR>");
	while ((pData < pEnd) && (nLen < (sizeof(str) - 3)))
	{
		nLen += sprintf(&str[nLen], " %02X", *pData++);
	}

	dbg_trace(str);
}
#else
#define NW12_DBGTX(...)
#define NW12_DBGRX(...)
#endif




//Internal Functions
//-------------------------------------------------------------------------
//接收报文分析
//-------------------------------------------------------------------------
static sys_res nw12_RmsgAnalyze(void *args)
{
	nw12_t *p = (nw12_t *)args;
	p_dlrcp pRcp = &p->parent;
	struct nw12_header *pH;
	u8 *pcs;
	int cslen;
	size_t nLen;

	chl_RecData(pRcp->chl, pRcp->rbuf, OS_TICK_MS);
	
	for (; ; buf_Remove(pRcp->rbuf, 1))
	{
		for (; ; buf_Remove(pRcp->rbuf, 1))
		{
			//不足报文头长度
			if (pRcp->rbuf->len < sizeof(struct nw12_header))
				return SYS_R_ERR;
			
			pH = (struct nw12_header *)pRcp->rbuf->p;
			if ((pH->sc1 == 0x68) && (pH->sc2 == 0x68)) {
				if (pH->len1 > NW12_DATA_SIZE)
					continue;
				if (pH->len1 != pH->len2)
					continue;
				
				//收到报文头
				if (pRcp->rcvtime == 0)
					pRcp->rcvtime = rtc_GetTimet();
				break;
			}
		}

		cslen = NW12_FIXHEADER_SIZE + pH->len1;
		//不足长度
		if (pRcp->rbuf->len < (cslen + 2))
		{
			if (((u16)rtc_GetTimet() - pRcp->rcvtime) < 10)
				return SYS_R_ERR;
			
			pRcp->rcvtime = 0;
			continue;
		}
		
		pRcp->rcvtime = 0;
		pcs = pRcp->rbuf->p + cslen;
		//CS
		if (*pcs != cs8(pRcp->rbuf->p + NW12_FIXHEADER_SIZE, pH->len1))
			continue;
		
		//结束符
		if (pcs[1] != 0x16)
			continue;
		
#if NW12_DEBUG_ENABLE
		NW12_DBGRX(pRcp->rbuf->p, cslen + 2);
#endif
		//接收到报文
		p->msa = pH->msa;
		p->c = pH->c;
		p->afn = pH->afn;
		p->seq = pH->seq;
		memcpy(p->radr, pH->adr, 6);
		
		if (pH->seq.tpv)
		{
			//有时间标志
			pcs -= sizeof(p->tp);
			memcpy(&p->tp, pcs, sizeof(p->tp));
		}
		
		if (nw12_IsPW(pH->afn))
		{
			pcs -= sizeof(p->pw);
			memcpy(&p->pw, pcs, sizeof(p->pw));
		}
		buf_Release(p->data);
		
		nLen = pcs - pRcp->rbuf->p - sizeof(struct nw12_header);
		
		if (nLen > 0)
			buf_Push(p->data, pRcp->rbuf->p + sizeof(struct nw12_header), nLen);
		
		buf_Remove(pRcp->rbuf, cslen + 2);
		
		return SYS_R_OK;
	}
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
static void nw12_TmsgHeaderInit(nw12_t *p, struct nw12_header *pH)
{

	memset(pH, 0, sizeof(struct nw12_header));
	pH->sc1 = 0x68;
	pH->sc2 = 0x68;
	memcpy(pH->adr, p->adr, 6);
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
static sys_res nw12_TmsgLinkcheck(void *p, int nCmd)
{
	buf b = {0};

	buf_PushData(b, nw12_ConvertDa2DA(0), 2);
	
	switch (nCmd)
	{
	case DLRCP_LINKCHECK_LOGIN:
		buf_PushData(b, 0xE0001000, 4);
		buf_PushData(b, 0x0100, 2);
		break;
	case DLRCP_LINKCHECK_LOGOUT:
		buf_PushData(b, 0xE0001002, 4);
		break;
	default:
		buf_PushData(b, 0xE0001001, 4);
		break;
	}
	
	nw12_TmsgSend(p, NW12_FUN_LINKCHECK, NW12_AFN_LINKCHECK, b, DLRCP_TMSG_REPORT);
	
	buf_Release(b);
	
	return SYS_R_OK;
}




//External Functions
//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void nw12_Init(nw12_t *p)
{

	memset(p, 0, sizeof(nw12_t));
	dlrcp_Init(&p->parent, nw12_TmsgLinkcheck, nw12_RmsgAnalyze);
}


//-------------------------------------------------------------------------
//发送报文
//-------------------------------------------------------------------------
sys_res nw12_TmsgSend(nw12_t *p, int nFun, int nAfn, buf b, int nType)
{
	struct nw12_header xH;
	u8 nCS;

	nw12_TmsgHeaderInit(p, &xH);
	
	if (nType == DLRCP_TMSG_REPORT)
	{
		xH.c.prm = 1;
		xH.seq.seq = p->parent.pfc++;
		xH.seq.con = 1;
	}
	else
	{
		xH.msa = p->msa;
		xH.seq.seq = p->seq.seq;
	}
	
	xH.c.dir = NW12_DIR_SEND;
	xH.c.fun = nFun;
	xH.seq.fin = 1;
	xH.seq.fir = 1;
	xH.afn = nAfn;
	if (nType == DLRCP_TMSG_RESPOND)
	{
		if (p->seq.tpv)
		{
			xH.seq.tpv = 1;
			buf_Push(b, &p->tp, sizeof(p->tp));
		}
	}
	
	xH.len1 = xH.len2 = b->len + (sizeof(struct nw12_header) - NW12_FIXHEADER_SIZE);
	nCS =	cs8((u8 *)&xH + NW12_FIXHEADER_SIZE, (sizeof(struct nw12_header) - NW12_FIXHEADER_SIZE)) +
			cs8(b->p, b->len);
	
	buf_PushData(b, 0x1600 | nCS, 2);
	
#if NW12_DEBUG_ENABLE
	NW12_DBGTX(&xH, b->p, b->len);
#endif
	return dlrcp_TmsgSend(&p->parent, &xH, sizeof(struct nw12_header), b->p, b->len, nType);
}




//-------------------------------------------------------------------------
//数据转换
//-------------------------------------------------------------------------
u16 nw12_ConvertDa2DA(int nDa)
{

	if (nDa)
	{
		nDa -= 1;
		return (((nDa >> 3) + 1) << 8) | BITMASK(nDa & 7);
	}
	
	return 0;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
int nw12_ConvertDA2Map(u16 nDA, void *pData)
{
	u16 *p = (u16 *)pData;
	int j, nQty = 0, nDa;

	nDa = nDA >> 8;
	if (nDa == 0)
	{
		*p = 0;
		return 1;
	}
	nDa -= 1;
	for (j = 0; j < 8; j++)
	{
		if (nDA & BITMASK(j))
			p[nQty++] = nDa * 8 + j + 1;
	}
	
	return nQty;
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res nw12_Transmit(nw12_t *p, nw12_t *pD)
{
	sys_res res;
	struct nw12_header xH;
	u8 nCS;
	buf b = {0};

	xH.sc1 = 0x68;
	xH.len1 = xH.len2 = p->data->len + (sizeof(struct nw12_header) - NW12_FIXHEADER_SIZE);
	xH.sc2 = 0x68;
	xH.c = p->c;
	memcpy(xH.adr, p->radr, 6);
	xH.msa = p->msa;
	xH.afn = p->afn;
	xH.seq = p->seq;
	buf_Push( b,p->data->p, p->data->len);
	
	nCS =	cs8((u8 *)&xH + NW12_FIXHEADER_SIZE, (sizeof(struct nw12_header) - NW12_FIXHEADER_SIZE)) +
			cs8(b->p, b->len);
	buf_PushData(b, 0x1600 | nCS, 2);
	
	res = dlrcp_TmsgSend(&pD->parent, &xH, sizeof(struct nw12_header), b->p, b->len, DLRCP_TMSG_RESPOND);
	
	buf_Release(b);
	return res;
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
int nw12_RecvCheck(nw12_t *p)
{

	if (memtest(p->radr, 0xFF, 6))
	{
		if (memcmp(p->adr, p->radr, 6))
			return 0;
	}
	
	return 1;
}


//-------------------------------------------------------------------------
//规约处理
//-------------------------------------------------------------------------
sys_res nw12_Handler(nw12_t *p)
{

	return dlrcp_Handler(&p->parent);
}



