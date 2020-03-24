


//Private Defines
#define PLC_SLAVE_TMO		300

#define PLC_DEBUG_ENABLE	1

//东软3代模式支持(兼容老表)
#define PLC_ES_III_ENABLE	0


//Private Variables




//Internal Functions
#if PLC_DEBUG_ENABLE
#if DEBUG_LOG_ENABLE
static void PLC_DBGOUT(const char *str)
{

	log_Write(LOG_T_STRING, str, strlen(str));
}
#else
static void PLC_DBGOUT(const char *str)
{

	dbg_trace(str);
}
#endif
#else
#define PLC_DBGOUT(...)
#endif





static void plc_Rst(int nHL)
{
	t_plc_def *p = &tbl_bspPlc;

	if (p->rst_effect == GPIO_EFFECT_HIGH)
		nHL ^= 1;
	
	sys_GpioSet(&p->rst, nHL);
}

#if PLC_SET_ENABLE
static void plc_Pwr(int nHL)
{
	t_plc_def *p = &tbl_bspPlc;

	if (p->set_effect == GPIO_EFFECT_HIGH)
		nHL ^= 1;
	
	sys_GpioSet(&p->set, nHL);
}
#endif


static sys_res plc_Reset(plc_t *p)
{

	plc_Rst(0);
	os_thd_sleep(100);
	plc_Rst(1);
	os_thd_sleep(10000);
	
	chl_rs232_Config(p->chl, 9600, UART_PARI_EVEN, UART_DATA_8D, UART_STOP_1D);
	
	if (gw3762_InfoGet(p, 3) != SYS_R_OK)
	{
#if XCN6N12_ENABLE
		//晓程广东
		p->type = PLC_T_XC_GD;
		memcpy(p->info, "CXDG", 4);
		return SYS_R_OK;
#else
		p->type = PLC_T_NULL;
		return SYS_R_ERR;
#endif
	}
	memcpy(p->info, p->data->p, sizeof(p->info));
	
	if (memcmp(p->info, "GB", 2) == 0)
	{
		//成都博高
		p->type = PLC_T_BOST;
		return SYS_R_OK;
	}
	
	if (memcmp(p->info, "SETR", 4) == 0)
	{
		//东软路由
		p->type = PLC_T_ES_RT;
		return SYS_R_OK;
	}
	
	if (memcmp(p->info, "SE", 2) == 0)
	{
		//东软无路由
		p->type = PLC_T_ES_38;
		return SYS_R_OK;
	}
	
	if (memcmp(p->info, "CT", 2) == 0)
	{
		//鼎信
		p->type = PLC_T_TOPCOM;
		return SYS_R_OK;
	}
	
	if (memcmp(p->info, "HL", 2) == 0)
	{
		//力合微
		p->type = PLC_T_LEAGUERME;
		return SYS_R_OK;
	}
	
	if (memcmp(p->info, "10", 2) == 0)
	{
		//瑞斯康
		p->type = PLC_T_RISECOM;
		return SYS_R_OK;
	}
	
	if (memcmp(p->info, "im", 2) == 0)
	{
		//弥亚微
		p->type = PLC_T_MIARTECH;
		return SYS_R_OK;
	}

	if (memcmp(p->info, "SS", 2) == 0)
	{
		//航天赛世特
		p->type = PLC_T_HTSST;
		return SYS_R_OK;
	}
	
	//默认晓程
	p->type = PLC_T_XC_GW;
	
	return SYS_R_OK;
}

static int plc_MeterInit()
{
	int i;
	u8 aAdr[6];

	for (i = 0; i < LCP_SN_MAX; i++)
	{
		if (plc_MeterAdr(i, aAdr) == 0)
			break;
	}
	
	return i;
}

static sys_res plc_Sync(plc_t *p)
{
	int i, nSn, nPrtl, nFrom1, nValid, nErr;
	u16 nQty;
	u8 aBuf[6], aMadr[6], aMeter[(LCP_SN_MAX + 7) / 8];

#if XCN6N12_ENABLE
	if (p->type != PLC_T_XC_GD)
#endif
		gw3762_ModAdrSet(p);

	if (plc_IsNotSync(p))
		return SYS_R_OK;

	if (p->type == PLC_T_ES_RT)
#if PLC_ES_III_ENABLE
		gw3762_Es_ModeSet(p, 3);
#else
		gw3762_Es_ModeSet(p, 4);
#endif

	gw3762_RtCtrl(p, 0x0002, 1);
	os_thd_sleep(5000);

	nValid = plc_MeterInit();
	if (nValid == 0)
		return gw3762_ParaReset(p);

	if (p->type == PLC_T_ES_RT)
		nFrom1 = 0;
	else
		nFrom1 = 1;

	for (i = 0; i < 2; i++)
	{
		if (p->type != PLC_T_XC_RT)
		{
			if (gw3762_SubAdrQty(p, &nQty) != SYS_R_OK)
				return SYS_R_TMO;
		}
		else
		{
			nQty = LCP_SN_MAX;
		}
		
		memset(aMeter, 0, sizeof(aMeter));
		for (nSn = 0; nSn < nQty; nSn++)
		{
			if (gw3762_SubAdrRead(p, nSn + nFrom1, NULL, aBuf) == SYS_R_OK)
			{
				if (plc_MeterAdr(nSn, aMadr) == 0)
					continue;
				
				if (memcmp(aBuf, aMadr, 6)) {
					gw3762_SubAdrDelete(p, aBuf);
					continue;
				}
				
				setbit(aMeter, nSn);
			}
		}
		
		nErr = 0;
		for (nSn = 0; nSn < nValid; nSn++)
		{
			if (getbit(aMeter, nSn))
				continue;
			
			nPrtl = plc_MeterAdr(nSn, aMadr);
			gw3762_SubAdrAdd(p, nSn + nFrom1, aMadr, nPrtl);
			if (gw3762_SubAdrRead(p, nSn + nFrom1, NULL, aBuf) != SYS_R_OK)
				return SYS_R_TMO;
			
			if (memcmp(aBuf, aMadr, 6)) {
				nErr = 1;
				break;
			}
		}
		
		if (gw3762_SubAdrQty(p, &nQty) != SYS_R_OK)
			return SYS_R_TMO;
		
		if (nErr || (nQty != nValid))
		{
			if (i == 0)
			{
				if (gw3762_ParaReset(p) != SYS_R_OK)
					return SYS_R_TMO;
			}
			continue;
		}
		break;
	}
	
	if (i < 2)
		return SYS_R_OK;
	
	PLC_DBGOUT("<PLC> Sync failed...");
	
	return SYS_R_ERR;
}

static sys_res plc_Recv(plc_t *p, buf b, const u8 *pAdr, size_t nTmo)
{
	u8 *pTemp;

	for (nTmo *= (1000 / OS_TICK_MS); nTmo; nTmo--)
	{
		if (gw3762_Analyze(p) != SYS_R_OK)
			continue;
		
		switch (p->afn)
		{
		case GW3762_AFN_CONFIRM:
			//否认
			if (p->fn == 0x0002)
				return SYS_R_ERR;
			break;
			
		case GW3762_AFN_TRANSMIT:
		case GW3762_AFN_ROUTE_TRANSMIT:
			if (p->fn == 0x0001)
			{
				buf_Remove(p->data, 2);
				//645包解析
				pTemp = dlt645_PacketAnalyze(p->data->p, p->data->len);
				if (pTemp != NULL)
				{
					//校验表地址
					if (p->rup.module)
					{
						if (memcmp(p->madr, &pTemp[1], 6))
							break;;
					}
					
					if (memcmp(&pTemp[1], pAdr, 6) == 0)
					{
						buf_Push(b, &pTemp[DLT645_HEADER_SIZE - 2], pTemp[DLT645_HEADER_SIZE - 1] + 2);
						byteadd(&b->p[2], -0x33, b->p[1]);
						return SYS_R_OK;
					}
				}
			}
			break;
			
		default:
			break;
		}
	}
	
	return SYS_R_TMO;
}

static sys_res plc_Slave(plc_t *p, buf b, u8 *pAdr)
{
	u8 *pTemp;
	int nIs97, nCode;
	size_t nLen;
	u32 nDI;

	if (gw3762_Analyze(p) != SYS_R_OK)
		return SYS_R_TMO;
	
	switch (p->afn)
	{
	case GW3762_AFN_ROUTE_REQUEST:
		//路由请求
		if (p->fn == 0x0001)
		{
			nDI = plc_Request(&p->data->p[1], &nIs97);
			if (nDI)
			{
				if (nIs97)
				{
					nCode = DLT645_CODE_READ97;
					nLen = 2;
				}
				else
				{
					nCode = DLT645_CODE_READ07;
					nLen = 4;
				}
				
				dlt645_Packet2Buf(b, &p->data->p[1], nCode, &nDI, nLen);
				gw3762_RequestAnswer(p, p->data->p[0], &p->data->p[1], 1, b->p, b->len);
				
				buf_Release(b);
			}
			else
			{
				nDI = 0x00000001;
				gw3762_RequestAnswer(p, p->data->p[0], &p->data->p[1], 0, &nDI, 2);
			}
			
			return SYS_R_BUSY;
		}
		break;
		
 	case GW3762_AFN_REPORT:
		//自动上报数据
		if (p->fn == 0x0002)
		{
			gw3762_Confirm(p, 0xFFFF, 0);
			buf_Remove(p->data, 4);
			
			//645包解析
			pTemp = dlt645_PacketAnalyze(p->data->p, p->data->len);
			if (pTemp != NULL)
			{
				//校验表地址
				if (p->rup.module)
				{
					if (memcmp(p->madr, &pTemp[1], 6))
						break;
				}
				memcpy(pAdr, &pTemp[1], 6);
				buf_Push(b, &pTemp[DLT645_HEADER_SIZE - 2], pTemp[DLT645_HEADER_SIZE - 1] + 2);
				byteadd(&b->p[2], -0x33, b->p[1]);
				
				return SYS_R_OK;
			}
		}
		break;
		
	default:
		break;
	}
	
	return SYS_R_TMO;
}

static sys_res plc_MeterReport(plc_t *p, u8 *pAdr)
{

	if (gw3762_Analyze(p) != SYS_R_OK)
		return SYS_R_TMO;
	
	//自动上报表号
	if (p->afn == GW3762_AFN_REPORT) 
	{
		if (p->fn == 0x0001)
		{
			gw3762_Confirm(p, 0xFFFF, 0);
			memcpy(pAdr, &p->data->p[1], 6);
			
			return SYS_R_OK;
		}
	}
	
	return SYS_R_TMO;
}

static sys_res plc_Pause(plc_t *p, size_t nTmo)
{
	size_t nCnt;

	chl_rs232_Config(p->chl, 9600, UART_PARI_EVEN, UART_DATA_8D, UART_STOP_1D);

	nCnt = nTmo * 1000 / OS_TICK_MS;
	for (; nCnt; nCnt--, os_thd_slp1tick())
	{
		if (p->ste == PLC_S_INIT)
			continue;
		
		if (p->ste == PLC_S_SYNC)
			continue;
		
#if PLC_PROBE_ENABLE
		if (p->ste == PLC_S_PROBE)
			continue;
#endif
		break;
	}
	if (nCnt == 0)
		return SYS_R_TMO;

	switch (p->ste)
	{
	case PLC_S_SLAVE:
		if (gw3762_RtCtrl(p, 0x0002, 2) != SYS_R_OK)
			return SYS_R_ERR;
		os_thd_sleep(1000);
		p->ste = PLC_S_WAIT;
		break;
		
	case PLC_S_IDLE:
	case PLC_S_WAIT:
		break;
	default:
		return SYS_R_ERR;
	}
	
	p->tmo = nTmo;
	
	return SYS_R_OK;
}








//External Functions
int plc_IsNeedRt(plc_t *p)
{

	switch (p->type)
	{
	case PLC_T_XC_GW:
	case PLC_T_ES_38:
#if XCN6N12_ENABLE
	case PLC_T_XC_GD:
#endif
		return 1;
	default:
		return 0;
	}
}

int plc_IsNotSync(plc_t *p)
{

	switch (p->type)
	{
	case PLC_T_XC_GW:
	case PLC_T_ES_38:
	case PLC_T_RISECOM:
#if XCN6N12_ENABLE
	case PLC_T_XC_GD:
#endif
		return 1;
	default:
		return 0;
	}
}

int plc_GetRetry(plc_t *p)
{

	switch (p->type)
	{
	case PLC_T_XC_RT:
	case PLC_T_ES_38:
	case PLC_T_BOST:
		return 2;
	default:
		return 1;
	}
}

int plc_GetWait(plc_t *p, int nRelay)
{

	switch (p->type)
	{
 	case PLC_T_ES_38:
		return 12 + nRelay * 6;
	case PLC_T_XC_GW:
		return 10 + nRelay * 4;
#if XCN6N12_ENABLE
	case PLC_T_XC_GD:
		return 15 + nRelay * 10;
#endif
	case PLC_T_XC_RT:
		return 30;
	case PLC_T_TOPCOM:
		return 90;
	case PLC_T_HTSST:
		return 45;
	default:
		return 20;
	}
}



void plc_Init(plc_t *p, const u8 *pAdr)
{
	t_plc_def *pDef = &tbl_bspPlc;

	sys_GpioConf(&pDef->rst);
#if PLC_SET_ENABLE
	sys_GpioConf(&pDef->set);
#endif
	memset(p, 0, sizeof(plc_t));
	memcpy(p->adr, pAdr, 6);
	
	chl_Bind(p->chl, CHL_T_RS232, pDef->uartid, OS_TICK_MS);
}

sys_res plc_RealRead(plc_t *p, buf b, const u8 *pAdr, int nCode, const void *pData, size_t nLen, int nRelay, const u8 *pRtAdr)
{
	sys_res res = SYS_R_ERR;

#if XCN6N12_ENABLE
	if (p->type == PLC_T_XC_GD)
	{
		chl_rs232_Config(p->chl, 2400, UART_PARI_NO, UART_DATA_8D, UART_STOP_1D);
		switch (nCode) {
		case DLT645_CODE_READ97:
			res = xcn6_MeterRead(p, b, pAdr, nRelay, pRtAdr, pData, nLen);
			break;
		case DLT645_CODE_WRITE97:
			res = xcn6_MeterWrite(p, b, pAdr, nRelay, pRtAdr, pData, nLen);
			break;
		case DLT645_CODE_READ07:
		case DLT645_CODE_CTRL07:
			res = xcn12_Meter(p, b, nCode, pAdr, nRelay, pRtAdr, pData, nLen);
			break;
		default:
			res = SYS_R_ERR;
			break;
		}
	}
	else
#endif
	{
		if (plc_Pause(p, 100) != SYS_R_OK)
			return SYS_R_ERR;

		dlt645_Packet2Buf(b, pAdr, nCode, pData, nLen);
		if (plc_IsNeedRt(p))
			gw3762_MeterRead(p, pAdr, nRelay, pRtAdr, b->p, b->len);
		else
			gw3762_MeterRT(p, pAdr, b->p, b->len);
		
		buf_Release(b);
		
		res = plc_Recv(p, b, pAdr, plc_GetWait(p, nRelay));
	}

	return res;
}


sys_res plc_Transmit(plc_t *p, buf b, const void *pData, size_t nLen)
{
	u8 mAdr[6];
	size_t nTmo ;
	u8 *pTemp;

#if XCN6N12_ENABLE
	if (p->type == PLC_T_XC_GD)
	{
		chl_rs232_Config(p->chl, 2400, UART_PARI_NO, UART_DATA_8D, UART_STOP_1D);
		xcn12_Meter(p, b, DLT645_CODE_READ07, NULL, 0, NULL, pData, nLen);
	}
	else
#endif
	{
		if (plc_Pause(p, 100) != SYS_R_OK)
			return SYS_R_ERR;

		memcpy(&mAdr,&b->p[1],6);
		
		gw3762_MeterRT(p, mAdr, b->p, b->len);
		buf_Release(b);
		for (nTmo = plc_GetWait(p, 0) * (1000 / OS_TICK_MS); nTmo; nTmo--)
		{
			if (gw3762_Analyze(p) != SYS_R_OK)
				continue;
			
			switch (p->afn)
			{
			case GW3762_AFN_CONFIRM:
				//否认
				if (p->fn == 0x0002)
					return SYS_R_ERR;
				break;
				
			case GW3762_AFN_TRANSMIT:
			case GW3762_AFN_ROUTE_TRANSMIT:
				if (p->fn == 0x0001)
				{
					buf_Remove(p->data, 2);
					
					//645包解析
					pTemp = dlt645_PacketAnalyze(p->data->p, p->data->len);
					if (pTemp != NULL)
					{
						//校验表地址
						if (p->rup.module)
						{
							if (memcmp(p->madr, &pTemp[1], 6))
								break;
						}
						
						if (memcmp(&pTemp[1], p->madr, 6) == 0)
						{
							buf_Push(b, pTemp, pTemp[DLT645_HEADER_SIZE - 1] + DLT645_HEADER_SIZE+2);
							return SYS_R_OK;
						}
					}
				}
				break;
				
			default:
				break;
			}
		}
	}

	return SYS_R_ERR;
}

void plc_Broadcast(plc_t *p)
{
	buf b = {0};
	u8 aBuf[6], aTime[6];
	int i;

	memset(aBuf, 0x99, 6);
#if XCN6N12_ENABLE
	if (p->type == PLC_T_XC_GD)
	{
		chl_rs232_Config(p->chl, 2400, UART_PARI_NO, UART_DATA_8D, UART_STOP_1D);

		for (i = 0; i < 2; i++)
		{
			timet2array(rtc_GetTimet(), aTime, 1);
			xcn6_Broadcast(p, aBuf, aTime, 6);
			os_thd_sleep(3000);
		}
	}
	else
#endif
	{
		if (plc_Pause(p, 100) != SYS_R_OK)
			return;

		for (i = 0; i < 2; i++)
		{
#if PLC_ES_III_ENABLE
			if ((p->type == PLC_T_ES_RT) && i)
			{
				gw3762_Es_ModeSet(p, 4);
				os_thd_sleep(6000);
			}
#endif
			timet2array(rtc_GetTimet(), aTime, 1);
			dlt645_Packet2Buf(b, aBuf, DLT645_CODE_BROADCAST, aTime, 6);
			gw3762_Broadcast(p, aBuf, b->p, b->len);
			
			buf_Release(b);
			
			os_thd_sleep(5000);
			
#if PLC_ES_III_ENABLE
			if ((p->type == PLC_T_ES_RT) && i)
				plc_Sync(p);
#endif
		}
	}
}

void plc_Heartbeat(plc_t *p, const void *pData, size_t nLen)
{
	buf b = {0};
	u8 aBuf[32];
	int i;

	memset(aBuf, 0x99, 6);
#if XCN6N12_ENABLE
	if (p->type == PLC_T_XC_GD)
	{
		chl_rs232_Config(p->chl, 2400, UART_PARI_NO, UART_DATA_8D, UART_STOP_1D);

		for (i = 0; i < 2; i++)
		{
			xcn6_Broadcast(p, aBuf, pData, 1);
			os_thd_sleep(3000);
		}
	} else
#endif
	{
		if (plc_Pause(p, 100) != SYS_R_OK)
			return;

		for (i = 0; i < 2; i++)
		{
			dlt645_Packet2Buf(b, aBuf, DLT645_CODE_HEARTBEAT, aBuf, 1);
			gw3762_Broadcast(p, aBuf, b->p, b->len);
			
			buf_Release(b);
			
			os_thd_sleep(5000);
		}
	}
}

sys_res plc_Handler(plc_t *p, buf b, u8 *pAdr)
{
	sys_res res;

	if (p->time != (u8)rtc_GetTimet())
	{
		p->time = rtc_GetTimet();
		if (p->tmo)
			p->tmo -= 1;
	}
	switch (p->ste)
	{
	case PLC_S_INIT:
		if (plc_Reset(p) != SYS_R_OK)
		{
			os_thd_sleep(10000);
			PLC_DBGOUT("<PLC> Init failed...");
			//曾经初始化成功过但发生连续20次初始化失败时请求重启终端
			if (p->inited)
			{
				if (p->inited == 1)
					return SYS_R_RESET;
				
				p->inited -= 1;
			}
		}
		else
		{
			p->ste = PLC_S_SYNC;
			p->inited = 20;
		}
		break;
		
	case PLC_S_SYNC:
		plc_Sync(p);
		
		p->ste = PLC_S_IDLE;
		p->tmo = 3;
#if PLC_PROBE_ENABLE
#if XCN6N12_ENABLE
		if (p->type != PLC_T_XC_GD)
		{
#endif
			//允许主动注册
			if (p->probe)
			{
				if (gw3762_ModeSet(p, 0x02) == SYS_R_OK)
				{
					if (gw3762_MeterProbe(p, p->probe) == SYS_R_OK)
					{
						p->ste = PLC_S_PROBE;
						p->tmo = p->probe * 60;
					}
				}
			}
#if XCN6N12_ENABLE
		}
#endif
#endif
		break;

#if PLC_PROBE_ENABLE
	case PLC_S_PROBE:
		if (plc_MeterReport(p, pAdr) == SYS_R_OK)
			plc_NewMeter(pAdr);
		
		if ((p->tmo == 0) || (p->probe == 0))
		{
			plc_Sync(p);
			
			p->ste = PLC_S_IDLE;
			p->tmo = 3;
			p->probe = 0;
		}
		break;
#endif
	case PLC_S_IDLE:
		if (plc_IsInTime() == 0)
			break;
		
		switch (p->type)
		{
		case PLC_T_TOPCOM:
		case PLC_T_LEAGUERME:
		case PLC_T_ES_RT:
			if (p->tmo == 0)
			{
				p->tmo = PLC_SLAVE_TMO;
				p->ste = PLC_S_SLAVE;
				
				if (gw3762_RtCtrl(p, 0x0001, 2) != SYS_R_OK)
					p->ste = PLC_S_INIT;
			}
			break;
			
		default:
			break;
		}
		break;
		
	case PLC_S_WAIT:
		if (p->tmo == 0)
		{
			p->tmo = PLC_SLAVE_TMO;
			p->ste = PLC_S_SLAVE;
			
			if (gw3762_RtCtrl(p, 0x0004, 2) != SYS_R_OK)
				p->ste = PLC_S_INIT;
		}
		
	case PLC_S_SLAVE:
		//被动抄表状态
		res = plc_Slave(p, b, pAdr);
		if (res != SYS_R_TMO)
		{
			p->tmo = PLC_SLAVE_TMO;
			p->rstcnt = 0;
		}
		if (res == SYS_R_OK)
			return SYS_R_OK;
		
		//长时间无交互
		if (p->tmo == 0)
		{
			p->tmo = PLC_SLAVE_TMO;
			
			if (p->rstcnt > 5)
			{
				p->ste = PLC_S_INIT;
			}
			else
			{
				do
				{
					if (gw3762_StateGet(p, 2) == SYS_R_OK)
					{
						if (gw3762_RtCtrl(p, 0x0004, 2) == SYS_R_OK)
							break;
					}
					p->ste = PLC_S_INIT;
				}
				while (0);
			}
			
			p->rstcnt += 1;
		}
		//抄表时段完
		
#if PLC_PROBE_ENABLE
		if ((plc_IsInTime() == 0) || p->probe)
		{
#else
		if (plc_IsInTime() == 0)
		{
#endif
			if (gw3762_RtCtrl(p, 0x0002, 2) == SYS_R_OK)
				p->ste = PLC_S_SYNC;
			else
				p->ste = PLC_S_INIT;
		}
		break;
	default:
		p->ste = PLC_S_INIT;
		break;
	}

	return SYS_R_ERR;
}



void plc_GetInfo(plc_t *p, char *pInfo)
{
	int i;

	pInfo[0] = p->info[1];
	pInfo[1] = p->info[0];
	pInfo[3] = p->info[3];
	pInfo[4] = p->info[2];
	
	for (i = 0; i < 5; i++)
	{
		sprintf(&pInfo[6 + 3 * i], "%02X", p->info[4 + i]);
	}
	
	for (i = 0; i < 6; i++)
	{
		pInfo[2 + 3 * i] = ' ';
	}
}

#if PLC_PROBE_ENABLE
void plc_ProbeStart(plc_t *p, int nMin)
{

	p->probe = nMin;
}

void plc_ProbeEnd(plc_t *p)
{

	p->probe = 0;
}
#endif


