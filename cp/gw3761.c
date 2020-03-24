

//Private Defines
#define GW3761_DATA_SIZE            4096

//固定帧头长度
#define GW3761_FIXHEADER_SIZE       6




//Private Typedef
struct gw3761_header
{
	u8	sc1;
	u16	prtc1 : 2,
		len1 : 14;
	u16	prtc2 : 2,
		len2 : 14;
	u8	sc2;
	struct gw3761_c		c;
	u16	a1;
	u16	a2;
	u8	group : 1,
		msa : 7;
	u8	afn;
	struct gw3761_seq	seq;
} PACK_STRUCT_STRUCT;


static int gw3761_IsPW(int nAfn)
{

    switch (nAfn)
	{
    case 0x01:
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x10:
        return 1;
    default:
        return 0;
    }
}

static int gw3761_IsEC(int nAfn)
{

	switch (nAfn)
	{
	case GW3761_AFN_CONFIRM:
	case 0x08:
	case 0x09:
	case 0x0A:
	case 0x0B:
	case 0x0C:
	case 0x0D:
	case 0x0E:
	case 0x10:
		return 1;
	default:
		return 0;
	}
}









//Internal Functions
//-------------------------------------------------------------------------
//接收报文分析
//-------------------------------------------------------------------------
static sys_res gw3761_RmsgAnalyze(void *args)
{
	gw3761_t *p = (gw3761_t *)args;
	p_dlrcp pRcp = &p->parent;
	struct gw3761_header *pH;
	u8 *pTemp;
	size_t nLen;

	chl_RecData(pRcp->chl, pRcp->rbuf, OS_TICK_MS);
	
	for (; ; buf_Remove(pRcp->rbuf, 1))
	{
		for (; ; buf_Remove(pRcp->rbuf, 1))
		{
			//不足报文头长度
			if (pRcp->rbuf->len < sizeof(struct gw3761_header))
				return SYS_R_ERR;
			
			pH = (struct gw3761_header *)pRcp->rbuf->p;
			if ((pH->sc1 == 0x68) && (pH->sc2 == 0x68))
			{
#if GW3761_IDCHECK_ENABLE
				if (pH->prtc1 != GW3761_PROTOCOL_ID)
					continue;
				if (pH->prtc2 != GW3761_PROTOCOL_ID)
					continue;
#endif
				if (pH->len1 > GW3761_DATA_SIZE)
					continue;
				if (pH->len1 != pH->len2)
					continue;
				
				//收到报文头
				if (pRcp->rcvtime == 0)
					pRcp->rcvtime = rtc_GetTimet();
				
				break;
			}
		}
		
		//不足长度
		if (pRcp->rbuf->len < (GW3761_FIXHEADER_SIZE + 2 + pH->len1))
		{
			if (((u16)rtc_GetTimet() - pRcp->rcvtime) < 10)
				return SYS_R_ERR;
			
			pRcp->rcvtime = 0;
			continue;
		}
		pRcp->rcvtime = 0;
		pTemp = pRcp->rbuf->p + GW3761_FIXHEADER_SIZE + pH->len1;
		
		//CS
		if (*pTemp != cs8(pRcp->rbuf->p + GW3761_FIXHEADER_SIZE, pH->len1))
			continue;
		
		//结束符
		if (pTemp[1] != 0x16)
			continue;
		
		//接收到报文
		p->rmsg.c = pH->c;
		p->rmsg.a1 = pH->a1;
		p->rmsg.a2 = pH->a2;
		p->rmsg.group = pH->group;
		p->rmsg.msa = pH->msa;
		p->rmsg.afn = pH->afn;
		p->rmsg.seq = pH->seq;
		
		if (pH->seq.tpv)
		{
			//有时间标志
			pTemp -= sizeof(p->rmsg.tp);
			memcpy(&p->rmsg.tp, pTemp, sizeof(p->rmsg.tp));
	    }
		if (gw3761_IsPW(pH->afn))
		{
			pTemp -= sizeof(p->rmsg.pw);
			memcpy(&p->rmsg.pw, pTemp, sizeof(p->rmsg.pw));
		}
		
		buf_Release(p->rmsg.data);
		
		nLen = pTemp - pRcp->rbuf->p - sizeof(struct gw3761_header);
		if (nLen > 0)
			buf_Push(p->rmsg.data, pRcp->rbuf->p + sizeof(struct gw3761_header), nLen);
		
		buf_Remove(pRcp->rbuf, pH->len1 + GW3761_FIXHEADER_SIZE + 2);
		
		return SYS_R_OK;
	}
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
static void gw3761_TmsgHeaderInit(gw3761_t *p, struct gw3761_header *pH)
{

	memset(pH, 0, sizeof(struct gw3761_header));
	pH->sc1 = 0x68;
	pH->sc2 = 0x68;
	pH->prtc1 = GW3761_PROTOCOL_ID;
	pH->prtc2 = GW3761_PROTOCOL_ID;
	pH->a1 = p->rtua;
	pH->a2 = p->terid;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
static void gw3761_TmsgAfn00(gw3761_t *p, u32 nDu, int nFun)
{
	buf b = {0};

	buf_PushData(b, nDu, 4);
	gw3761_TmsgSend(p, nFun, GW3761_AFN_CONFIRM, b, DLRCP_TMSG_RESPOND);
	buf_Release(b);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
static sys_res gw3761_TmsgLinkcheck(void *p, int nCmd)
{
	buf b = {0};

	switch (nCmd)
	{
	case DLRCP_LINKCHECK_LOGIN:
		nCmd = 1;
		break;
	case DLRCP_LINKCHECK_LOGOUT:
		nCmd = 2;
		break;
	default:
		nCmd = 3;
		break;
	}
	buf_PushData(b, gw3761_ConvertDa2DA(0), 2);
	buf_PushData(b, gw3761_ConvertFn2DT(nCmd), 2);
	
	gw3761_TmsgSend(p, GW3761_FUN_LINKCHECK, GW3761_AFN_LINKCHECK, b, DLRCP_TMSG_REPORT);
	
	buf_Release(b);
	
#if GW3761_ECREPORT_ENABLE
	buf_PushData(b, gw3761_ConvertDa2DA(0), 2);
	buf_PushData(b, gw3761_ConvertFn2DT(7), 2);
	buf_PushData(b, gw3761_EvtCount(), GW3761_EC_SIZE);
	
	gw3761_TmsgSend(p, GW3761_FUN_RESPONSE, 0x0C, b, DLRCP_TMSG_REPORT);
	
	buf_Release(b);
#endif

	return SYS_R_OK;
}




//External Functions
//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void gw3761_Init(gw3761_t *p)
{

	memset(p, 0, sizeof(gw3761_t));
	dlrcp_Init(&p->parent, gw3761_TmsgLinkcheck, gw3761_RmsgAnalyze);
}

//-------------------------------------------------------------------------
//发送报文
//-------------------------------------------------------------------------
sys_res gw3761_TmsgSend(gw3761_t *p, int nFun, int nAfn, buf b, int nType)
{
	struct gw3761_header xH;
	u8 nCS;

	gw3761_TmsgHeaderInit(p, &xH);
	
	if (nType == DLRCP_TMSG_REPORT)
	{
		xH.c.prm = 1;
		xH.seq.seq = p->parent.pfc++;
		xH.seq.con = 1;
	}
	else
	{
		xH.msa = p->rmsg.msa;
		xH.seq.seq = p->rmsg.seq.seq;
	}
	
	xH.c.dir = GW3761_DIR_SEND;
	xH.c.fun = nFun;
	xH.seq.fin = 1;
	xH.seq.fir = 1;
	xH.afn = nAfn;
	
	if (gw3761_IsEC(nAfn))
	{
		xH.c.fcb_acd = 1;
		buf_PushData(b, gw3761_EvtCount(), GW3761_EC_SIZE);
	}
	
	if (nType == DLRCP_TMSG_RESPOND)
	{
		if (p->rmsg.seq.tpv)
		{
			xH.seq.tpv = 1;
			buf_Push(b, &p->rmsg.tp, sizeof(p->rmsg.tp));
		}
	}
	
	xH.len1 = xH.len2 = b->len + (sizeof(struct gw3761_header) - GW3761_FIXHEADER_SIZE);
	nCS =	cs8((u8 *)&xH + GW3761_FIXHEADER_SIZE, (sizeof(struct gw3761_header) - GW3761_FIXHEADER_SIZE)) +
			cs8(b->p, b->len);
	
	buf_PushData(b, 0x1600 | nCS, 2);
	
	return dlrcp_TmsgSend(&p->parent, &xH, sizeof(struct gw3761_header), b->p, b->len, nType);
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res gw3761_TmsgConfirm(gw3761_t *p)
{

	gw3761_TmsgAfn00(p, 0x00010000, GW3761_FUN_CONFIRM);
	
	return SYS_R_OK;
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res gw3761_TmsgReject(gw3761_t *p)
{

	gw3761_TmsgAfn00(p, 0x00020000, GW3761_FUN_NODATA);
	
	return SYS_R_OK;
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res gw3761_Transmit(gw3761_t *p, gw3761_t *pD)
{
	sys_res res;
	struct gw3761_header xH;
	u8 nCS;
	buf b = {0};

	buf_Push(b, p->rmsg.data->p, p->rmsg.data->len);
	
	xH.sc1 = 0x68;
	xH.sc2 = 0x68;
	xH.prtc1 = GW3761_PROTOCOL_ID;
	xH.prtc2 = GW3761_PROTOCOL_ID;
	xH.c = p->rmsg.c;
	xH.a1 = p->rmsg.a1;
	xH.a2 = p->rmsg.a2;
	xH.group = p->rmsg.group;
	xH.msa = p->rmsg.msa;
	xH.afn = p->rmsg.afn;
	xH.seq = p->rmsg.seq;
	
	//有时间标志
	if (xH.seq.tpv)
		buf_Push(b, &p->rmsg.tp, sizeof(p->rmsg.tp));
	
	//有密码
	if (gw3761_IsPW(xH.afn))
		buf_Push(b, &p->rmsg.pw, sizeof(p->rmsg.pw));
	
	xH.len1 = xH.len2 = b->len + (sizeof(struct gw3761_header) - GW3761_FIXHEADER_SIZE);
	
	nCS =	cs8((u8 *)&xH + GW3761_FIXHEADER_SIZE, (sizeof(struct gw3761_header) - GW3761_FIXHEADER_SIZE)) +
			cs8(b->p, b->len);
	buf_PushData(b, 0x1600 | nCS, 2);
	
	res = dlrcp_TmsgSend(&pD->parent, &xH, sizeof(struct gw3761_header), b->p, b->len, DLRCP_TMSG_RESPOND);
	
	buf_Release(b);
	
	return res;
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
int gw3761_RecvCheck(gw3761_t *p)
{

	if ((p->rmsg.a1 != 0xFFFF) || (p->rmsg.a2 != 0xFFFF))
	{
		if ((p->rtua != p->rmsg.a1) || (p->terid != p->rmsg.a2))
			return 0;
	}
	
	return 1;
}


//-------------------------------------------------------------------------
//规约处理
//-------------------------------------------------------------------------
sys_res gw3761_Handler(gw3761_t *p)
{

	return dlrcp_Handler(&p->parent);
}








