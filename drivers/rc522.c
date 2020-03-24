#if MIFARE_ENABLE
#include <drivers/rc522.h>

//Private Defines
#define RC522_RX_SIZE			32

//MF522 FIFO长度定义
#define DEF_FIFO_LENGTH			64                 //FIFO size=64byte

//MF522命令字
#define PCD_IDLE				0x00               //取消当前命令
#define PCD_AUTHENT				0x0E               //验证密钥
#define PCD_RECEIVE				0x08               //接收数据
#define PCD_TRANSMIT			0x04               //发送数据
#define PCD_TRANSCEIVE			0x0C               //发送并接收数据
#define PCD_RESETPHASE			0x0F               //复位
#define PCD_CALCCRC				0x03               //CRC计算

//Mifare_One卡片命令字
#define PICC_REQIDL				0x26               //寻天线区内未进入休眠状态
#define PICC_REQALL				0x52               //寻天线区内全部卡
#define PICC_ANTICOLL1			0x93               //防冲撞
#define PICC_ANTICOLL2			0x95               //防冲撞
#define PICC_AUTHENT1A			0x60               //验证A密钥
#define PICC_AUTHENT1B			0x61               //验证B密钥
#define PICC_READ				0x30               //读块
#define PICC_WRITE				0xA0               //写块
#define PICC_DECREMENT			0xC0               //扣款
#define PICC_INCREMENT			0xC1               //充值
#define PICC_RESTORE			0xC2               //调块数据到缓冲区
#define PICC_TRANSFER			0xB0               //保存缓冲区中数据
#define PICC_HALT				0x50               //休眠

//Mifare_Pro卡片命令字
#define PICC_RESET				0xE0               //复位

//MF522寄存器定义
//PAGE 0
#define RFU00					0x00    
#define CommandReg				0x01    
#define ComIEnReg				0x02    
#define DivlEnReg				0x03    
#define ComIrqReg				0x04    
#define DivIrqReg				0x05
#define ErrorReg				0x06    
#define Status1Reg				0x07    
#define Status2Reg				0x08    
#define FIFODataReg				0x09
#define FIFOLevelReg			0x0A
#define WaterLevelReg			0x0B
#define ControlReg				0x0C
#define BitFramingReg			0x0D
#define CollReg					0x0E
#define RFU0F					0x0F
//PAGE 1     
#define RFU10					0x10
#define ModeReg					0x11
#define TxModeReg				0x12
#define RxModeReg				0x13
#define TxControlReg			0x14
#define TxAutoReg				0x15
#define TxSelReg				0x16
#define RxSelReg				0x17
#define RxThresholdReg			0x18
#define DemodReg				0x19
#define RFU1A					0x1A
#define RFU1B					0x1B
#define MifareReg				0x1C
#define RFU1D					0x1D
#define RFU1E					0x1E
#define SerialSpeedReg			0x1F
//PAGE 2    
#define RFU20					0x20  
#define CRCResultRegM			0x21
#define CRCResultRegL			0x22
#define RFU23					0x23
#define ModWidthReg				0x24
#define RFU25					0x25
#define RFCfgReg				0x26
#define GsNReg					0x27
#define CWGsCfgReg				0x28
#define ModGsCfgReg				0x29
#define TModeReg				0x2A
#define TPrescalerReg			0x2B
#define TReloadRegH				0x2C
#define TReloadRegL				0x2D
#define TCounterValueRegH		0x2E
#define TCounterValueRegL		0x2F
//PAGE 3      
#define RFU30					0x30
#define TestSel1Reg				0x31
#define TestSel2Reg				0x32
#define TestPinEnReg			0x33
#define TestPinValueReg			0x34
#define TestBusReg				0x35
#define AutoTestReg				0x36
#define VersionReg				0x37
#define AnalogTestReg			0x38
#define TestDAC1Reg				0x39  
#define TestDAC2Reg				0x3A   
#define TestADCReg				0x3B   
#define RFU3C					0x3C   
#define RFU3D					0x3D   
#define RFU3E					0x3E   
#define RFU3F					0x3F



//Private Macros
#define mf_Rst(x)					sys_GpioSet(&tbl_bspMifare.tbl[0], x)
#define mf_Nss(x)					sys_GpioSet(&tbl_bspMifare.tbl[1], x)




//Private Functions
static int rc522_RegRead(spi_t *p, int nReg)
{
	int res = 0;

	mf_Nss(0);
	spi_Transce(p, ((nReg << 1) & 0x7E) | 0x80, &res, 1);
	mf_Nss(1);
	
	return res;
}

static void rc522_RegWrite(spi_t *p, int nReg, int nData)
{

	nData = (nData << 8) | (nReg << 1) & 0x7E;
	
	mf_Nss(0);
	spi_Send(p, &nData, 2);
	mf_Nss(1);
}

static void rc522_SetRegBit(spi_t *p, int nReg, int nMask)
{
	int nTemp;

	nTemp = rc522_RegRead(p, nReg);
	rc522_RegWrite(p, nReg, nTemp | nMask);
}

static void rc522_ClrRegBit(spi_t *p, int nReg, int nMask)
{
	int nTemp;

	nTemp = rc522_RegRead(p, nReg);
	rc522_RegWrite(p, nReg, nTemp & ~nMask);
}

/////////////////////////////////////////////////////////////////////
//用MF522计算CRC16函数
/////////////////////////////////////////////////////////////////////
static void rc522_CRC16(spi_t *p, u8 *pIn, size_t nLen, u8 *pOut)
{
	int i, n;

	rc522_ClrRegBit(p, DivIrqReg, 0x04);
	rc522_RegWrite(p, CommandReg, PCD_IDLE);
	rc522_SetRegBit(p, FIFOLevelReg, 0x80);
	for (i = 0; i < nLen; i++)
	{
		rc522_RegWrite(p, FIFODataReg, pIn[i]);
	}
	
	rc522_RegWrite(p, CommandReg, PCD_CALCCRC);
	i = 0xFF;
	do 
	{
	    n = rc522_RegRead(p, DivIrqReg);
	    i--;
	}
	while (i && ((n & 0x04) == 0));
	
	pOut[0] = rc522_RegRead(p, CRCResultRegL);
	pOut[1] = rc522_RegRead(p, CRCResultRegM);
}

//--------------------------------------------------------------------
//函数名称：PcdComMF522()
//函数功能：通过RC522和ISO14443卡通讯
//函数说明：Command[IN]：RC522命令字
//        pIndata[IN]：通过RC522发送到卡片的数据
//        InLenByte[IN]：发送数据的字节长度
//        pOutData[OUT]：接收到的卡片返回数据
//        *pOutLenBit[OUT]：返回数据的位长度
//返 回 值：成功返回 MI_OK
//--------------------------------------------------------------------
static sys_res rc522_PcdCom(spi_t *p, int cmd, u8 *pIn, size_t nInLen, u8 *pOut, size_t *pOutLen)
{
	sys_res res = SYS_R_ERR;
	u8 irqEn, waitFor, lastBits, n = 0;
	int i, nErr;

	switch (cmd)
	{
	case PCD_AUTHENT:
		irqEn   = 0x12;
		waitFor = 0x10;
		break;
	case PCD_TRANSCEIVE:
		irqEn   = 0x77;
		waitFor = 0x30;
		break;
	default:
		irqEn   = 0;
		waitFor = 0;
		break;
	}
	
	rc522_RegWrite(p, ComIEnReg, irqEn | 0x80);
	rc522_ClrRegBit(p, ComIrqReg, 0x80);
	rc522_RegWrite(p, CommandReg, PCD_IDLE);
	rc522_SetRegBit(p, FIFOLevelReg, 0x80);
	for (i = 0; i < nInLen; i++)
	{
		rc522_RegWrite(p, FIFODataReg, pIn[i]);
	}
	rc522_RegWrite(p, CommandReg, cmd);
	
	if (cmd == PCD_TRANSCEIVE)
		rc522_SetRegBit(p, BitFramingReg, 0x80);
	
	//600根据时钟频率调整，操作M1卡最大等待时间25ms
	for (i = 600; i && ((n & 1) == 0) && ((n & waitFor) == 0); i--)
	{
		n = rc522_RegRead(p, ComIrqReg);
	}
	
	rc522_ClrRegBit(p, BitFramingReg, 0x80);
	nErr = rc522_RegRead(p, ErrorReg);
	
	if (i && ((nErr & 0x1B) == 0))
	{
		res = SYS_R_OK;
		if (n & irqEn & 1)
			res = SYS_R_EMPTY;
		
		if (cmd == PCD_TRANSCEIVE)
		{
			n = rc522_RegRead(p, FIFOLevelReg);
			lastBits = rc522_RegRead(p, ControlReg) & 0x07;
			
			if (lastBits)
				*pOutLen = (n - 1) * 8 + lastBits;
			else
				*pOutLen = n * 8;
			
			if (n == 0)
				n = 1;
			else if (n > RC522_RX_SIZE)
				n = RC522_RX_SIZE;
			
			for (i = 0; i < n; i++)
			{
				pOut[i] = rc522_RegRead(p, FIFODataReg);
			}
		}
	}
	
	rc522_SetRegBit(p, ControlReg, 0x80);           //stop timer now
	rc522_RegWrite(p, CommandReg, PCD_IDLE); 
	
	return res;
}

static sys_res rc522_Reset(mifare mf)
{

	mf_Rst(1);
	os_thd_slp1tick();
	mf_Rst(0);
	os_thd_slp1tick();
	mf_Rst(1);
	os_thd_slp1tick();

	rc522_RegWrite(mf->p, CommandReg, PCD_RESETPHASE);
	os_thd_slp1tick();

	//和Mifare卡通讯，CRC初始值0x6363
	rc522_RegWrite(mf->p, ModeReg, 0x3D);
	rc522_RegWrite(mf->p, TReloadRegL, 30);
	rc522_RegWrite(mf->p, TReloadRegH, 0);
	rc522_RegWrite(mf->p, TModeReg, 0x8D);
	rc522_RegWrite(mf->p, TPrescalerReg, 0x3E);
	
	//Ask Enable
	rc522_RegWrite(mf->p, TxAutoReg, 0x40);
	
	//接收灵敏度最大
	rc522_RegWrite(mf->p, RFCfgReg, 0x70);
	
	//激活收发天线
	rc522_RegWrite(mf->p, TxControlReg, 0x83);
	
	if ((rc522_RegRead(mf->p, TxControlReg) & 0x03) == 0x03)
		return SYS_R_OK;
	
	return SYS_R_ERR;
}

static sys_res mf_Transce(mifare mf, u8 *pBuf, size_t nIn, size_t *pOutLen)
{
	sys_res res;
	size_t nLen;

	if (mf->ste != MF_S_ACTIVE)
		return SYS_R_BUSY;
	
	mf->ste = MF_S_BUSY;
	pBuf[0] = 0x02 | mf->block++;
	rc522_CRC16(mf->p, pBuf, nIn, pBuf + nIn);
	res = rc522_PcdCom(mf->p, PCD_TRANSCEIVE, pBuf, nIn + 2, pBuf, &nLen);
	
	if (res == SYS_R_OK)
		*pOutLen = nLen >> 3;
	else
		*pOutLen = 0;
	
	mf->ste = MF_S_ACTIVE;
	
	return res;
}


//--------------------------------------------------------------------
//External Functions
//--------------------------------------------------------------------
void mf_InitGpio()
{
	int i;

	for (i = 0; i < tbl_bspMifare.qty; i++)
	{
		sys_GpioConf(&tbl_bspMifare.tbl[i]);
	}
}

//--------------------------------------------------------------------
//函数名称：rc522_PcdAntennaOn()
//函数功能：开启天线
//函数说明：每次开启或关闭天线发射之间至少有1ms的间隔
//返 回 值：无
//--------------------------------------------------------------------
void mf_Init(mifare mf)
{

	memset(mf, 0, sizeof(mifare));
}

//--------------------------------------------------------------------
//函数名称：rc522_PcdAntennaOn()
//函数功能：开启天线
//函数说明：每次开启或关闭天线发射之间至少有1ms的间隔
//返 回 值：无
//--------------------------------------------------------------------
void mf_Release(mifare mf)
{

	if (mf->p != NULL)
		spi_Close(mf->p);
	
	memset(mf, 0, sizeof(mifare));
}

//--------------------------------------------------------------------
//函数名称：rc522_PcdAntennaOn()
//函数功能：开启天线
//函数说明：每次开启或关闭天线发射之间至少有1ms的间隔
//返 回 值：无
//--------------------------------------------------------------------
sys_res mf_Get(mifare mf, int nId, size_t nTmo)
{

	if (mf->ste != MF_S_IDEL)
		return SYS_R_BUSY;
	
	mf_Release(mf);
	
	mf->p = spi_Open(nId, nTmo);
	if (mf->p == NULL)
		return SYS_R_TMO;
	
	spi_Config(mf->p, SPI_SCKIDLE_HIGH, SPI_LATCH_2EDGE, RC522_SPI_SPEED);
	
	if (rc522_Reset(mf) == SYS_R_OK)
	{
		mf->ste = MF_S_ACTIVE;
		mf->block = 0;
		
		return SYS_R_OK;
	}
	
	mf_Release(mf);
	
	return SYS_R_ERR;
}

//--------------------------------------------------------------------
//函数名称：rc522_PcdAntennaOn()
//函数功能：开启天线
//函数说明：每次开启或关闭天线发射之间至少有1ms的间隔
//返 回 值：无
//--------------------------------------------------------------------
void mf_AntennaOn(mifare mf)
{
	int i;

	i = rc522_RegRead(mf->p, TxControlReg);
	if ((i & 0x03) == 0)
		rc522_RegWrite(mf->p, TxControlReg, i | 0x03);
}

//--------------------------------------------------------------------
//函数名称：PcdAntennaOff()
//函数功能：关闭天线
//函数说明：每次开启或关闭天线发射之间至少有1ms的间隔
//返 回 值：无
//--------------------------------------------------------------------
void mf_AntennaOff(mifare mf)
{

	rc522_ClrRegBit(mf->p, TxControlReg, 0x03);
}


//--------------------------------------------------------------------
//函数名称：PcdRequest()
//函数功能：寻卡
//函数说明：nReg[IN]:寻卡方式
//             0x52 = 寻感应区内所有符合14443A标准的卡
//             0x26 = 寻未进入休眠状态的卡
//       pTagType[OUT]：卡片类型代码
//             0x4400 = Mifare_UltraLight
//             0x0400 = Mifare_One(S50)
//             0x0200 = Mifare_One(S70)
//             0x0800 = Mifare_Pro(X)
//             0x4403 = Mifare_DESFire
//返 回 值：成功返回MI_OK
//--------------------------------------------------------------------
sys_res mf_FindCard(mifare mf, int nOp)
{
	sys_res res = SYS_R_ERR;
	size_t nLen;
	u8 aBuf[RC522_RX_SIZE]; 

	if (mf->ste != MF_S_ACTIVE)
		return SYS_R_BUSY;
	
	mf->ste = MF_S_BUSY;
	rc522_ClrRegBit(mf->p, Status2Reg, 0x08);
	rc522_RegWrite(mf->p, BitFramingReg, 0x07);
	rc522_SetRegBit(mf->p, TxControlReg, 0x03);
	aBuf[0] = nOp;
	res = rc522_PcdCom(mf->p, PCD_TRANSCEIVE, aBuf, 1, aBuf, &nLen);

	if ((res == SYS_R_OK) && (nLen == 0x10))
		memcpy(&mf->ctype, aBuf, 2);
	else
		res = SYS_R_ERR;
	
	mf->ste = MF_S_ACTIVE;
	
	return res;
}

//--------------------------------------------------------------------
//函数名称：PcdSelect()
//函数功能：选定卡片
//函数说明：pSnr[IN]:卡片序列号，4字节
//返 回 值：成功返回MI_OK
//--------------------------------------------------------------------
sys_res mf_SelectCard(mifare mf)
{
    sys_res res;
	size_t nLen;
	int i;
	u8 snr_check = 0;
	u8 aBuf[RC522_RX_SIZE];

	if (mf->ste != MF_S_ACTIVE)
		return SYS_R_BUSY;
	
	mf->ste = MF_S_BUSY;
	rc522_ClrRegBit(mf->p, Status2Reg, 0x08);
	rc522_RegWrite(mf->p, BitFramingReg, 0x00);
	rc522_ClrRegBit(mf->p, CollReg, 0x80);
	aBuf[0] = PICC_ANTICOLL1;
	aBuf[1] = 0x20;
	
	res = rc522_PcdCom(mf->p, PCD_TRANSCEIVE, aBuf, 2, aBuf, &nLen);
	if (res == SYS_R_OK)
	{
		memcpy(&mf->cid, aBuf, 4);
		for (i = 0; i < 4; i++)
		{
			snr_check ^= aBuf[i];
		}
		if (snr_check != aBuf[4])
			res = SYS_R_ERR;
	}
	
	rc522_SetRegBit(mf->p, CollReg, 0x80);
	if (res == SYS_R_OK)
	{
		aBuf[0] = PICC_ANTICOLL1;
		aBuf[1] = 0x70;
		aBuf[6] = 0;
		memcpy(&aBuf[2], &mf->cid, 4);
		for (i = 0; i < 4; i++)
		{
			aBuf[6] ^= aBuf[i + 2];
		}
		rc522_CRC16(mf->p, aBuf, 7, &aBuf[7]);
		rc522_ClrRegBit(mf->p, Status2Reg, 0x08);
		res = rc522_PcdCom(mf->p, PCD_TRANSCEIVE, aBuf, 9, aBuf, &nLen);
		
		if ((res != SYS_R_OK) || (nLen != 0x18))
			res = SYS_R_ERR;
	}
	mf->ste = MF_S_ACTIVE;
	
	return res;
}

sys_res mf_PcdReset(mifare mf)
{
	sys_res res;
	size_t nLen;
	u8 aBuf[RC522_RX_SIZE]; 

	if (mf->ste != MF_S_ACTIVE)
		return SYS_R_BUSY;
	
	mf->ste = MF_S_BUSY;
	aBuf[0] = PICC_RESET;
	aBuf[1] = 0x50;
	
	rc522_CRC16(mf->p, aBuf, 2, &aBuf[2]);
	res = rc522_PcdCom(mf->p, PCD_TRANSCEIVE, aBuf, 4, aBuf, &nLen);
	
	mf->ste = MF_S_ACTIVE;
    if (res == SYS_R_OK)
		mf->block = 0;
	
	return res;
}

sys_res mf_SelectApp(mifare mf, int nParam)
{
	sys_res res;
	size_t nLen;
	u8 aBuf[RC522_RX_SIZE] = {0x02, 0x00, 0xA4, 0x00, 0x00, 0x02};

	memcpy(&aBuf[6], &nParam, 2);
	res = mf_Transce(mf, aBuf, 8, &nLen);
	
	return res;
}

sys_res mf_Send(mifare mf, int nIns, int nP1, int nP2, void *pBuf, size_t nIn, size_t nOut)
{
	sys_res res = SYS_R_ERR;
	u8 aBuf[RC522_RX_SIZE];

	if (nOut > (RC522_RX_SIZE - 5))
		nOut = RC522_RX_SIZE - 5;
	
	aBuf[1] = 0;
	aBuf[2] = nIns;
	aBuf[3] = nP1;
	aBuf[4] = nP2;
	if (nIn)
	{
		aBuf[5] = nIn;
		memcpy(&aBuf[6], pBuf, nIn);
	}
	else
	{
		aBuf[5] = nOut;
	}
	
	if (mf_Transce(mf, aBuf, 6 + nIn, &nIn) == SYS_R_OK)
	{
		if (nOut == 0)
		{
			if ((aBuf[nIn - 4] == 0x90) && (aBuf[nIn - 3] == 0x00))
				res = SYS_R_OK;
		}
		else if (nIn == (5 + nOut))
		{
			if ((aBuf[1 + nOut] == 0x90) && (aBuf[2 + nOut] == 0x00))
			{
				memcpy(pBuf, &aBuf[1], nOut);
				res = SYS_R_OK;
			}
		}
	}
	
	return res;
}


#endif

