
//Private Defines
#define CHL_PRECODE_ENABLE				1


//Private const variables
#if CHL_PRECODE_ENABLE
static u8 const tbl_chlHeaderCodes[] = {0xFE, 0xFE, 0xFE};
#endif


//Internal Functions



//External Functions
void chl_Init(chl p)
{
	
	memset(p, 0, sizeof(chl));
	p->pIf = (void *)(-1);
}

sys_res chl_Bind(chl p, int nType, int nId, size_t nTmo)
{
	sys_res res = SYS_R_ERR;

	if (p->ste != CHL_S_IDLE)
		return SYS_R_ERR;

	switch (nType)
	{
#if UART_ENABLE
	case CHL_T_RS232:
	case CHL_T_IRDA:
		p->pIf = uart_Open(nId, nTmo);
		if (p->pIf != NULL)
			res = SYS_R_OK;
		break;
#endif
#if TCPPS_ENABLE
#if TCPPS_ETH_RECON_EN
	case CHL_T_SOC_TC_RECON:
#endif
	case CHL_T_SOC_TC:
#if MODEM_TCP_ENABLE
		if (modem_IsMTcp())
		{
			res = SYS_R_OK;
			break;
		}
#endif
	case CHL_T_SOC_TS:
	case CHL_T_SOC_UC:
	case CHL_T_SOC_US:
		res = chl_soc_Bind(p, nType, nId);
		break;
#endif
	default:
		return SYS_R_ERR;
	}
	
	if (res == SYS_R_OK)
	{
		p->type = nType;
		p->ste = CHL_S_STANDBY;
	}
	return res;
}

sys_res chl_Release(chl p)
{
	sys_res res = SYS_R_ERR;

	switch (p->type)
	{
#if UART_ENABLE
	case CHL_T_RS232:
	case CHL_T_IRDA:
		if (((int)p->pIf != -1) && (p->pIf != NULL))
			res = uart_Close(p->pIf);
		break;
#endif
#if TCPPS_ENABLE
#if TCPPS_ETH_RECON_EN
	case CHL_T_SOC_TC_RECON:
#endif
	case CHL_T_SOC_TC:
#if MODEM_TCP_ENABLE
		if (modem_IsMTcp())
		{
			res = mtcp_TcpClose();
			break;
		}
#endif
	case CHL_T_SOC_TS:
	case CHL_T_SOC_UC:
	case CHL_T_SOC_US:
		if ((int)p->pIf != -1)
		{
			if (closesocket((int)p->pIf) == 0)
				res = SYS_R_OK;
		}
		break;
#endif
	default:
		break;
	}
	if (res != SYS_R_OK)
		return res;

	p->ste = CHL_S_IDLE;
	return res;
}

#if TCPPS_TYPE == TCPPS_T_LWIP
static int lwip_nIsSendBusy = 0;
#endif
sys_res chl_Send(chl p, const void *pData, size_t nLen)
{
	sys_res res = SYS_R_ERR;
#if TCPPS_TYPE == TCPPS_T_LWIP
	size_t nTmo;
	int nSend, nSended;
	u8 *pSnd;
#endif

	if (p->ste != CHL_S_READY)
		return SYS_R_ERR;
	switch (p->type)
	{
#if UART_ENABLE
	case CHL_T_IRDA:
#if CHL_PRECODE_ENABLE
		uart_Send(p->pIf, (void *)tbl_chlHeaderCodes, sizeof(tbl_chlHeaderCodes));
#endif
	case CHL_T_RS232:
		res = uart_Send(p->pIf, pData, nLen);
		break;
#endif
#if TCPPS_ENABLE
#if TCPPS_ETH_RECON_EN
	case CHL_T_SOC_TC_RECON:
#endif
	case CHL_T_SOC_TC:
#if MODEM_TCP_ENABLE
		if (modem_IsMTcp())
		{
			res = mtcp_TcpSend(pData, nLen);

			if (res != SYS_R_OK)
				res = mtcp_TcpSend(pData, nLen);
			break;
		}
#endif
	case CHL_T_SOC_TS:
	case CHL_T_SOC_UC:
	case CHL_T_SOC_US:
#if TCPPS_TYPE == TCPPS_T_LWIP
		for (; lwip_nIsSendBusy; os_thd_slp1tick());
		lwip_nIsSendBusy = 1;

		pSnd = (u8 *)pData;
		nSended = 0;
		for (nTmo = 60; nTmo; nTmo--)
		{
			nSend = send((int)p->pIf, &pSnd[nSended], nLen, MSG_DONTWAIT);
			if (nSend > 0)
			{
				if (nSend >= nLen)
					break;
				nLen -= nSend;
				nSended += nSend;
			}
			os_thd_sleep(500);
		}

		lwip_nIsSendBusy = 0;
#endif
#if TCPPS_TYPE == TCPPS_T_KEILTCP
		send((int)p->pIf, pData, nLen, MSG_DONTWAIT);
#endif
		res = SYS_R_OK;
		break;
#endif
	default:
		break;
	}
	return res;
}

sys_res chl_RecData(chl p, buf b, size_t nTmo)
{
	sys_res res = SYS_R_ERR;
#if TCPPS_ENABLE
	char aBuf[64];
	int nLen;
#endif

	if (p->ste != CHL_S_READY)
		return SYS_R_ERR;

	switch (p->type)
	{
#if UART_ENABLE
	case CHL_T_RS232:
	case CHL_T_IRDA:
		res = uart_RecTmo(p->pIf, b, nTmo);
		break;
#endif
#if TCPPS_ENABLE
#if TCPPS_ETH_RECON_EN
	case CHL_T_SOC_TC_RECON:
#endif
	case CHL_T_SOC_TC:
#if MODEM_TCP_ENABLE
		if (modem_IsMTcp())
		{
			mtcp_TcpRecv(b);
			break;
		}
#endif
	case CHL_T_SOC_TS:
	case CHL_T_SOC_UC:
	case CHL_T_SOC_US:
		for (nTmo /= OS_TICK_MS; nTmo; nTmo--)
		{
			while ((nLen = recv((int)(p->pIf), aBuf, sizeof(aBuf), MSG_DONTWAIT)) > 0)
			{
				buf_Push(b, aBuf, nLen);
				res = SYS_R_OK;
			}
			
			if (res == SYS_R_OK)
				break;
		
			os_thd_slp1tick();
		}
		break;
#endif
	default:
		break;
	}
	return res;
}


