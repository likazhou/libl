#if RN8302_ENABLE
#include <drivers/rn8302.h>


//Private Defines
#define RN8302_FOSC					8192000.0f

#define RN8302_SPI_SPEED			200000



//Private Variables
static rn8302_t rn_x8302 = {0};


//Private Macros







//Internal Functions
#if RN8302_DEBUG_ENABLE
#define RN8302_DBGOUT				dbg_printf
#else
#define RN8302_DBGOUT(...)
#endif



static void rn8302_SpiGet()
{
	rn8302_t *p = &rn_x8302;
	
	p->spi = spi_Open(RN8302_COMID, OS_TMO_FOREVER);
	spi_Config(p->spi, SPI_SCKIDLE_LOW, SPI_LATCH_2EDGE, RN8302_SPI_SPEED);
#if SPI_SEL_ENABLE
	spi_CsSel(p->spi, RN8302_CSID);
#endif
}




//External Functions
void rn8302_Init()
{
	t_gpio_def *pDef;

	for (pDef = tbl_bspRn8302[0]; pDef < tbl_bspRn8302[1]; pDef++)
	{
		sys_GpioConf(pDef);
	}
}


sys_res rn8302_Reset(rn8302_cali_t *pCali)
{
	u32 nTemp;
	int i;
	rn8302_t *p = &rn_x8302;

	//复位RN8302B
	sys_GpioSet(gpio_node(tbl_bspRn8302, 0), 0);
	sys_Delay(3000);				//delay 30us
	sys_GpioSet(gpio_node(tbl_bspRn8302, 0), 1);
	os_thd_slp1tick();

	p->ec = pCali->EC;
	p->kv = pCali->kv;
	p->ki = pCali->ki;
	p->kp = pCali->kp;

	nTemp = 0xE5;
	rn8302_Write(RN8302_REG_WDISABLE, &nTemp, 1);

	nTemp = 0;
	rn8302_Read(RN8302_REG_SYSSR, &nTemp, 2);
	if ((nTemp & 0x0080) == 0)
		return SYS_R_ERR;
	
	rn8302_Write(RN8302_REG_HFCONST1, &pCali->HFConst, 2);

	for (i = 0; i < 3; i++)
	{
		rn8302_Write(RN8302_REG_GSUA + i, &pCali->Ugain[i], 2);
		rn8302_Write(RN8302_REG_GSIA + i, &pCali->Igain[i], 2);
		rn8302_Write(RN8302_REG_PHSUA + i, &pCali->UPhs[i], 1);
		rn8302_Write(RN8302_REG_PHSIA + i, &pCali->IPhs[i], 3);
	}

	nTemp = 0xFFFFFF;
	rn8302_Write(RN8302_REG_EMUCON, &nTemp, 3);

	nTemp = 0xDC;
	rn8302_Write(RN8302_REG_WDISABLE, &nTemp, 1);
	
	os_thd_sleep(2000);

	nTemp = 0;
	rn8302_Read(RN8302_REG_CheckSum1, &nTemp, 3);
	if (nTemp != pCali->crc)
		return SYS_R_ERR;

	p->crc = nTemp;
	
	return SYS_R_OK;
}




int rn8302_Read(u16 nReg, void *pData, size_t nLen)
{
	s32 nData, nCrc;
	rn8302_t *p = &rn_x8302;

	rn8302_SpiGet();

	//读数据寄存器
	nData = 0;
	spi_TransThenRecv(p->spi, &nReg, 2, &nData, nLen);
	invert(&nData, nLen);

	//读校验寄存器
	os_thd_slp1tick();
	nReg = RN8302_REG_RData;
	nCrc = 0;
	spi_TransThenRecv(p->spi, &nReg, 2, &nCrc, 4);
	switch (nLen)
	{
	case 2:
		nCrc >>= 16;
		nCrc &= 0x0000FFFF;
		break;
	case 3:
		nCrc >>= 8;
		nCrc &= 0x00FFFFFF;
		break;
	default:
		break;
	}
	invert(&nCrc, nLen);

	spi_Close(p->spi);

	if (nData != nCrc)
	{
		RN8302_DBGOUT("<RN8302>ReadReg %02X Err %x %x", nReg, nData, nCrc);
		return 0;
	}
	memcpy(pData, &nData, nLen);
	
	return 1;
}

int rn8302_Write(u16 nReg, const void *pData, size_t nLen)
{
	u32 nCrc;
	u8 aBuf[7];
	rn8302_t *p = &rn_x8302;

	rn8302_SpiGet();

	//写数据寄存器
	nReg += 0x8000;
	memcpy(&aBuf[0], &nReg, 2);
	memcpy(&aBuf[2], pData, nLen);
	invert(&aBuf[2], nLen);
	aBuf[2 + nLen] = ~cs8(aBuf, 2 + nLen);
	spi_Send(p->spi, aBuf, 3 + nLen);

	//读校验寄存器
	os_thd_slp1tick();
	nReg = RN8302_REG_WData;
	nCrc = 0;
	spi_TransThenRecv(p->spi, &nReg, 2, &nCrc, 3);
	switch (nLen)
	{
	case 1:
		nCrc >>= 16;
		nCrc &= 0x000000FF;
		break;
	case 2:
		nCrc >>= 8;
		nCrc &= 0x0000FFFF;
		break;
	default:
		break;
	}

	spi_Close(p->spi);

	if (memcmp(&aBuf[2], &nCrc, nLen))
	{
		RN8302_DBGOUT("<RN8302>WriteReg %02X Err %x %x", nReg, nData, nCrc);
		return 0;
	}
	return 1;
}





int rn8302_CheckSum()
{
	u32 nData;

	nData = 0;
	if (rn8302_Read(RN8302_REG_CheckSum1, &nData, 3))
	{
		if (nData == rn_x8302.crc)
			return 1;
	}

	return 0;
}


const u8 tbl_rn8302RegEnergy[6] = {
	RN8302_REG_PosEPA,
	RN8302_REG_NegEPA,
	RN8302_REG_PosEQA,
	RN8302_REG_NegEQA,
	RN8302_REG_NegEQA,
	RN8302_REG_PosEQA,
};

u32 rn8302_GetEnergy(int nPhase, int nType)
{
	u32 nData;

	nData = 0;
	rn8302_Read(tbl_rn8302RegEnergy[nType] + nPhase, &nData, 3);

	return nData;
}


//------------------------------------------------------------------------
//名	称:  rn8302_GetFreq ()
//设	计: 
//输	入:  -
//输	出:  -
//返	回:  频率值
//功	能:  获取频率值
//------------------------------------------------------------------------
float rn8302_GetFreq()
{
	u32 nData = 0;
	float fData;

	if (rn8302_Read(RN8302_REG_UFreq, &nData, 3) == 0)
		return 0;
	
	fData = (RN8302_FOSC * 8.0f / (float)nData);
	
	return fData;
}


//------------------------------------------------------------------------
//名	称: rn8302_GetUI()
//设	计: 
//输	入: nPhase - 相位值
//输	出: -
//返	回: 电压有效值
//功	能: 获取当前电压
//------------------------------------------------------------------------
float rn8302_GetUI(int nPhase, int nType)
{
	s32 nData;
	u16 nReg;
	float fData;

	switch (nType)
	{
	case 0: //电压
		nReg = RN8302_REG_UA + nPhase;
		fData = rn_x8302.kv;
		break;
	case 1: //电流
		nReg = RN8302_REG_IA + nPhase;
		fData = rn_x8302.ki;
		break;
	case 2: //电压谐波
		nReg = RN8302_REG_HUA + nPhase;
		fData = rn_x8302.kv;
		break;
	case 3: //电流谐波
		nReg = RN8302_REG_HIA + nPhase;
		fData = rn_x8302.ki;
		break;
	case 4: //电流SUM
		nReg = RN8302_REG_ISUM;
		fData = rn_x8302.ki;
		break;
	default:
		return 0;
	}
	
	if (rn8302_Read(nReg, &nData, 4) == 0)
		return 0;
	
	if (nData > POW2_27)
		nData -= POW2_28;
	fData *= (float)nData;
	
	return fData;
}


//------------------------------------------------------------------------
//名	称: rn8302_GetPower()
//设	计: 
//输	入: nPhase - 相位值
//输	出: -
//返	回: 电压有效值
//功	能: 获取当前电压
//------------------------------------------------------------------------
float rn8302_GetPower(int nPhase, int nType)
{
	s32 nData;
	u16 nReg;
	float fData;

	nReg = RN8302_REG_PA + (4 * nType) + nPhase;
	if (rn8302_Read(nReg, &nData, 4) == 0)
		return 0;
	
	fData = (float)nData * rn_x8302.kp;
	
	return fData;
}



//------------------------------------------------------------------------
//名	称:  rn8302_GetPFV ()
//设	计: 
//输	入:  nPhase - 相位值
//输	出:  -
//返	回:  功率因数
//功	能:  获取功率因数
//------------------------------------------------------------------------
float rn8302_GetPFV(int nPhase)
{
	s32 nData = 0;
	float fData;

	if (rn8302_Read(RN8302_REG_PfA + nPhase, &nData, 3) == 0)
		return 0;
	
	if (nData > POW2_23)
		nData -= POW2_24;
	fData = (float)nData / POW2_23;
	
	return fData;
}

//------------------------------------------------------------------------
//名	称:  rn8302_GetPAG ()
//设	计: 
//输	入: nPhase - 相位值
//输	出: -
//返	回: 电流于电压的夹角
//功	能: 获取角度
//------------------------------------------------------------------------
float rn8302_GetPAG(int nPhase, int nType) 
{
	u32 nData = 0;
	u16 nReg;
	float fData;

	if (nType)
		nReg = RN8302_REG_YIA + nPhase;
	else
		nReg = RN8302_REG_YUA + nPhase;
	
	if (rn8302_Read(nReg, &nData, 3) == 0)
		return 0;
	
	fData = (float)nData * 360.0f / POW2_24;
	return fData;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
//Bit0-3 分别表示A B  C  合相的有功功率的方向	0 表示为正	1 表示为负 
//Bit4-7 分别表示A B  C  合相的无功功率的方向 	0 表示为正	1 表示为负 
u16 rn8302_GetPowerDir()
{
	u16 nData;

	if (rn8302_Read(RN8302_REG_PQSign, &nData, 2) == 0)
		return 0;
	
	return nData;
}

//------------------------------------------------------------------------
//名	称: BSP_RN8302B_UIP_gainCalibration () 
//设	计: 
//输	入: -
//输	出: -
//返	回: -
//功	能: 校正U，I，P增益
//------------------------------------------------------------------------
void rn8302_Cali(rn8302_cali_t *pCali, float fUn, float fIb, int nIs3P3)
{
	int i;
	u32 nData;
	s16 phv;
	s8 phv8;
	float fData, fU;

	memset(pCali, 0, sizeof(rn8302_cali_t));
	pCali->UPhs[0] = 0x80;
	pCali->UPhs[1] = 0x80;
	pCali->UPhs[2] = 0x80;
	pCali->IPhs[0][0] = 0x80;
	pCali->IPhs[1][0] = 0x80;
	pCali->IPhs[2][0] = 0x80;

	pCali->mode = nIs3P3;
	if (fIb == 20.0f)
	{
		pCali->HFConst = 3600;
		pCali->EC = 400;
	}
	else
	{
		pCali->HFConst = 4500;
		pCali->EC = 6400;
	}
	pCali->kv = fUn / (0.1375f * (float)POW2_27);
	pCali->ki = fIb / (0.05f * (float)POW2_27);
	pCali->kp = (fUn * fIb) / (0.1375f * 0.05f * (float)POW2_31);
	rn8302_Reset(pCali);

	for (i = 0; i < 3; i++)
	{
		//电压通道校正
		fData = rn8302_GetUI(i, 0);
		fData = fUn / fData - 1.0f;
		phv = fData * POW2_15 + 0.5f;
		pCali->Ugain[i] = phv;
	
		//电流通道校正
		fData = rn8302_GetUI(i, 1);
		fData = fIb / fData - 1.0f;
		phv = fData * POW2_15 + 0.5f;
		pCali->Igain[i] = phv;

		//电压相角
		if (i)
		{
			fU = rn8302_GetPAG(i, 0);
			fData = (fU - 120.0f * i) / 0.017578f;
			phv8 = 0x80 - (s8)(fData + 0.5f);
			pCali->UPhs[i] = phv8;
		}
	}

	rn8302_Reset(pCali);

	//电流相角
	for (i = 0; i < 3; i++)
	{
		fU = rn8302_GetPAG(i, 0);
		fData = rn8302_GetPAG(i, 1);
		fData = (fData - fU - 60.0f) / 0.017578f;
		phv8 = 0x80 - (s8)(fData + 0.5f);
		pCali->IPhs[i][0] = phv8;
	}

	rn8302_Reset(pCali);

	nData = 0;
	rn8302_Read(RN8302_REG_CheckSum1, &nData, 3);

	pCali->crc = nData;
}

u16 rn8302_GetEConst()
{
	
	return rn_x8302.ec;
}


#endif





