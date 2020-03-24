#if RTC_ENABLE


//Private Defines
//最大数据域长度
#define GDRCP_DATA_SIZE				4096

//异常标志定义
#define GD5100_CABN_NORMAL				0x00	//确认帧
#define GD5100_CABN_ABNORMAL			0x01	//否定帧

//功能码定义
#define GD5100_CCODE_LOGIN				0x21	//登录
#define GD5100_CCODE_LOGOUT				0x22	//登录退出
#define GD5100_CCODE_KEEPALIVE			0x24	//心跳



//Private Typedef
struct gd5100_header
{
	u8	sc1;			//0x68
	u16	rtua;			//地市区县码
	u16	terid;			//终端地址
	u16	msta : 6,		//主站编号
		fseq : 7,		//帧序号
		iseq : 3;		//帧内序号
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
static sys_res gd5100_RmsgAnalyze(void *args)
{
	gd5100_t *p = (gd5100_t *)args;
	p_dlrcp pRcp = &p->parent;
	u8 *pTemp;
#if LIB_ZIP_ENABLE
	size_t nLen;
	int nDelen;
#endif
	struct gd5100_header *pH;

	chl_RecData(pRcp->chl, pRcp->rbuf, OS_TICK_MS);
	
	for (; ; buf_Remove(pRcp->rbuf, 1))
	{
		for (; ; buf_Remove(pRcp->rbuf, 1))
		{
			//不足报文头长度
			if (pRcp->rbuf->len < sizeof(struct gd5100_header))
				return SYS_R_ERR;
			
#if LIB_ZIP_ENABLE
			if (pRcp->zip)
			{
				pTemp = pRcp->rbuf->p;
				if ((pTemp[0] == 0x88) && (pTemp[4] == 0xFF))
				{
					nLen = (pTemp[2] << 8) | pTemp[3];
					if (pRcp->rbuf->len < (nLen + 5))
						return SYS_R_ERR;
					
					if (pTemp[nLen + 4] == 0x77)
					{
						nDelen = DeData(pTemp, nLen + 5);
						if (nDelen > 0)
						{
							buf_Remove(pRcp->rbuf, nLen + 5);
							buf_Push(pRcp->rbuf, RecvBuf, nDelen);
						}
					}
				}
			}
#endif
			pH = (struct gd5100_header *)pRcp->rbuf->p;
			if ((pH->sc1 == 0x68) && (pH->sc2 == 0x68))
			{
				if (pH->len > GDRCP_DATA_SIZE)
					continue;
				
				//收到报文头
				if (pRcp->rcvtime == 0)
					pRcp->rcvtime = rtc_GetTimet();
				break;
			}
		}
		
		//不足长度
		if (pRcp->rbuf->len < (sizeof(struct gd5100_header) + pH->len + 2))
		{
			if (((u16)rtc_GetTimet() - pRcp->rcvtime) < 10)
				return SYS_R_ERR;
			
			pRcp->rcvtime = 0;
			continue;
		}
		pRcp->rcvtime = 0;
		pTemp = pRcp->rbuf->p + sizeof(struct gd5100_header) + pH->len;
		
		//CS
		if (cs8(pRcp->rbuf->p, sizeof(struct gd5100_header) + pH->len) != *pTemp++)
			continue;
		
		//结束符
		if (*pTemp != 0x16)
			continue;
		
		//接收到报文
		p->rmsg->msta = pH->msta;
		p->rmsg->rtua = pH->rtua;
		p->rmsg->terid = pH->terid;
		p->rmsg->fseq = pH->fseq;
		p->rmsg->iseq = pH->iseq;
		p->rmsg->code = pH->code;
		p->rmsg->abn = pH->abn;
		p->rmsg->dir = pH->dir;
		
		buf_Release(p->rmsg->data);
		
		buf_Push(p->rmsg->data, pRcp->rbuf->p + sizeof(struct gd5100_header), pH->len);
		
		buf_Remove(pRcp->rbuf, sizeof(struct gd5100_header) + pH->len + 2);
		
		return SYS_R_OK;
	}
}


//-------------------------------------------------------------------------
//报文头初始化
//-------------------------------------------------------------------------
static void gd5100_TmsgHeaderInit(gd5100_t *p, struct gd5100_header *pH)
{

	pH->sc1 = 0x68;
	pH->sc2 = 0x68;
	pH->rtua = p->rtua;
	pH->terid = p->terid;
	pH->iseq = 0;
	pH->dir = GDRCP_CDIR_SEND;
}


//-------------------------------------------------------------------------
//登录
//-------------------------------------------------------------------------
static sys_res gd5100_TmsgLinkcheck (void *p, int nCmd)
{
	sys_res res;
	buf b = {0};

	switch (nCmd)
	{
	case DLRCP_LINKCHECK_LOGIN:
		nCmd = GD5100_CCODE_LOGIN;
		buf_Push(b, ((gd5100_t *)p)->pwd, 3);
		break;
	case DLRCP_LINKCHECK_LOGOUT:
		nCmd = GD5100_CCODE_LOGOUT;
		break;
	default:
		nCmd = GD5100_CCODE_KEEPALIVE;
		break;
	}
	if (((gd5100_t *)p)->flag)
		res = gd5100_TmsgSend(p, nCmd, b, DLRCP_TMSG_CASCADE);	// 贵州电压规约链路帧方向反
	else
		res = gd5100_TmsgSend(p, nCmd, b, DLRCP_TMSG_REPORT);
	
	buf_Release(b);
	
	return res;
}






//External Functions
//-------------------------------------------------------------------------
//初始化
//-------------------------------------------------------------------------
void gd5100_Init(gd5100_t *p, int nFlag)
{

	memset(p, 0, sizeof(gd5100_t));
	dlrcp_Init(&p->parent, gd5100_TmsgLinkcheck, gd5100_RmsgAnalyze);
	p->flag = nFlag;
}

//-------------------------------------------------------------------------
//发送报文
//-------------------------------------------------------------------------
sys_res gd5100_TmsgSend(gd5100_t *p, int nCode, buf b, int nType)
{
	struct gd5100_header xH;
	u8 nCS;

	gd5100_TmsgHeaderInit(p, &xH);
	
	switch (nType)
	{
	case DLRCP_TMSG_CASCADE:
		xH.dir = GDRCP_CDIR_RECV;
	case DLRCP_TMSG_REPORT:
		xH.msta = 0;
		xH.fseq = p->parent.pfc++;
		break;
	default:
		xH.msta = p->rmsg->msta;
		xH.fseq = p->rmsg->fseq;
		break;
	}
	xH.code = nCode;
	xH.abn = GD5100_CABN_NORMAL;
	xH.len = b->len;
	nCS = cs8(&xH, sizeof(struct gd5100_header)) + cs8(b->p, b->len);
	
	buf_PushData(b, 0x1600 | nCS, 2);
	
	return dlrcp_TmsgSend(&p->parent, &xH, sizeof(struct gd5100_header), b->p, b->len, nType);
}


//-------------------------------------------------------------------------
//发送异常应答
//-------------------------------------------------------------------------
sys_res gd5100_TmsgError(gd5100_t *p, int nCode, int nErr)
{
	struct gd5100_header xH;
	u8 aBuf[3];

	gd5100_TmsgHeaderInit(p, &xH);
	
	xH.msta = p->rmsg->msta;
	xH.fseq = p->rmsg->fseq;
	xH.code = nCode;
	xH.abn = GD5100_CABN_ABNORMAL;
	xH.len = 1;
	aBuf[0] = nErr;
	aBuf[1] = cs8(&xH, sizeof(struct gd5100_header)) + nErr;
	aBuf[2] = 0x16;
	
	return dlrcp_TmsgSend(&p->parent, &xH, sizeof(struct gd5100_header), aBuf, 3, DLRCP_TMSG_RESPOND);
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res gd5100_Transmit(gd5100_t *p, gd5100_t *pD)
{
	sys_res res;
	struct gd5100_header xH;
	u8 nCS;
	buf b = {0};

	buf_Push(b, p->rmsg->data->p, p->rmsg->data->len);
	
	xH.sc1 = 0x68;
	xH.rtua = p->rmsg->rtua;
	xH.terid = p->rmsg->terid;
	xH.msta = p->rmsg->msta;
	xH.fseq = p->rmsg->fseq;
	xH.iseq = p->rmsg->iseq;
	xH.sc2 = 0x68;
	xH.code = p->rmsg->code;
	xH.abn = p->rmsg->abn;
	xH.dir = p->rmsg->dir;
	xH.len = b->len;
	nCS = cs8(&xH, sizeof(struct gd5100_header)) + cs8(b->p, b->len);
	buf_PushData(b, 0x1600 | nCS, 2);
	
	res = dlrcp_TmsgSend(&pD->parent, &xH, sizeof(struct gd5100_header), b->p, b->len, DLRCP_TMSG_RESPOND);
	
	buf_Release(b);
	
	return res;
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
int gd5100_RecvCheck(gd5100_t *p)
{

	if ((p->rmsg->rtua != 0xFFFF) || (p->rmsg->terid != 0xFFFF))
	{
		if ((p->rtua != p->rmsg->rtua) || (p->terid != p->rmsg->terid))
			return 0;
	}
	
	return 1;
}


//-------------------------------------------------------------------------
//规约处理
//-------------------------------------------------------------------------
sys_res gd5100_Handler(gd5100_t *p)
{

	return dlrcp_Handler(&p->parent);
}









#endif

