
#if BKP_ENABLE





//Internal Functions
static sys_res bkp_EepromWrite(adr_t nAdr, const u8 *pBuf, size_t nLen)
{
	i2c_t *p;
	adr_t nEnd;
	size_t nSize;
	u8 aBuf[BKP_PAGE_SIZE + 2];

	p = i2c_Open(BKP_COMID, OS_TMO_FOREVER);
	nEnd = nAdr + nLen;
	for (; nAdr < nEnd; nAdr += nSize, pBuf += nSize, nLen -= nSize)
	{
		aBuf[0] = nAdr >> 8;
		aBuf[1] = nAdr;
		nSize = nLen < BKP_PAGE_SIZE ? nLen : BKP_PAGE_SIZE;
		memcpy(&aBuf[2], pBuf, nSize);
		if (i2c_Write(p, BKP_DEVID, aBuf, nSize + 2))
			break;
#if BKP_WAIT_MS
		os_thd_sleep(BKP_WAIT_MS);
#endif
	}
	
	i2c_Close(p);
	
	if (nAdr < nEnd)
		return SYS_R_ERR;
	
	return SYS_R_OK;
}








//External Functions
void bkp_Init()
{

#if BKP_TYPE == BKP_T_INT
	arch_BkpInit();
#endif
}

//-------------------------------------------------------------------------
//Function Name  :bkp_Write
//Description    : 把以nStartAdr开头的nLen个字节写入BKP的寄存器，
//				   每次写一个字节，内容为*P
//Input          : -nStartAdr:0~19的整数 ，BKP共10组寄存器，2个字节为一组
//				-*p:写进BKP的内容，一次写一个字节
//				-nLen:1~(20-nStartAdr)
//Output         : Non
//Return         : SYS_R_ERR 或SYS_R_OK
//-------------------------------------------------------------------------
sys_res bkp_Write(adr_t nAdr, const void *pBuf, size_t nLen)
{

#if BKP_TYPE == BKP_T_INT
	return arch_BkpWrite(nAdr, pBuf, nLen);
#else
	return bkp_EepromWrite(nAdr, pBuf, nLen);
#endif
}


sys_res bkp_WriteData(adr_t nAdr, const u64 nData, size_t nLen)
{

	return bkp_Write(nAdr, &nData, nLen);
}

void bkp_Fill(adr_t nAdr, adr_t nEnd, int nVal)
{
	size_t nLen, nFill;
	u8 aBuf[64];

	memset(aBuf, nVal, sizeof(aBuf));
	nLen = nEnd - nAdr;
	for (; nLen; nLen -= nFill, nAdr += nFill)
	{
		nFill = MIN(sizeof(aBuf), nLen);
		bkp_Write(nAdr, aBuf, nFill);
	}
}


//-------------------------------------------------------------------------
//Function Name  :bkp_Read
//Description    : 把以nStartAdr开头的nLen个字节从BKP寄存器  读出
//				   每次读一个字节，内容为*P
//Input          : -nStartAdr:0~19的整数 ，BKP共10组寄存器，2个字节为一组
//				-*p: 被读出的BKP寄存器的内容，一次读一个字节
//				-nLen:1~(20-nStartAdr)
//Output         : None
//Return         : SYS_R_ERR 或SYS_R_OK
//-------------------------------------------------------------------------
sys_res bkp_Read(adr_t nAdr, void *pBuf, size_t nLen)
{

#if BKP_TYPE == BKP_T_INT
	return arch_BkpRead(nAdr, pBuf, nLen);
#else
	sys_res res;
	i2c_t *p;

	p = i2c_Open(BKP_COMID, OS_TMO_FOREVER);
	
	invert(&nAdr, 2);
	res = i2c_Write(p, BKP_DEVID, &nAdr, 2);
	if (res == SYS_R_OK)
		res = i2c_Read(p, BKP_DEVID, pBuf, nLen);
	
	i2c_Close(p);
	
	return res;
#endif
}

#endif

