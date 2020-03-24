#if DLRCP_ENABLE

//Private Defines
#if DLRCP_DEBUG_ENABLE
void DLRCP_DBGOUT(const str *p)
{

#if DEBUG_LOG_ENABLE
	log_Write(LOG_T_STRING, p, strlen(p));
#else
	dbg_trace(p);
#endif
}
#else
#define DLRCP_DBGOUT(...)
#endif

//发送报文类型
#define DLRCP_LINKCHECK_LOGIN		0
#define DLRCP_LINKCHECK_LOGOUT		1
#define DLRCP_LINKCHECK_KEEPALIVE	2

//TCP连接超时
#define DLRCP_TCP_CONNECT_TMO		16




void dlrcp_Init(p_dlrcp p, sys_res (*linkcheck)(void *, int), sys_res (*analyze)(void *))
{

	memset(p, 0, sizeof(struct dlrcp));
	chl_Init(p->chl);
	p->refresh = 3 * 60;
	p->linkcheck = linkcheck;
	p->analyze = analyze;
}


//-------------------------------------------------------------------------
//通道设置
//-------------------------------------------------------------------------
sys_res dlrcp_SetChl(p_dlrcp p, int nType, int nId, int nPar1, int nPar2, int nPar3, int nPar4)
{
	int nChanged = 0;

	if (p->chl->type != nType)
	{
		p->chl->type = nType;
		nChanged = 1;
	}

	if (p->chlid != nId)
	{
		p->chlid = nId;
		nChanged = 1;
	}

	switch (nType)
	{
#if TCPPS_ENABLE
#if TCPPS_ETH_RECON_EN
	case CHL_T_SOC_TC_RECON:
#endif
	case CHL_T_SOC_TC:
	case CHL_T_SOC_TS:
	case CHL_T_SOC_UC:
	case CHL_T_SOC_US:
		if (p->ip[0] != nPar1)
		{
			p->ip[0] = nPar1;
			nChanged = 1;
		}
		if (p->ip[1] != nPar2)
		{
			p->ip[1] = nPar2;
			nChanged = 1;
		}
		if (p->ip[2] != nPar3)
		{
			p->ip[2] = nPar3;
			nChanged = 1;
		}
		if (p->ip[3] != nPar4)
		{
			p->ip[3] = nPar4;
			nChanged = 1;
		}
		break;
#endif
#if UART_ENABLE
	case CHL_T_RS232:
	case CHL_T_IRDA:
		if (p->uart.baud != nPar1)
		{
			p->uart.baud = nPar1;
			nChanged = 1;
		}
		if (p->uart.pari != nPar2)
		{
			p->uart.pari = nPar2;
			nChanged = 1;
		}
		if (p->uart.data != nPar3)
		{
			p->uart.data = nPar3;
			nChanged = 1;
		}
		if (p->uart.stop != nPar4)
		{
			p->uart.stop = nPar4;
			nChanged = 1;
		}
		break;
#endif
	default:
		return SYS_R_ERR;
	}
	if (nChanged)
		chl_Release(p->chl);

	return SYS_R_OK;
}


//-------------------------------------------------------------------------
//发送报文
//-------------------------------------------------------------------------
sys_res dlrcp_TmsgSend(p_dlrcp p, void *pHeader, size_t nHeaderLen, void *pData, size_t nDataLen, int nType)
{
	sys_res res = SYS_R_ERR;
	buf b = {0};

	buf_Push(b, pHeader, nHeaderLen);
	buf_Push(b, pData, nDataLen);
#if LIB_ZIP_ENABLE
	if (p->zip)
	{
		int nLen;
		nLen = EnData(b->p, b->len, EXE_COMPRESS_NEW);
		buf_Release(b);
		if (nLen > 0)
			buf_Push(b, SendBuf, nLen);
	}
#endif
#if TCPPS_ETH_RECON_EN
	if ((nType == DLRCP_TMSG_REPORT) && (p->chl->type == CHL_T_SOC_TC_RECON))
	{
		size_t nTmo;
		chl_Release(p->chl);
		chl_Bind(p->chl, p->chl->type, p->chlid, OS_TICK_MS);
		chl_soc_Connect(p->chl, p->ip, p->chlid);

		for (nTmo = 5000 / OS_TICK_MS ; nTmo; nTmo--)
		{
			if (chl_soc_IsConnect(p->chl))
				break;
			os_thd_slp1tick();
		}
	}
#endif
	res = chl_Send(p->chl, b->p, b->len);
	buf_Release(b);
	
	return res;
}

//-------------------------------------------------------------------------
//通讯处理
//-------------------------------------------------------------------------
sys_res dlrcp_Handler(p_dlrcp p)
{
	sys_res res = SYS_R_ERR;
#if TCPPS_ENABLE
	time_t tTime;
#endif
	
	switch (p->chl->ste)
	{
	default:
		p->ste = DLRCP_S_IDLE;
		
		if (chl_Bind(p->chl, p->chl->type, p->chlid, OS_TICK_MS) != SYS_R_OK)
			os_thd_slp1tick();
		
		break;
	case CHL_S_STANDBY:
		switch (p->chl->type)
		{
#if TCPPS_ENABLE
#if TCPPS_ETH_RECON_EN
		case CHL_T_SOC_TC_RECON:
#endif
		case CHL_T_SOC_TC:
		case CHL_T_SOC_UC:
			if (p->cnt == 0)
			{
				if (chl_soc_Connect(p->chl, p->ip, p->chlid) != SYS_R_OK)
					p->cnt = 1;
				
				DLRCP_DBGOUT("[RCP] connecting");
			}
			else
			{
				tTime = rtc_GetTimet();
				if (p->time == (u8)tTime)
				{
					os_thd_slp1tick();
					break;
				}
				
				p->time = tTime;
				p->cnt += 1;
				
				if (p->cnt > (p->refresh >> 3))
					p->cnt = 0;
			}
			break;
		case CHL_T_SOC_TS:
		case CHL_T_SOC_US:
  			chl_soc_Listen(p->chl);
			break;
#endif
#if UART_ENABLE
		case CHL_T_RS232:
		case CHL_T_IRDA:
			chl_rs232_Config(p->chl, p->uart.baud, p->uart.pari, p->uart.data, p->uart.stop);
			break;
#endif
		default:
			break;
		}
		break;
	case CHL_S_CONNECT:
		if (p->time == (u8)rtc_GetTimet())
		{
			os_thd_slp1tick();
			break;
		}
		
		p->time = rtc_GetTimet();
		p->cnt += 1;
		
		switch (p->chl->type)
		{
#if TCPPS_ENABLE
#if TCPPS_ETH_RECON_EN
		case CHL_T_SOC_TC_RECON:
#endif
		case CHL_T_SOC_TC:
		case CHL_T_SOC_UC:
			if (chl_soc_IsConnect(p->chl))
			{
				p->cnt = 0;
				p->ste = DLRCP_S_CHECK;
				(p->linkcheck)(p, DLRCP_LINKCHECK_LOGIN);
				
				DLRCP_DBGOUT("[RCP] login");
			}
			else
			{
				if (p->cnt > DLRCP_TCP_CONNECT_TMO)
					chl_Release(p->chl);
			}
			break;
		case CHL_T_SOC_TS:
		case CHL_T_SOC_US:
			if (p->cnt > 900)
			{
				p->cnt = 0;
				chl_Release(p->chl);
			}
			
			if (chl_soc_IsConnect(p->chl))
				p->cnt = 0;
			
			break;
#endif
		default:
			break;
		}
		break;
	case CHL_S_READY:
		//接收处理
		if (p->time != (u8)rtc_GetTimet())
		{
			p->time = rtc_GetTimet();
			p->cnt += 1;
			
			//链路检测
			switch (p->chl->type)
			{
#if TCPPS_ENABLE
#if TCPPS_ETH_RECON_EN
			case CHL_T_SOC_TC_RECON:
#endif
			case CHL_T_SOC_TC:
			case CHL_T_SOC_UC:
				if (chl_soc_IsConnect(p->chl))
				{
					if (p->cnt > p->refresh)
					{
						p->cnt = 0;
						switch (p->ste)
						{
						case DLRCP_S_CHECK:
							chl_Release(p->chl);
							break;
						case DLRCP_S_READY:
							(p->linkcheck)(p, DLRCP_LINKCHECK_KEEPALIVE);
							DLRCP_DBGOUT("[RCP] keeping-alive");
							
							if (p->retry < 3)
							{
								p->retry += 1;
								p->cnt = p->refresh - 20;
							}
							else
							{
								p->ste = DLRCP_S_CHECK;
							}
							break;
						default:
							break;
						}
					}
				}
				else
				{
					chl_Release(p->chl);
					DLRCP_DBGOUT("[RCP] connect abort");
				}
				break;
			case CHL_T_SOC_TS:
			case CHL_T_SOC_US:
				if ((p->cnt > p->refresh) || (chl_soc_IsConnect(p->chl) == 0))
					chl_Release(p->chl);
				
				break;
#endif
#if UART_ENABLE
			case CHL_T_RS232:
			case CHL_T_IRDA:
				if (p->cnt > 900)
				{
					p->cnt = 0;
					chl_Release(p->chl);
				}
				
				break;
#endif
			default:
				break;
			}
		}
		
		res = (p->analyze)(p);
		if (res == SYS_R_OK)
		{
			DLRCP_DBGOUT("[RCP] recv");
			p->cnt = 0;
			p->retry = 0;
			switch (p->chl->type)
			{
#if TCPPS_ENABLE
#if TCPPS_ETH_RECON_EN
			case CHL_T_SOC_TC_RECON:
#endif
			case CHL_T_SOC_TC:
			case CHL_T_SOC_UC:
			case CHL_T_SOC_TS:
			case CHL_T_SOC_US:
				p->ste = DLRCP_S_READY;
#if MODEM_ENABLE
				modem_Refresh();
#endif
				break;
#endif
			default:
				break;
			}
		}
		
		break;
	}
	
	return res;
}

#endif



