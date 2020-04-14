#if ATT7022_ENABLE
#include <drivers/att7022.h>


//Private Defines
#define ATT7022_SAMPLEPOINT   		128
#define ATT7022_DATA_MASK			0x00FFFFFF
#define ATT7022_DATA_DEC			8192.0f

#define ATT7022_SPI_SPEED			200000



//Private Variables
static att7022_t att_x7022;


//Private Macros







//Internal Functions
#if ATT7022_DEBUG_ENABLE
#define ATT7022_DBGOUT				dbg_printf
#else
#define ATT7022_DBGOUT(...)
#endif



static void att7022_SpiGet()
{
	att7022_t *p = &att_x7022;
	
	p->spi = spi_Open(ATT7022_COMID, OS_TMO_FOREVER);
	spi_Config(p->spi, SPI_SCKIDLE_LOW, SPI_LATCH_2EDGE, ATT7022_SPI_SPEED);
#if SPI_SEL_ENABLE
	spi_CsSel(p->spi, ATT7022_CSID);
#endif
}


static sys_res att7022_Write(u32 nReg, u32 nData)
{
	u32 nCrc1, nCrc2, nTemp;
	att7022_t *p = &att_x7022;

	att7022_SpiGet();

	//写数据寄存器
	nData &= ATT7022_DATA_MASK;
	invert(&nData, 3);
	nTemp = nReg | 0x80 | (nData << 8);
	spi_Send(p->spi, &nTemp, 4);
	os_thd_slp1tick();
	//读校验1寄存器
	nTemp = ATT7022_REG_WSPIData1;
	spi_TransThenRecv(p->spi, &nTemp, 1, &nCrc1, 3);
	nCrc1 &= ATT7022_DATA_MASK;
	//读校验2寄存器
#if 0
	nTemp = ATT7022_REG_WSPIData2;
	spi_TransThenRecv(p->spi, &nTemp, 1, &nCrc2, 3);
	nCrc2 &= ATT7022_DATA_MASK;
#else
	nCrc2 = nCrc1;
#endif

	spi_Close(p->spi);

	if ((nData != nCrc1) || (nData != nCrc2))
	{
		ATT7022_DBGOUT("<ATT7022>WriteReg %02X Err %x %x %x", nReg, nData, nCrc1, nCrc2);
		return SYS_R_ERR;
	}
	return SYS_R_OK;
}

static u32 att7022_UgainCalibration(int nPhase)
{
	float urms, k, gain = 0;

	//读取该相电压值 
	urms = att7022_Read(ATT7022_REG_URmsA + nPhase);
	if (urms)
	{
		 //计算实际测出的工程量
		urms = urms / SYS_KVALUE;
		 
		//此处以UCALI_CONST为标准计算Ugain 
		k = UCALI_CONST / urms - 1;
		gain = k * POW2_23;
		if (k < 0)
			gain = POW2_24 + gain;
	}
	
	return (u32)(gain + 0.5f);
} 

static u32 att7022_IgainCalibration(int nPhase, float fIb)
{
	float irms, k, gain = 0;

	//读取该相电压值 
	irms = att7022_Read(ATT7022_REG_IRmsA + nPhase);
	if (irms)
	{
		irms = irms / SYS_KVALUE; 
		
		//此处以ICALI_CONST为标准计算Igain	
		k = (fIb * ICALI_MUL) / irms - 1;
		gain = k * POW2_23;
		if (k < 0)
			gain = POW2_24 + gain;
	}
	
	return (u32)(gain + 0.5f);
}

static u32 att7022_PgainCalibration(int nPhase, float fIb)
{
	float pvalue, err, pcali, pgain = 0;
	att7022_t *p = &att_x7022;

	//读出测量到的功率
	pvalue = att7022_Read(ATT7022_REG_PA + nPhase);	
	if (pvalue > POW2_23)
		pvalue -= POW2_24;
	
	//转换成工程量
	pvalue = (pvalue / 256.0f) * (3200.0f / (float)p->ec);
	
	//误差计算
	pcali = UCALI_CONST * fIb;
	err = (pvalue - pcali) / pcali;
	if (err)
	{
		err = -err / (1 + err);
		pgain = err * POW2_23;
		if (err < 0 )
			pgain = POW2_24 + pgain;			//计算Pgain
	}
	
	return (u32)(pgain + 0.5f);
}

static u32 att7022_FPhaseCaliData(int nPhase, float fIb)
{
	float phase_v = 0, att7022_pvalue, seta, err, pcali_value = 0;
	u32 nTmp = 0;
	att7022_t *p = &att_x7022;

	pcali_value = UCALI_CONST * fIb * 0.5f;					//载入校正点的有功功率常数
	nTmp = att7022_Read(ATT7022_REG_PA + nPhase);	//读取有功功率值
 	if (nTmp > POW2_23)
 		nTmp -= POW2_24;
	
	att7022_pvalue = ((float)nTmp / 256.0f) * (3200.0f / (float)p->ec); //转换成工程量
	err = (att7022_pvalue - pcali_value) / pcali_value; //误差计算
	if (err)
	{
		seta = acosf((1 + err) * 0.5f);
		seta -= PI / 3.0f;
		phase_v = seta * POW2_23;
		if (seta < 0)
			phase_v = POW2_24 + phase_v;
	}
	
	return (u32)(phase_v + 0.5f);
}







//External Functions
void att7022_Init()
{
	t_gpio_def *pDef;

	memset(&att_x7022, 0, sizeof(att7022_t));
	for (pDef = tbl_bspAtt7022[0]; pDef < tbl_bspAtt7022[1]; pDef++)
	{
		sys_GpioConf(pDef);
	}
}


sys_res att7022_Reset(att7022_cali_t *pCali)
{
	u32 nTemp;
	int i;
	att7022_t *p = &att_x7022;

	//复位ATT7022B
	sys_GpioSet(gpio_node(tbl_bspAtt7022, 0), 1);
	sys_Delay(3000);				//delay 30us
	sys_GpioSet(gpio_node(tbl_bspAtt7022, 0), 0);
	os_thd_slp1tick();

	switch (pCali->HFConst)
	{
	case 35:
		p->ec = 800;
		break;
	case 16:
		p->ec = 8000;
		break;
	default:
		p->ec = 3200;
		break;
	}

	for (i = 0; i < 3; i++)
	{
		if (att7022_Write(ATT7022_CALIDATA_WEN, 0) == SYS_R_OK)
			break;
	}
	if (i >= 3)
		return SYS_R_ERR;

	att7022_Write(ATT7022_CALIDATA_CLR, 0);

	att7022_Write(ATT7022_REG_UADCPga, 0);			//电压通道ADC增益设置为1
	att7022_Write(ATT7022_REG_HFConst, pCali->HFConst);	//设置HFConst
	nTemp = IB_VO * ISTART_RATIO * CONST_G * POW2_23;
	att7022_Write(ATT7022_REG_Istartup, nTemp);		//启动电流设置
	att7022_Write(ATT7022_REG_EnUAngle, 0x003584);	//使能电压夹角测量
	att7022_Write(ATT7022_REG_EnDtIorder, 0x005678);	//使能相序检测
	att7022_Write(ATT7022_REG_EAddMode, 0);			//合相能量累加模式	
	att7022_Write(ATT7022_REG_GCtrlT7Adc, 0); 		//关闭温度和ADC7测量 	
	att7022_Write(ATT7022_REG_EnlineFreq, 0x007812);	//基波/谐波测量功能
	att7022_Write(ATT7022_REG_EnHarmonic, 0x0055AA);	//基波/谐波测量功能

	//写入功率增益
	for (i = 0; i < 3; i++)
	{
		att7022_Write(ATT7022_REG_PgainA0 + i, pCali->Pgain0[i]);
		att7022_Write(ATT7022_REG_PgainA1 + i, pCali->Pgain1[i]);
	}

	//写入电压增益
	for (i = 0; i < 3; i++)
	{
		att7022_Write(ATT7022_REG_UgainA + i, pCali->Ugain[i]);
	}

	//写入电流增益
	for (i = 0; i < 3; i++)
	{
		att7022_Write(ATT7022_REG_IgainA + i, pCali->Igain[i]);
	}

	//写入相位角增益
	for (i = 0; i < 5; i++)
	{
		att7022_Write(ATT7022_REG_PhsregA0 + i, pCali->PhsregA[i]);
		att7022_Write(ATT7022_REG_PhsregB0 + i, pCali->PhsregB[i]);
		att7022_Write(ATT7022_REG_PhsregC0 + i, pCali->PhsregC[i]);
	}

	att7022_Write(ATT7022_CALIDATA_WEN, 1);

	os_thd_sleep(1600);
	
	return SYS_R_OK;
}

u32 att7022_Read(u32 nReg)
{
	u32 nData, nCrc;
	att7022_t *p = &att_x7022;

	att7022_SpiGet();

	//读数据寄存器
	spi_TransThenRecv(p->spi, &nReg, 1, &nData, 3);
	os_thd_slp1tick();
	invert(&nData, 3);
	nData &= ATT7022_DATA_MASK;
	
	//读校验寄存器	
	nReg = ATT7022_REG_RSPIData;
	spi_TransThenRecv(p->spi, &nReg, 1, &nCrc, 3);
	invert(&nCrc, 3);
	nCrc &= ATT7022_DATA_MASK;

	spi_Close(p->spi);

	if (nData != nCrc)
	{
		ATT7022_DBGOUT("<ATT7022>ReadReg %02X Err %x %x", nReg, nData, nCrc);
		nData = 0;
	}
	
	return nData;
}


const u8 tbl_att7022RegEnergy[6] = {
	ATT7022_REG_PosEpA2,
	ATT7022_REG_NegEpA2,
	ATT7022_REG_PosEqA2,
	ATT7022_REG_NegEqA2,
	ATT7022_REG_NegEqA2,
	ATT7022_REG_PosEqA2,
};

u32 att7022_GetEnergy(int nPhase, int nType)
{

	return att7022_Read(tbl_att7022RegEnergy[nType] + nPhase);
}

//------------------------------------------------------------------------
//名	称:  att7022_GetFreq ()
//设	计: 
//输	入:  -
//输	出:  -
//返	回:  频率值
//功	能:  获取频率值
//------------------------------------------------------------------------
float att7022_GetFreq()
{
	s32 nData;

	nData = att7022_Read(ATT7022_REG_Freq);
	if (nData != (-1))
		return (nData / ATT7022_DATA_DEC);
	
	return 0;
}


//------------------------------------------------------------------------
//名	称: att7022_GetVoltage ()
//设	计: 
//输	入: nPhase - 相位值
//输	出: -
//返	回: 电压有效值
//功	能: 获取当前电压
//------------------------------------------------------------------------
float att7022_GetVoltage(int nPhase)
{
	u32 nVoltage;
	float fResult;

	nVoltage = att7022_Read(ATT7022_REG_URmsA + nPhase);
	fResult = (float)nVoltage / ATT7022_DATA_DEC;
	
	return fResult;
}



//------------------------------------------------------------------------
//名	称: att7022_GetCurrent ()
//设	计: 
//输	入: nPhase - 相位值
//输	出: -
//返	回: 电流有效值
//功	能: 获取当前电流
//------------------------------------------------------------------------
float att7022_GetCurrent(int nPhase)
{
	u32 nCurrent;
	float fResult;

	nCurrent = att7022_Read(ATT7022_REG_IRmsA + nPhase);
	fResult = (float)nCurrent / (ICALI_MUL * ATT7022_DATA_DEC);
	return fResult;
}


float att7022_GetPower(u32 nReg, int nPhase)
{
	float fResult, eck;
	
	eck = 3200.0f / (float)att_x7022.ec; //脉冲输出系数
	fResult = att7022_Read(nReg + nPhase);
	
	if (fResult > POW2_23)
		fResult -= POW2_24;
	
	if (nPhase < 3)
	{
		//读取分相功率
		fResult = fResult * eck / 256000.0f; //转换成工程量
	}
	else
	{
		//读取合相功率
		fResult = fResult * eck / 64000.0f; //转换成工程量
	}
	
	return fResult;
}

u32 att7022_GetFlag() 
{
	
	return att7022_Read(ATT7022_REG_SFlag); //读取状态寄存器 
}
//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
u32 att7022_GetPowerDir()
{

	//Bit0-3 分别表示A B  C  合相的有功功率的方向	0 表示为正	1 表示为负 
	//Bit4-7 分别表示A B  C  合相的无功功率的方向 	0 表示为正	1 表示为负 
	return att7022_Read(ATT7022_REG_PFlag); //读取功率方向寄存器
}


//------------------------------------------------------------------------
//名	称:  att7022_GetPFV ()
//设	计: 
//输	入:  nPhase - 相位值
//输	出:  -
//返	回:  功率因数
//功	能:  获取功率因数
//------------------------------------------------------------------------
float att7022_GetPFV(int nPhase)
{
	int nData = 0;
	float f_data = 0.0f;

	nData = att7022_Read(ATT7022_REG_PfA + nPhase);
	if (nData > POW2_23)
		nData -= POW2_24;
	
	f_data = (float)nData / POW2_23;
	
	return f_data;
}
//------------------------------------------------------------------------
//名	称:  att7022_GetPAG ()
//设	计: 
//输	入: nPhase - 相位值
//输	出: -
//返	回: 电流于电压的夹角
//功	能: 获取角度
//------------------------------------------------------------------------
float att7022_GetPAG(int nPhase) 
{
	float sita;
	
	sita = att7022_Read(ATT7022_REG_PgA + nPhase);
	if (sita > POW2_23)
		sita -= POW2_24;
	
	return (sita * 360 / POW2_23 / PI);
}

//-------------------------------------------------------------------------
//电压相角
//-------------------------------------------------------------------------
float att7022_GetPVAG(int nPhase) 
{

	return ((float)att7022_Read(ATT7022_REG_YUaUb + nPhase) / 8192.0f);
}

u32 att7022_GetQuanrant(int Phase) 
{
	u32 pflag;
	u32 p_direction, q_direction;

	pflag = att7022_Read(ATT7022_REG_PFlag); //先读取功率方向寄存器(正向为0,负向为1)
	
	p_direction = ((pflag >> Phase) & 0x00000001);
	q_direction = ((pflag >> (Phase + 4)) & 0x00000001);
	if (p_direction)
	{
		if (q_direction)
		{
			//P- Q-
			return 3; //ATT7022B  3
		}
		else
		{
			//P- Q+
			return 2; //ATT7022B  2
		}
	}
	else
	{
		if (q_direction)
		{
			//P+ Q-
			return 4; //ATT7022B  4
		}
		else
		{
			//P+ Q+
			return 1; //ATT7022B  1
		}
	}
}


/**************************
 * 名称:			att7022_GetHarmonic
 * 描述:			读取各通道谐波
 * 输入参数:		Ch(0--B)  分别对应下面各通道，采样率3.2k
 Ua\Ia\Ub\Ib\Uc\Ic\In\Ua+Ia\Ub+Ib\Uc+Ic\Ua+Ub+Uc\Ia+Ib+Ic
 * 返回值:			
 * 特殊说明:		RVMDK 3.01a
***************************/
sys_res att7022_GetHarmonic(int Ch, s16 *pbuf)
{
	int i;
	u32 nData, nReg = 0x7F;
	att7022_t *p = &att_x7022;

	//开启采样功能
	nData=0xCCCCC0|Ch;
	att7022_Write(0xC0, nData);//启动波形数据缓存
	//等待采样数据完成
#if 0
	os_thd_sleep(200);
#else
	for (i = 0; i < 20; i++)
	{   //一般重复3次就行了?
		if(att7022_Read(0x7E) >= 240)//下一个写数据的位置
			break;
		
		os_thd_sleep(20);
	}
#endif
	if (i < 20)
	{
		//设置用户读取指针的起始位置
		att7022_Write(0xC1, 0);
		
		att7022_SpiGet();
		for (i = 0; i < 240; i++)
		{
			spi_TransThenRecv(p->spi, &nReg, 1, &nData, 3);//读取数据
			invert(&nData, 3);
			nData &= ATT7022_DATA_MASK;
			if (i < ATT7022_SAMPLEPOINT)
				pbuf[i] = (s16)(nData >> 8);
		}
		spi_Close(p->spi);

		return SYS_R_OK;
	}
	
	return SYS_R_ERR;
}

int att7022_GetSig()
{

	return sys_GpioGet(gpio_node(tbl_bspAtt7022, 1));
}


u16 att7022_GetEConst()
{
	
	return att_x7022.ec;
}


//------------------------------------------------------------------------
//名	称: BSP_ATT7022B_UIP_gainCalibration () 
//设	计: 
//输	入: -
//输	出: -
//返	回: -
//功	能: 校正U，I，P增益
//------------------------------------------------------------------------
void att7022_CaliUIP(att7022_cali_t *pCali)
{
	int i, j;
	u32 phv;

	for (i = 0; i < 3; i++)
	{
		//电压通道校正
		phv = 0;
		
		for (j = 0; j < 8; j++)
		{
			phv += att7022_UgainCalibration(PHASE_A + i);
			os_thd_sleep(250);
		}
		phv /= 8;
		pCali->Ugain[i] = phv;
		
		//电流通道校正
		phv = 0;
		for (j = 0; j < 8; j++)
		{
			phv += att7022_IgainCalibration(PHASE_A + i, 1.5f);
			os_thd_sleep(250);
		}
		phv /= 8;
		pCali->Igain[i] = phv;   
		
		//功率校正
		phv = 0;
		for (j = 0; j < 8; j++)
		{
			phv += att7022_PgainCalibration(PHASE_A + i, 1.5f);
			os_thd_sleep(250);
		}
		phv /= 8;
		pCali->Pgain0[i] = phv;
		pCali->Pgain1[i] = phv;
	}
}
//------------------------------------------------------------------------
//名	称: BSP_ATT7022B_Phase_gainCalibration () 
//设	计: 
//输	入: -
//输	出: -
//返	回: -
//功	能: 相位角校正,功率因数为  0.5L  的条件下
//------------------------------------------------------------------------
void att7022_CaliPhase(att7022_cali_t *pCali)
{
	int i;
	u32 phv;

	phv = 0;
	for (i = 0; i < 8; i++)
	{
		phv += att7022_FPhaseCaliData(PHASE_A, 1.5f); //1.5A处校正
		os_thd_sleep(250);
	}
	
	phv /= 8;
	for (i = 0; i < 5; i++)
	{
		pCali->PhsregA[i] = phv;   
	}

	phv = 0;
	for (i = 0; i < 8; i++)
	{
		phv += att7022_FPhaseCaliData(PHASE_B, 1.5f); //1.5A处校正
		os_thd_sleep(250);
	}
	
	phv /= 8;
	for (i = 0; i < 5; i++)
	{
		pCali->PhsregB[i] = phv;  
	}

	phv = 0;
	for (i = 0; i < 8; i++)
	{
		phv += att7022_FPhaseCaliData(PHASE_C, 1.5f); //1.5A处校正
		os_thd_sleep(250);
	}
	
	phv /= 8;
	for (i = 0; i < 5; i++)
	{
		pCali->PhsregC[i] = phv;  
	}
}



#endif

