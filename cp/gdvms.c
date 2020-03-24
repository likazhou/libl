#if RTC_ENABLE


//Private Defines
//最大数据域长度
#define GDVMS_DATA_SIZE					4096

//异常标志定义
#define GDVMS_CABN_NORMAL				0x00	//确认帧
#define GDVMS_CABN_ABNORMAL				0x01	//否定帧

//功能码定义
#define GDVMS_CCODE_LOGIN				0x21	//登录
#define GDVMS_CCODE_LOGOUT				0x22	//登录退出
#define GDVMS_CCODE_KEEPALIVE			0x24	//心跳



//Private Typedef
struct gdvms_header
{
	u8	sc1;			//0x68
	u8	adr[GDVMS_ADR_SIZE];
	u8	fseq;			//帧序号
	u8	iseq;			//帧内序号
	u8	sc2;			//0x68
	u8	code : 6,		//控制码
		abn : 1,		//异常标志
		dir : 1;		//传送方向
	u16	len;			//数据长度
} PACK_STRUCT_STRUCT;



//Internal Functions
//-------------------------------------------------------------------------
//分析报文
//-------------------------------------------------------------------------
static sys_res gdvms_RmsgAnalyze(void *args)
{
	gdvms_t *p = (gdvms_t *)args;
	p_dlrcp pRcp = &p->parent;
	u8 *pTemp;
	struct gdvms_header *pH;

	chl_RecData(pRcp->chl, pRcp->rbuf, OS_TICK_MS);
	
	for (; ; buf_Remove(pRcp->rbuf, 1))
	{
		for (; ; buf_Remove(pRcp->rbuf, 1))
		{
			//不足报文头长度
			if (pRcp->rbuf->len < sizeof(struct gdvms_header))
				return SYS_R_ERR;
			
			pH = (struct gdvms_header *)pRcp->rbuf->p;
			if ((pH->sc1 == 0x68) && (pH->sc2 == 0x68))
			{
				if (pH->len > GDVMS_DATA_SIZE)
					continue;
				
				//收到报文头
				if (pRcp->rcvtime == 0)
					pRcp->rcvtime = rtc_GetTimet();
				
				break;
			}
		}
		
		//不足长度
		if (pRcp->rbuf->len < (sizeof(struct gdvms_header) + pH->len + 2))
		{
			if (((u16)rtc_GetTimet() - pRcp->rcvtime) < 10)
				return SYS_R_ERR;
			
			pRcp->rcvtime = 0;
			continue;
		}
		pRcp->rcvtime = 0;
		pTemp = pRcp->rbuf->p + sizeof(struct gdvms_header) + pH->len;
		
		//CS
		if (cs8(pRcp->rbuf->p, sizeof(struct gdvms_header) + pH->len) != *pTemp++)
			continue;
		
		//结束符
		if (*pTemp != 0x16)
			continue;
		
		if (pH->dir)
			continue;
		
		//地址匹配(全0通配)
		if (memtest(pH->adr, 0, GDVMS_ADR_SIZE))
		{
			if (memcmp(p->adr, pH->adr, GDVMS_ADR_SIZE))
			{
				buf_Remove(pRcp->rbuf, sizeof(struct gdvms_header) + pH->len + 2);
				continue;
			}
		}
		
		//接收到报文
		p->fseq = pH->fseq;
		p->code = pH->code;
		p->abn = pH->abn;
		p->dir = pH->dir;
		buf_Release(p->data);
		buf_Push(p->data, pRcp->rbuf->p + sizeof(struct gdvms_header), pH->len);
		buf_Remove(pRcp->rbuf, sizeof(struct gdvms_header) + pH->len + 2);
		
		return SYS_R_OK;
	}
}


//-------------------------------------------------------------------------
//报文头初始化
//-------------------------------------------------------------------------
static void gdvms_TmsgHeaderInit(gdvms_t *p, struct gdvms_header *pH)
{

	pH->sc1 = 0x68;
	pH->sc2 = 0x68;
	
	memcpy(pH->adr, p->adr, GDVMS_ADR_SIZE);
	
	pH->iseq = 0;
	pH->dir = GDRCP_CDIR_SEND;
}


//-------------------------------------------------------------------------
//登录
//-------------------------------------------------------------------------
static sys_res gdvms_TmsgLinkcheck (void *p, int nCmd)
{
	sys_res res;
	buf b = {0};

	switch (nCmd)
	{
	case DLRCP_LINKCHECK_LOGIN:
		nCmd = GDVMS_CCODE_LOGIN;
		break;
	case DLRCP_LINKCHECK_LOGOUT:
		nCmd = GDVMS_CCODE_LOGOUT;
		break;
	default:
		nCmd = GDVMS_CCODE_KEEPALIVE;
		break;
	}
	res = gdvms_TmsgSend(p, nCmd, b, DLRCP_TMSG_REPORT);
	
	buf_Release(b);
	
	return res;
}







//External Functions
//-------------------------------------------------------------------------
//初始化
//-------------------------------------------------------------------------
void gdvms_Init(gdvms_t *p)
{

	memset(p, 0, sizeof(gdvms_t));
	dlrcp_Init(&p->parent, gdvms_TmsgLinkcheck, gdvms_RmsgAnalyze);
}

//-------------------------------------------------------------------------
//发送报文
//-------------------------------------------------------------------------
sys_res gdvms_TmsgSend(gdvms_t *p, int nCode, buf b, int nType)
{
	struct gdvms_header xH;
	u8 nCS;

	gdvms_TmsgHeaderInit(p, &xH);
	
	if (nType == DLRCP_TMSG_REPORT)
	{
		if (p->parent.pfc == 0)
			p->parent.pfc = 1;
		else
			p->parent.pfc += 1;
		
		xH.fseq = p->parent.pfc;
	}
	else
	{
		xH.fseq = p->fseq;
	}
	
	xH.code = nCode;
	xH.abn = GDVMS_CABN_NORMAL;
	xH.len = b->len;
	nCS = cs8(&xH, sizeof(struct gdvms_header)) + cs8(b->p, b->len);
	
	buf_PushData(b, 0x1600 | nCS, 2);
	
	return dlrcp_TmsgSend(&p->parent, &xH, sizeof(struct gdvms_header), b->p, b->len, nType);
}


//-------------------------------------------------------------------------
//发送异常应答
//-------------------------------------------------------------------------
sys_res gdvms_TmsgError(gdvms_t *p, int nCode, int nErr)
{
	struct gdvms_header xH;
	u8 aBuf[3];

	gdvms_TmsgHeaderInit(p, &xH);
	
	xH.fseq = p->fseq;
	xH.code = nCode;
	xH.abn = GDVMS_CABN_ABNORMAL;
	xH.len = 1;
	aBuf[0] = nErr;
	aBuf[1] = cs8(&xH, sizeof(struct gdvms_header)) + nErr;
	aBuf[2] = 0x16;
	
	return dlrcp_TmsgSend(&p->parent, &xH, sizeof(struct gdvms_header), aBuf, 3, DLRCP_TMSG_RESPOND);
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res gdvms_Transmit(gdvms_t *p, gdvms_t *pD)
{
	sys_res res;
	struct gdvms_header xH;
	u8 nCS;
	buf b = {0};

	buf_Push(b, p->data->p, p->data->len);
	xH.sc1 = 0x68;
	
	memcpy(xH.adr, p->adr, GDVMS_ADR_SIZE);
	
	xH.fseq = p->fseq;
	xH.iseq = 0;
	xH.sc2 = 0x68;
	xH.code = p->code;
	xH.abn = p->abn;
	xH.dir = p->dir;
	xH.len = b->len;
	nCS = cs8(&xH, sizeof(struct gdvms_header)) + cs8(b->p, b->len);
	
	buf_PushData(b, 0x1600 | nCS, 2);
	
	res = dlrcp_TmsgSend(&pD->parent, &xH, sizeof(struct gdvms_header), b->p, b->len, DLRCP_TMSG_RESPOND);
	
	buf_Release(b);
	
	return res;
}




//-------------------------------------------------------------------------
//规约处理
//-------------------------------------------------------------------------
sys_res gdvms_Handler(gdvms_t *p)
{

	return dlrcp_Handler(&p->parent);
}









#endif


