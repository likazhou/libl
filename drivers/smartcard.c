
#include <drivers/smartcard.h>


//Private Defines
#define SC_RECEIVE_TMO		2000

#if SC_REFLEX_ENABLE
#define SC_REFLEX_SIZE		5
#else
#define SC_REFLEX_SIZE		0
#endif

//Private Constants
const sc_cmd sccmd_GetResp = {0x00, 0xC0, 0x00, 0x00};
const sc_cmd sccmd_GetRandom = {0x00, 0x84, 0x00, 0x00};


//Private Functions
static int sc_DecodeATR(u8 *pData, sc_atr atr)
{
	int i = 0, flag = 0, buf = 0, protocol = 0;

	for (; *pData == 0xFF; pData++);
	atr->TS = pData[0];  /* Initial character */
	atr->T0 = pData[1];  /* Format character */
	atr->Hlength = atr->T0 & (u8)0x0F;
	if ((atr->T0 & (u8)0x80) == 0x80)
		flag = 1;
	for (atr->Tlength = 0, i = 0; i < 4; i++) {
		atr->Tlength += (((atr->T0 & (u8)0xF0) >> (4 + i)) & (u8)0x1);
	}
	for (i = 0; i < atr->Tlength; i++) {
		atr->T[i] = pData[i + 2];
	}
	protocol = atr->T[atr->Tlength - 1] & (u8)0x0F;
	while (flag) {
		if ((atr->T[atr->Tlength - 1] & (u8)0x80) == 0x80)
			flag = 1;
		else
			flag = 0;
		buf = atr->Tlength;
		atr->Tlength = 0;
		for (i = 0; i < 4; i++) {
			atr->Tlength = (((atr->T[buf - 1] & (u8)0xF0) >> (4 + i)) & (u8)0x1);
		}
		for (i = 0; i < atr->Tlength; i++) {
			atr->T[buf + i] = pData[i + 2 + buf];
		}
		atr->Tlength += (u8)buf;
	}
	for (i = 0; i < atr->Hlength; i++) {
		atr->H[i] = pData[i + 2 + atr->Tlength];
	}
	return protocol;
}


static sys_res sc_CmdLC(p_smartcard sc, const sc_cmd cmd, int lc, void *pData)
{
	sys_res res = SYS_R_ERR;
	buf b = {0};

	uart_Send(sc->parent, (void *)cmd, 4);
	uart_Send(sc->parent, &lc, 1);
	
	if ((uart_RecLen(sc->parent, b, SC_REFLEX_SIZE + 1, SC_RECEIVE_TMO)) && (b->p[SC_REFLEX_SIZE] == cmd->ins))
	{
		uart_Send(sc->parent, pData, lc);
		if (uart_RecLen(sc->parent, b, lc + SC_REFLEX_SIZE + 3, SC_RECEIVE_TMO))
			res = SYS_R_OK;

		sc->sw1 = b->p[lc + SC_REFLEX_SIZE + 1];
		sc->sw2 = b->p[lc + SC_REFLEX_SIZE + 2];
	}
	
	buf_Release(b);

	return res;
}



static sys_res sc_CmdLE(p_smartcard sc, const sc_cmd cmd, int le)
{
	sys_res res;
	buf b = {0};

	if (le > SC_LE_MAX)
		le = SC_LE_MAX;
	
	uart_Send(sc->parent, (void *)cmd, 4);
	uart_Send(sc->parent, &le, 1);
	
	if ((uart_RecLen(sc->parent, b, le + SC_REFLEX_SIZE + 3, SC_RECEIVE_TMO)) && (b->p[SC_REFLEX_SIZE] == cmd->ins))
	{
		memcpy(&sc->resp[0], &b->p[SC_REFLEX_SIZE + 1], le);
		sc->sw1 = b->p[le + SC_REFLEX_SIZE + 1];
		sc->sw2 = b->p[le + SC_REFLEX_SIZE + 2];
		res = SYS_R_OK;
	}
	else
	{
		sc->sw1 = b->p[le + SC_REFLEX_SIZE];
		sc->sw2 = b->p[le + SC_REFLEX_SIZE + 1];
		res = SYS_R_TMO;
	}
	
	buf_Release(b);
	
	return res;
}


//External Functions
void sc_Init(p_smartcard sc)
{

	memset(sc, 0, sizeof(t_smartcard));
}

void sc_Release(p_smartcard sc)
{

	if (sc->parent != NULL)
		uart_Close(sc->parent);
	
	memset(sc, 0, sizeof(t_smartcard));
}

sys_res sc_Get(p_smartcard sc, int nId, size_t nTmo)
{
	buf b = {0};
	sys_res res = SYS_R_TMO;
	sc_atr atr;

	if (sc->ste != SC_S_IDLE)
		return SYS_R_BUSY;
	
	sc_Release(sc);
	sc->parent = uart_Open(nId, nTmo);
	if (sc->parent == NULL)
		return SYS_R_TMO;
	
	uart_ScReset(sc->parent, 1);
	os_thd_slp1tick();
	uart_ScReset(sc->parent, 0);
	os_thd_slp1tick();
	uart_ScReset(sc->parent, 1);
	while (uart_RecTmo(sc->parent, b, 500) == SYS_R_OK);
	
	if (b->len)
	{
		sc->ptrl = sc_DecodeATR(b->p, atr);
		if ((sc->ptrl == SC_TYPE_PSAM) || (sc->ptrl == SC_TYPE_ESAM))
		{
			memcpy(&sc->sn[0], &atr->H[atr->Hlength - sizeof(sc->sn)], sizeof(sc->sn));
			sc->ste = SC_S_ACTIVE;
			res = SYS_R_OK;
		}
	}
	
	buf_Release(b);
	
	return res;
}

sys_res sc_GetRandom(p_smartcard sc, size_t nLen)
{
	sys_res res;

	if (sc->ste != SC_S_ACTIVE)
		return SYS_R_BUSY;
	
	sc->ste = SC_S_BUSY;
	res = sc_CmdLE(sc, sccmd_GetRandom, nLen);
	sc->ste = SC_S_ACTIVE;
	
	return res;
}

sys_res sc_Authority(p_smartcard sc, int nIns, int nP1, int nP2, void *pBuf, size_t nLen)
{
	sys_res res = SYS_R_ERR;
	sc_cmd cmd;

	if (sc->ste != SC_S_ACTIVE)
		return SYS_R_BUSY;
	
	sc->ste = SC_S_BUSY;
	cmd->cla = 0;
	cmd->ins = nIns;
	cmd->p1 = nP1;
	cmd->p2 = nP2;
	
	if (sc_CmdLC(sc, cmd, nLen, pBuf) == SYS_R_OK)
	{
		if (sc->sw1 == 0x61)
			res = sc_CmdLE(sc, sccmd_GetResp, sc->sw2);
		else
			res = SYS_R_ERR;
	}
	sc->ste = SC_S_ACTIVE;
	
	return res;
}

sys_res sc_EsamIntAuthority(p_smartcard sc, int nP1, int nP2, void *pBuf)
{
	sys_res res = SYS_R_ERR;
	sc_cmd cmd;

	if (sc->ste != SC_S_ACTIVE)
		return SYS_R_BUSY;
	
	sc->ste = SC_S_BUSY;
	cmd->cla = 0x80;
	cmd->ins = 0xFA;
	cmd->p1 = nP1;
	cmd->p2 = nP2;
	
	if (sc_CmdLC(sc, cmd, 8, (u8 *)pBuf + 8) == SYS_R_OK)
	{
		if ((sc->sw1 == 0x90) && (sc->sw2 == 0x00))
		{
			cmd->cla = 0x80;
			cmd->ins = 0xFA;
			cmd->p1 = 0x00;
			cmd->p2 = 0x00;
			if (sc_CmdLC(sc, cmd, 8, pBuf) == SYS_R_OK)
			{
				if (sc->sw1 == 0x61)
					res = sc_CmdLE(sc, sccmd_GetResp, sc->sw2);
				else
					res = SYS_R_ERR;
			}
		}
		else
		{
			res = SYS_R_ERR;
		}
	}
	
	sc->ste = SC_S_ACTIVE;
	
	return res;
}

