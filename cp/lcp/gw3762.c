


//Private Defines
#define GW3762_HEADER_L_SIZE		2


//GW376.2 Code Defines
#define GW3762_CODE_T_PLC_FOCUS		1
#define GW3762_CODE_T_PLC_SCATT		2
#define GW3762_CODE_T_FM			10

#define GW3762_CODE_P_PUSH			1
#define GW3762_CODE_P_ACK			0

#define GW3762_CODE_D_2MODULE		0
#define GW3762_CODE_D_2TERMINAL		1

//GW376.2 Info Zone Defines
#define GW3762_RZONE_M_2MODULE		0
#define GW3762_RZONE_M_2METER		1

#define GW3762_RZONE_R_ROUTE		0
#define GW3762_RZONE_R_TRANS		1





//Private Typedefs
struct gw3762_header
{
	u8	sc1;
	u16	len;
	struct gw3762_c	c;
} PACK_STRUCT_STRUCT;



//Private Macros


//Internal Functions
#if GW3762_DEBUG_ENABLE
#if DEBUG_LOG_ENABLE
static void GW3762_DBGOUT(int nType, const void *pBuf, size_t nLen)
{

	if (nType)
		nType = LOG_T_GW3762T;
	else
		nType = LOG_T_GW3762R;
	
	log_Write(nType, pBuf, nLen);
}
#else
static void GW3762_DBGOUT(int nType, const void *pBuf, size_t nLen)
{
	const u8 *pData, *pEnd;
	char str[DBG_BUF_SIZE];

	pData = (const u8 *)pBuf;
	pEnd = pData + nLen;

	if (nType)
		nLen = sprintf(str, "%s", "<376.2T>");
	else
		nLen = sprintf(str, "%s", "<376.2R>");
	while ((pData < pEnd) && (nLen < (sizeof(str) - 3)))
	{
		nLen += sprintf(&str[nLen], " %02X", *pData++);
	}

	dbg_trace(str);
}
#endif
#else
#define GW3762_DBGOUT(...)
#endif

static sys_res gw3762_Transmit2ES(plc_t *p, int nAfn, u16 nDT, const void *pData, size_t nLen)
{
	buf bTx = {0};
	struct gw3762_rdown xR = {0};
	
	buf_PushData(bTx, 0x47000068, 4);
	buf_Push(bTx, &xR, sizeof(xR));

	buf_PushData(bTx, nAfn, 1);
	buf_PushData(bTx, nDT, 2);
	buf_Push(bTx, pData, nLen);
	buf_PushData(bTx, cs8(&bTx->p[1 + GW3762_HEADER_L_SIZE], bTx->len - (1 + GW3762_HEADER_L_SIZE)) | 0x1600, 2);
	memcpy(&bTx->p[1], (const void *)&bTx->len, GW3762_HEADER_L_SIZE);

	GW3762_DBGOUT(1, bTx->p, bTx->len);

	chl_Send(p->chl, bTx->p, bTx->len);
	buf_Release(bTx);

	return SYS_R_OK;
}


static sys_res gw3762_Transmit2Module(plc_t *p, int nAfn, u16 nDT, const void *pData, size_t nLen)
{
	buf bTx = {0};
	struct gw3762_rdown xR = {0};
	
//	xR.route = GW3762_RZONE_R_TRANS;

	buf_PushData(bTx, 0x41000068, 4);
	buf_Push(bTx, &xR, sizeof(xR));

	buf_PushData(bTx, nAfn, 1);
	buf_PushData(bTx, nDT, 2);
	buf_Push(bTx, pData, nLen);
	buf_PushData(bTx, cs8(&bTx->p[1 + GW3762_HEADER_L_SIZE], bTx->len - (1 + GW3762_HEADER_L_SIZE)) | 0x1600, 2);
	memcpy(&bTx->p[1], (const void *)&bTx->len, GW3762_HEADER_L_SIZE);

	GW3762_DBGOUT(1, bTx->p, bTx->len);

	chl_Send(p->chl, bTx->p, bTx->len);
	buf_Release(bTx);

	return SYS_R_OK;
}


static sys_res gw3762_Transmit2Meter(plc_t *p, int nRoute, int nAfn, u16 nDT, const void *pAdr, 
													int nRelay, const void *pRtAdr, const void *pData, size_t nLen)
{
	buf bTx = {0};
	struct gw3762_rdown xR = {0};

	if (nRoute == 0)
		xR.route = GW3762_RZONE_R_TRANS;
	xR.module = GW3762_RZONE_M_2METER;
	xR.relay = nRelay;

	buf_PushData(bTx, 0x41000068, 4);
	buf_Push(bTx, &xR, sizeof(xR));
	buf_Push(bTx, p->adr, sizeof(p->adr));
	if (nRelay)
		buf_Push(bTx, pRtAdr, nRelay * 6);
	buf_Push(bTx, pAdr, 6);

	buf_PushData(bTx, nAfn, 1);
	buf_PushData(bTx, nDT, 2);
	if (nAfn == GW3762_AFN_ROUTE_TRANSMIT)
		buf_PushData(bTx, 0x0002, 2);
	else
		buf_PushData(bTx, 0x02, 1);
	buf_PushData(bTx, nLen, 1);
	buf_Push(bTx, pData, nLen);
	buf_PushData(bTx, cs8(&bTx->p[1 + GW3762_HEADER_L_SIZE], bTx->len - (1 + GW3762_HEADER_L_SIZE)) | 0x1600, 2);
	memcpy(&bTx->p[1], (const void *)&bTx->len, GW3762_HEADER_L_SIZE);

	GW3762_DBGOUT(1, bTx->p, bTx->len);

	chl_Send(p->chl, bTx->p, bTx->len);
	buf_Release(bTx);
	
	return SYS_R_OK;
}






//External Functions
//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
sys_res gw3762_Analyze(plc_t *p)
{
	u8 *pData;
	int nLen, nOffset;
	struct gw3762_header *pH;

	chl_RecData(p->chl, p->rbuf, OS_TICK_MS);
	
	for (; ; buf_Remove(p->rbuf, 1))
	{
		for (; ; buf_Remove(p->rbuf, 1))
		{
			//不足报文头长度
			if (p->rbuf->len < sizeof(struct gw3762_header))
				return SYS_R_ERR;
			pH = (struct gw3762_header *)p->rbuf->p;
			
			//报文头检查
			if (pH->sc1 == 0x68)
			{
				if (pH->c.dir == GW3762_CODE_D_2TERMINAL)
					break;
			}
		}
		
		//不足长度
		if (p->rbuf->len < pH->len)
			return SYS_R_ERR;
		
		pData = p->rbuf->p + pH->len - 2;
		
		//CS
		if (cs8(&p->rbuf->p[sizeof(struct gw3762_header) - 1], pH->len - (sizeof(struct gw3762_header) + 1)) != *pData++)
			continue;
		
		//结束符
		if (*pData != 0x16)
			continue;

		GW3762_DBGOUT(0, pH, pH->len);

		memcpy(&p->rup, &p->rbuf->p[sizeof(struct gw3762_header)], 6);
		if (p->rup.module)
		{
			memcpy(p->madr, &p->rbuf->p[sizeof(struct gw3762_header) + 6], 6);
			memcpy(p->radr, &p->rbuf->p[sizeof(struct gw3762_header) + 12], 6);
			nOffset = 18;
		}
		else
		{
			nOffset = 6;
		}
		p->afn = p->rbuf->p[sizeof(struct gw3762_header) + nOffset];
		memcpy((void *)&p->fn, &p->rbuf->p[sizeof(struct gw3762_header) + nOffset + 1], 2);
		
		buf_Release(p->data);
		
		nLen = pH->len - (sizeof(struct gw3762_header) + nOffset + 5);
		if (nLen > 0)
			buf_Push(p->data, &p->rbuf->p[sizeof(struct gw3762_header) + nOffset + 3], nLen);
		
		buf_Remove(p->rbuf, pH->len);
		
		return SYS_R_OK;
	}
}


sys_res gw3762_Broadcast(plc_t *p, const void *pAdr, const void *pData, size_t nLen)
{
	int nAfn, nRoute, nTmo;
	u16 nDT;

	nRoute = 1;
	switch (p->type)
	{
	case PLC_T_XC_GW:
		nRoute = 0;
	case PLC_T_XC_RT:
		nAfn = GW3762_AFN_ROUTE_TRANSMIT;
		nDT = 0x0001;
		break;
	case PLC_T_ES_38:
		nRoute = 0;
	default:
		nAfn = GW3762_AFN_DATA_SET;
		nDT = 0x0004;
		break;
	}
	
	gw3762_Transmit2Meter(p, nRoute, nAfn, nDT, pAdr, 0, NULL, pData, nLen);;
	
	for (nTmo = 30000 / OS_TICK_MS; nTmo; nTmo--)
	{
		if (gw3762_Analyze(p) == SYS_R_OK)
			break;
	}
	
	if (nTmo == 0)
		return SYS_R_TMO;
	
	if (p->afn != GW3762_AFN_CONFIRM)
		return SYS_R_ERR;
	
	if (p->fn != 0x0001)
		return SYS_R_ERR;
	
	return SYS_R_OK;
}

sys_res gw3762_MeterRead(plc_t *p, const void *pAdr, int nRelay, const void *pRtAdr, const void *pData, size_t nLen)
{
	int nAfn;

	switch (p->type)
	{
	case PLC_T_ES_38:
#if XCN6N12_ENABLE
	case PLC_T_XC_GD:
#endif
		nAfn = GW3762_AFN_TRANSMIT;
		break;
	default:
		nAfn = GW3762_AFN_ROUTE_TRANSMIT;
		break;
	}
	
	return gw3762_Transmit2Meter(p, 0, nAfn, 0x0001, pAdr, nRelay, pRtAdr, pData, nLen);
}

sys_res gw3762_MeterRT(plc_t *p, const void *pAdr, const void *pData, size_t nLen)
{
	int nAfn;

	if (p->type == PLC_T_RISECOM)
		nAfn = GW3762_AFN_TRANSMIT;
	else
		nAfn = GW3762_AFN_ROUTE_TRANSMIT;
	
	return gw3762_Transmit2Meter(p, 1, nAfn, 0x0001, pAdr, 0, NULL, pData, nLen);
}


//-------------------------------------------------------------------------------------
// 确认
//-------------------------------------------------------------------------------------
sys_res gw3762_Confirm(plc_t *p, int nFlag, size_t nTmo)
{
	buf bTx = {0};
	struct gw3762_rdown xR = {0};
	
	xR.route = p->rup.route;
	xR.chlid = p->rup.chlid;

	buf_PushData(bTx, 0x01000068, 4);
	buf_Push(bTx, &xR, sizeof(xR));

	buf_PushData(bTx, GW3762_AFN_CONFIRM, 1);
	buf_PushData(bTx, 0x0001, 2);
	buf_PushData(bTx, nFlag, 2);
	buf_PushData(bTx, nTmo, 2);
	buf_PushData(bTx, cs8(&bTx->p[1 + GW3762_HEADER_L_SIZE], bTx->len - (1 + GW3762_HEADER_L_SIZE)) | 0x1600, 2);
	memcpy(&bTx->p[1], (const void *)&bTx->len, GW3762_HEADER_L_SIZE);

	GW3762_DBGOUT(1, bTx->p, bTx->len);

	chl_Send(p->chl, bTx->p, bTx->len);
	buf_Release(bTx);

	return SYS_R_OK;
}


//-------------------------------------------------------------------------------------
// 硬件初始化
//-------------------------------------------------------------------------------------
sys_res gw3762_HwReset(plc_t *p)
{
	size_t nTmo;

	gw3762_Transmit2Module(p, GW3762_AFN_RESET, 0x0001, NULL, 0);
	
	for (nTmo = 5000 / OS_TICK_MS; nTmo; nTmo--)
	{
		if (gw3762_Analyze(p) == SYS_R_OK)
			break;
	}
	
	if (nTmo == 0)
		return SYS_R_TMO;
	
	if (p->afn != GW3762_AFN_CONFIRM)
		return SYS_R_ERR;
	
	if (p->fn != 0x0001)
		return SYS_R_ERR;
	
	return SYS_R_OK;
}

//-------------------------------------------------------------------------------------
//参数初始化
//-------------------------------------------------------------------------------------
sys_res gw3762_ParaReset(plc_t *p)
{
	size_t nTmo;

	gw3762_Transmit2Module(p, GW3762_AFN_RESET, 0x0002, NULL, 0);
	
	for (nTmo = 5000 / OS_TICK_MS; nTmo; nTmo--)
	{
		if (gw3762_Analyze(p) == SYS_R_OK)
			break;
	}
	
	if (nTmo == 0)
		return SYS_R_TMO;
	
	if (p->afn != GW3762_AFN_CONFIRM)
		return SYS_R_ERR;
	
	if (p->fn != 0x0001)
		return SYS_R_ERR;
	
	return SYS_R_OK;
}

//-------------------------------------------------------------------------------------
// 获取厂商代码和版本信息
//-------------------------------------------------------------------------------------
sys_res _gw3762_InfoGet(plc_t *p)
{
	size_t nTmo;

	//查厂商代码及版本信息
	gw3762_Transmit2Module(p, GW3762_AFN_DATA_FETCH, 0x0001, NULL, 0);
	
	for (nTmo = 3000 / OS_TICK_MS; nTmo; nTmo--)
	{
		if (gw3762_Analyze(p) == SYS_R_OK)
			break;
	}
	
	if (nTmo == 0)
		return SYS_R_TMO;
	
	if (p->afn != GW3762_AFN_DATA_FETCH)
		return SYS_R_ERR;
	
	if (p->fn != 0x0001)
		return SYS_R_ERR;
	
	return SYS_R_OK;
}

sys_res gw3762_InfoGet(plc_t *p, int nRetry)
{
	int i;

	for (i = 0; i < nRetry; i++)
	{
		if (_gw3762_InfoGet(p) == SYS_R_OK)
			break;
		os_thd_sleep(1000);
	}
	
	if (i < nRetry)
		return SYS_R_OK;

	return SYS_R_ERR;
}

//-------------------------------------------------------------------------------------
// 设置模块主节点地址
//-------------------------------------------------------------------------------------
sys_res gw3762_ModAdrSet(plc_t *p)
{
	size_t nTmo;

	gw3762_Transmit2Module(p, GW3762_AFN_DATA_SET, 0x0001, p->adr, 6);
	
	for (nTmo = 3000 / OS_TICK_MS; nTmo; nTmo--)
	{
		if (gw3762_Analyze(p) == SYS_R_OK)
			break;
	}
	
	if (nTmo == 0)
		return SYS_R_TMO;
	
	if (p->afn != GW3762_AFN_CONFIRM)
		return SYS_R_ERR;
	
	if (p->fn != 0x0001)
		return SYS_R_ERR;
	
	return SYS_R_OK;
}

//-------------------------------------------------------------------------------------
// 读取从节点数量
//-------------------------------------------------------------------------------------
sys_res gw3762_SubAdrQty(plc_t *p, u16 *pQty)
{
	size_t nTmo;

	gw3762_Transmit2Module(p, GW3762_AFN_ROUTE_FETCH, 0x0001, NULL, 0);
	
	for (nTmo = 3000 / OS_TICK_MS; nTmo; nTmo--)
	{
		if (gw3762_Analyze(p) == SYS_R_OK)
			break;
	}
	
	if (nTmo == 0)
		return SYS_R_TMO;
	
	if (p->afn != GW3762_AFN_ROUTE_FETCH)
		return SYS_R_ERR;
	
	if (p->fn != 0x0001)
		return SYS_R_ERR;
	
	memcpy(pQty, &p->data->p[0], 2);
	
	return SYS_R_OK;
}

//-------------------------------------------------------------------------------------
// 读取从节点信息
//-------------------------------------------------------------------------------------
sys_res gw3762_SubAdrRead(plc_t *p, int nSn, u16 *pQty, u8 *pAdr)
{
	size_t nTmo;
	u32 nTemp;

	nTemp = 0x00010000 | nSn;
	gw3762_Transmit2Module(p, GW3762_AFN_ROUTE_FETCH, 0x0002, &nTemp, 3);
	
	for (nTmo = 2000 / OS_TICK_MS; nTmo; nTmo--)
	{
		if (gw3762_Analyze(p) == SYS_R_OK)
			break;
	}
	
	if (nTmo == 0)
		return SYS_R_TMO;
	
	if (p->afn != GW3762_AFN_ROUTE_FETCH)
		return SYS_R_ERR;
	
	if (p->fn != 0x0002)
		return SYS_R_ERR;
	
	if (pQty != NULL)
		memcpy(pQty, &p->data->p[0], 2);
	if (p->data->p[2] == 0)
		return SYS_R_ERR;
	
	memcpy(pAdr, &p->data->p[3], 6);

	return SYS_R_OK;
}

//-------------------------------------------------------------------------------------
// 读取路由运行状态
//-------------------------------------------------------------------------------------
static sys_res _gw3762_StateGet(plc_t *p)
{
	size_t nTmo;

	gw3762_Transmit2Module(p, GW3762_AFN_ROUTE_FETCH, 0x0008, NULL, 0);
	
	for (nTmo = 3000 / OS_TICK_MS; nTmo; nTmo--)
	{
		if (gw3762_Analyze(p) == SYS_R_OK)
			break;
	}
	
	if (nTmo == 0)
		return SYS_R_TMO;
	
	if (p->afn != GW3762_AFN_ROUTE_FETCH)
		return SYS_R_ERR;
	
	if (p->fn != 0x0008)
		return SYS_R_ERR;
	
	return SYS_R_OK;
}

sys_res gw3762_StateGet(plc_t *p, int nRetry)
{
	int i;

	for (i = 0; i < nRetry; i++)
	{
		if (_gw3762_StateGet(p) == SYS_R_OK)
			break;
		os_thd_sleep(1000);
	}
	
	if (i < nRetry)
		return SYS_R_OK;

	return SYS_R_ERR;
}


//-------------------------------------------------------------------------------------
// 添加从节点
//-------------------------------------------------------------------------------------
sys_res gw3762_SubAdrAdd(plc_t *p, int nSn, const void *pAdr, int nPrtl)
{
	size_t nTmo;
	u8 aBuf[10];

	aBuf[0] = 1;
	memcpy(&aBuf[1], pAdr, 6);
	memcpy(&aBuf[7], &nSn, 2);
	aBuf[9] = nPrtl;
	
	gw3762_Transmit2Module(p, GW3762_AFN_ROUTE_SET, 0x0001, aBuf, 10);
	
	for (nTmo = 2000 / OS_TICK_MS; nTmo; nTmo--)
	{
		if (gw3762_Analyze(p) == SYS_R_OK)
			break;
	}
	
	if (nTmo == 0)
		return SYS_R_TMO;
	
	if (p->afn != GW3762_AFN_CONFIRM)
		return SYS_R_ERR;
	
	if (p->fn != 0x0001)
		return SYS_R_ERR;
	
	return SYS_R_OK;
}


//-------------------------------------------------------------------------------------
// 删除从节点
//-------------------------------------------------------------------------------------
sys_res gw3762_SubAdrDelete(plc_t *p, const void *pAdr)
{
	size_t nTmo;
	u8 aBuf[7];

	aBuf[0] = 1;
	memcpy(&aBuf[1], pAdr, 6);
	gw3762_Transmit2Module(p, GW3762_AFN_ROUTE_SET, 0x0002, aBuf, 7);
	
	for (nTmo = 2000 / OS_TICK_MS; nTmo; nTmo--)
	{
		if (gw3762_Analyze(p) == SYS_R_OK)
			break;
	}
	
	if (nTmo == 0)
		return SYS_R_TMO;
	
	if (p->afn != GW3762_AFN_CONFIRM)
		return SYS_R_ERR;
	
	if (p->fn != 0x0001)
		return SYS_R_ERR;
	
	return SYS_R_OK;
}

//-------------------------------------------------------------------------------------
// 模式设置
//-------------------------------------------------------------------------------------
sys_res gw3762_ModeSet(plc_t *p, int nMode)
{
	size_t nTmo;
	u8 aBuf[3];

	aBuf[0] = nMode;
	aBuf[1] = 0x4A;
	aBuf[2] = 0x01;
	gw3762_Transmit2Module(p, GW3762_AFN_ROUTE_SET, 0x0008, aBuf, 3);
	
	for (nTmo = 3000 / OS_TICK_MS; nTmo; nTmo--)
	{
		if (gw3762_Analyze(p) == SYS_R_OK)
			break;
	}
	
	if (nTmo == 0)
		return SYS_R_TMO;
	
	if (p->afn != GW3762_AFN_CONFIRM)
		return SYS_R_ERR;
	
	if (p->fn != 0x0001)
		return SYS_R_ERR;
	
	return SYS_R_OK;
}

//-------------------------------------------------------------------------------------
// 启动搜表
//-------------------------------------------------------------------------------------
sys_res gw3762_MeterProbe(plc_t *p, int nTime)
{
	size_t nTmo;
	u8 aBuf[10];

	timet2array(rtc_GetTimet(), aBuf, 1);
	memcpy(&aBuf[6], &nTime, 2);
	aBuf[8] = 1;
	aBuf[9] = 1;
	gw3762_Transmit2Module(p, GW3762_AFN_ROUTE_SET, 0x0010, aBuf, 10);
	
	for (nTmo = 3000 / OS_TICK_MS; nTmo; nTmo--)
	{
		if (gw3762_Analyze(p) == SYS_R_OK)
			break;
	}
	
	if (nTmo == 0)
		return SYS_R_TMO;
	
	if (p->afn != GW3762_AFN_CONFIRM)
		return SYS_R_ERR;
	
	if (p->fn != 0x0001)
		return SYS_R_ERR;
	
	return SYS_R_OK;
}

//-------------------------------------------------------------------------------------
// 路由控制
//-------------------------------------------------------------------------------------
static sys_res _gw3762_RtCtrl(plc_t *p, u16 nDT)
{
	size_t nTmo;

	gw3762_Transmit2Module(p, GW3762_AFN_ROUTE_CTRL, nDT, NULL, 0);
	
	for (nTmo = 5000 / OS_TICK_MS; nTmo; nTmo--)
	{
		if (gw3762_Analyze(p) == SYS_R_OK)
			break;
	}
	if (nTmo == 0)
		return SYS_R_TMO;
	
	if (p->afn != GW3762_AFN_CONFIRM)
		return SYS_R_ERR;
	
	if (p->fn != 0x0001)
		return SYS_R_ERR;
	
	return SYS_R_OK;
}

sys_res gw3762_RtCtrl(plc_t *p, u16 nDT, int nRetry)
{
	int i;

	for (i = 0; i < nRetry; i++)
	{
		if (_gw3762_RtCtrl(p, nDT) == SYS_R_OK)
			break;
		os_thd_sleep(1000);
	}
	if (i < nRetry)
		return SYS_R_OK;

	return SYS_R_ERR;
}

//-------------------------------------------------------------------------------------
// 路由请求应答
//-------------------------------------------------------------------------------------
sys_res gw3762_RequestAnswer(plc_t *p, int nPhase, const void *pAdr, int nIsRead, const void *pData, size_t nLen)
{
	buf bTx = {0};
	struct gw3762_rdown xR = {0};

	xR.module = GW3762_RZONE_M_2METER;
	xR.chlid = nPhase;
	xR.acklen = 40;

	buf_PushData(bTx, 0x01000068, 4);
	buf_Push(bTx, &xR, sizeof(xR));
	buf_Push(bTx, p->adr, sizeof(p->adr));
	buf_Push(bTx, pAdr, 6);

	buf_PushData(bTx, GW3762_AFN_ROUTE_REQUEST, 1);
	buf_PushData(bTx, 0x0001, 2);
	if (nIsRead)
	{
		buf_PushData(bTx, 0x02, 1);
		buf_PushData(bTx, nLen, 1);
	}
	buf_Push(bTx, pData, nLen);
	buf_PushData(bTx, 0, 1);
	buf_PushData(bTx, cs8(&bTx->p[1 + GW3762_HEADER_L_SIZE], bTx->len - (1 + GW3762_HEADER_L_SIZE)) | 0x1600, 2);
	memcpy(&bTx->p[1], (const void *)&bTx->len, GW3762_HEADER_L_SIZE);

	GW3762_DBGOUT(1, bTx->p, bTx->len);

	chl_Send(p->chl, bTx->p, bTx->len);
	
	buf_Release(bTx);

	return SYS_R_OK;
}

//-------------------------------------------------------------------------------------
// 透明转发
//-------------------------------------------------------------------------------------
sys_res gw3762_Transmit(plc_t *p, buf b, const void *pData, size_t nLen)
{
	size_t nTmo;

	GW3762_DBGOUT(1, pData, nLen);

	chl_Send(p->chl, pData, nLen);

	for (nTmo = 200 / OS_TICK_MS; nTmo; nTmo--)
	{
		chl_RecData(p->chl, b, OS_TICK_MS);
	}
	if (b->len == 0)
		return SYS_R_ERR;		

	GW3762_DBGOUT(0, b->p, b->len);
	
	return SYS_R_OK;
}

//-------------------------------------------------------------------------------------
// 东软扩展-读取路由运行模式
//-------------------------------------------------------------------------------------
sys_res gw3762_Es_ModeGet(plc_t *p, u8 *pMode)
{
	size_t nTmo;

	gw3762_Transmit2ES(p, GW3762_AFN_TRANSMIT, 0x0010, NULL, 0);
	
	for (nTmo = 3000 / OS_TICK_MS; nTmo; nTmo--)
	{
		if (gw3762_Analyze(p) == SYS_R_OK)
			break;
	}
	if (nTmo == 0)
		return SYS_R_TMO;
	
	if (p->afn != GW3762_AFN_TRANSMIT)
		return SYS_R_ERR;
	
	if (p->fn != 0x0010)
		return SYS_R_ERR;
	
	*pMode = p->data->p[0];
	
	return SYS_R_OK;
}

//-------------------------------------------------------------------------------------
// 东软扩展-设置路由运行模式
//-------------------------------------------------------------------------------------
sys_res gw3762_Es_ModeSet(plc_t *p, int nMode)
{
	size_t nTmo;

	gw3762_Transmit2ES(p, GW3762_AFN_RESET, 0x0040, &nMode, 1);
	for (nTmo = 5000 / OS_TICK_MS; nTmo; nTmo--)
	{
		if (gw3762_Analyze(p) == SYS_R_OK)
			break;
	}
	if (nTmo == 0)
		return SYS_R_TMO;
	
	if (p->afn != GW3762_AFN_CONFIRM)
		return SYS_R_ERR;
	
	if (p->fn != 0x0001)
		return SYS_R_ERR;
	
	return SYS_R_OK;
}



