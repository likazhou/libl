


//Private Defines
#define XCN6_HEADER_TX_SIZE			12
#define XCN6_HEADER_RX_SIZE			4



//Private Typedef


//Internal Functions
#if XCN6N12_DEBUG_ENABLE
static void XC_DBGOUT(int nType, const void *pBuf, size_t nLen)
{
	const u8 *pData, *pEnd;
	char str[DBG_BUF_SIZE];

	pData = (const u8 *)pBuf;
	pEnd = pData + nLen;

	if (nType)
		nLen = sprintf(str, "%s", "<XCT>");
	else
		nLen = sprintf(str, "%s", "<XCR>");
	
	while ((pData < pEnd) && (nLen < (sizeof(str) - 3)))
	{
		nLen += sprintf(&str[nLen], " %02X", *pData++);
	}

	dbg_trace(str);
}
#else
#define XC_DBGOUT(...)
#endif




static sys_res xcn6_Transmit2Meter(plc_t *p, int nCtrl, const void *pAdr, int nRelay, 
												const u8 *pRtAdr, const u8 *pData, size_t nLen)
{
	int i;
	buf bTx = {0};

	buf_Push(bTx, "SND", 3);
	buf_PushData(bTx, XCN6_HEADER_TX_SIZE + 6 + nRelay * 3 + nLen, 1);
	buf_Fill(bTx, 0xFF, 6);
	buf_PushData(bTx, 0xAF09, 2);
	if (nRelay)
	{
		buf_Push(bTx, pRtAdr, 3);
		SETBIT(nCtrl, 6);
		nCtrl |= ((nRelay - 1) << 4);
	}
	else
	{
		buf_Push(bTx, pAdr, 3);
	}
	buf_PushData(bTx, nCtrl, 1);
	buf_PushData(bTx, 3 + nRelay * 3 + nLen, 1);
	buf_Push(bTx, pData, 2);
	if (nRelay)
	{
		for (i = 1; i < nRelay; i++)
		{
			buf_Push(bTx, &pRtAdr[i * 6], 3);
		}
		buf_Push(bTx, pAdr, 3);
	}
	buf_Fill(bTx, 0xBB, 3);
	if (nLen > 2)
		buf_Push(bTx, &pData[2], nLen - 2);
	
	i = cs16(0, &bTx->p[XCN6_HEADER_TX_SIZE], 8 + nRelay * 3 + nLen);
	buf_PushData(bTx, i, 3);

	XC_DBGOUT(1, bTx->p, bTx->len);

	chl_Send(p->chl, bTx->p, bTx->len);
	buf_Release(bTx);

	return SYS_R_OK;
}

static sys_res xcn6_Analyze(plc_t *p)
{
	u8 *pData;
	size_t nLen, nTemp;

	chl_RecData(p->chl, p->rbuf, OS_TICK_MS);
	for (; ; buf_Remove(p->rbuf, 1))
	{
		for (; ; buf_Remove(p->rbuf, 1))
		{
			//不足报文头长度
			if (p->rbuf->len < XCN6_HEADER_RX_SIZE)
				return SYS_R_ERR;

			//报文头检查
			pData = p->rbuf->p;
			if (memcmp(pData, "DAT", 3) == 0)
				break;
		}
		nLen = pData[3];
		//不足长度
		if (p->rbuf->len < (nLen + (XCN6_HEADER_RX_SIZE + 1)))
			return SYS_R_ERR;
		//CS
		nTemp = cs16(0, &pData[XCN6_HEADER_RX_SIZE], nLen - 2);
		if (memcmp(&pData[nLen + 2], &nTemp, 2))
			continue;

		XC_DBGOUT(0, pData, nLen + (XCN6_HEADER_RX_SIZE + 1));

		nTemp = pData[XCN6_HEADER_RX_SIZE + 3];
		pData += (XCN6_HEADER_RX_SIZE + 5);

		//方向
		if ((nTemp & BITMASK(7)) == 0)
			continue;

		buf_Release(p->data);
		buf_PushData(p->data, nTemp, 1);
		buf_PushData(p->data, nLen - (XCN6_HEADER_RX_SIZE + 7), 1);
		if (nTemp & BITMASK(6))
		{
			memcpy(p->madr, pData, 3);
			memset(&p->madr[3], 0, 3);
#if 0
			pData += 4;
			buf_Push(p->data, pData, 1);
#else
			//采集器否认不正确,构造错误字
			buf_PushData(p->data, BITMASK(2), 1);
#endif
		}
		else
		{	
			if ((nTemp & 0x0F) == 0x01)
			{
				buf_Push(p->data, pData, 2);
				pData += 2;
				memcpy(p->madr, pData, 3);
				memset(&p->madr[3], 0, 3);
				pData += 4;
				buf_Push(p->data, pData, nLen - (XCN6_HEADER_RX_SIZE + 9));
			}
			else
			{
				memcpy(p->madr, pData, 3);
				memset(&p->madr[3], 0, 3);
			}
		}
		buf_Remove(p->rbuf, nLen + (XCN6_HEADER_RX_SIZE + 1));
		return SYS_R_OK;
	}
}

static sys_res xcn12_Transmit2Meter(plc_t *p, int nCtrl, const void *pAdr, int nRelay, 
												const u8 *pRtAdr, const u8 *pData, size_t nLen)
{
	buf bTx = {0};

	buf_PushData(bTx, 0xFCFC, 2);
	buf_PushData(bTx, 0x68, 1);
	if (nRelay)
		buf_Push(bTx, pRtAdr, 6);
	else
		buf_Push(bTx, pAdr, 6);
	buf_PushData(bTx, 0x68, 1);
	buf_PushData(bTx, (nRelay << 5) | nCtrl, 1);
	buf_PushData(bTx, nRelay * 6 + nLen, 1);
	if (nRelay)
	{
		buf_Push(bTx, &pRtAdr[6], (nRelay - 1) * 6);
		buf_Push(bTx, pAdr, 6);
	}
	buf_Push(bTx, pData, nLen);
	byteadd(&bTx->p[12], 0x33, bTx->p[11]);
	buf_PushData(bTx, 0x1600 | cs8(&bTx->p[2], bTx->len - 2), 2);

	XC_DBGOUT(1, &bTx->p[2], bTx->len - 2);
 
	chl_Send(p->chl, bTx->p, bTx->len);
	buf_Release(bTx);

	return SYS_R_OK;
}


//External Functions
sys_res xcn12_Meter(plc_t *p, buf b, int nCode, const void *pAdr, int nRelay, const void *pRtAdr, 
							const void *pData, size_t nLen)
{
	size_t nTmo;
	u8 *pH;

	xcn12_Transmit2Meter(p, nCode, pAdr, nRelay, pRtAdr, pData, nLen);
	nTmo = plc_GetWait(p, nRelay);
	for (nTmo *= 1000 / OS_TICK_MS; nTmo; nTmo--)
	{
		if (chl_RecData(p->chl, b, OS_TICK_MS) != SYS_R_OK)
			continue;
		
		pH = dlt645_PacketAnalyze(b->p, b->len);
		if (pH == NULL)
			continue;
		
		buf_Remove(b, pH - b->p);

		XC_DBGOUT(0, b->p, b->p[9] + (DLT645_HEADER_SIZE + 2));

//		if (memcmp(&b->p[1], pAdr, 6))
//		{
//			buf_Remove(b, DLT645_HEADER_SIZE);
//			continue;
//		}
		buf_Remove(b, DLT645_HEADER_SIZE - 2);
		byteadd(&b->p[2], -0x33, b->p[1]);
		
		return SYS_R_OK;
	}
	
	return SYS_R_TMO;
}

sys_res xcn12_Broadcast(plc_t *p, const void *pAdr, const void *pData, size_t nLen)
{

	return xcn12_Transmit2Meter(p, DLT645_CODE_BROADCAST, pAdr, 0, NULL, pData, nLen);
}

sys_res xcn6_MeterRead(plc_t *p, buf b, const void *pAdr, int nRelay, const void *pRtAdr, 
								const void *pData, size_t nLen)
{
	size_t nTmo;

	xcn6_Transmit2Meter(p, 0x01, pAdr, nRelay, pRtAdr, pData, nLen);
	nTmo = plc_GetWait(p, nRelay);
	for (nTmo *= 1000 / OS_TICK_MS; nTmo; nTmo--)
	{
		if (xcn6_Analyze(p) != SYS_R_OK)
			continue;
		
		if (memcmp(p->madr, pAdr, 3))
			continue;
		
		buf_Push(b, p->data->p, p->data->len);
		
		return SYS_R_OK;
	}
	
	return SYS_R_TMO;
}

sys_res xcn6_MeterWrite(plc_t *p, buf b, const void *pAdr, int nRelay, const void *pRtAdr, 
								const void *pData, size_t nLen)
{
	size_t nTmo;

	xcn6_Transmit2Meter(p, 0x04, pAdr, nRelay, pRtAdr, pData, nLen);
	nTmo = plc_GetWait(p, nRelay);
	for (nTmo *= 1000 / OS_TICK_MS; nTmo; nTmo--)
	{
		if (xcn6_Analyze(p) != SYS_R_OK)
			continue;
		
		if (memcmp(p->madr, pAdr, 3))
			continue;
		
		buf_Push(b, p->data->p, p->data->len);
		
		return SYS_R_OK;
	}
	
	return SYS_R_TMO;
}

sys_res xcn6_Broadcast(plc_t *p, const void *pAdr, const void *pData, size_t nLen)
{
	u32 i;
	buf bTx = {0};

	buf_Push(bTx, "SND", 3);
	buf_PushData(bTx, XCN6_HEADER_TX_SIZE + 6 + nLen, 1);
	buf_Fill(bTx, 0xFF, 6);
	buf_PushData(bTx, 0xAF09, 2);
	buf_Push(bTx, pAdr, 3);
	buf_PushData(bTx, 0x08, 1);
	buf_PushData(bTx, 3 + nLen, 1);
	buf_Fill(bTx, 0xBB, 3);
	buf_Push(bTx, pData, nLen);
	i = cs16(0, &bTx->p[XCN6_HEADER_TX_SIZE], 8 + nLen);
	buf_PushData(bTx, i, 3);

	XC_DBGOUT(1, bTx->p, bTx->len);

	chl_Send(p->chl, bTx->p, bTx->len);
	buf_Release(bTx);

	return SYS_R_OK;
}


