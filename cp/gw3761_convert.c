


//-------------------------------------------------------------------------
//Êý¾Ý×ª»»
//-------------------------------------------------------------------------
u16 gw3761_ConvertDa2DA(int nDa)
{

#if GW3761_TYPE < GW3761_T_GWFK2004
	if (nDa)
	{
		nDa -= 1;
		return (((nDa >> 3) + 1) << 8) | BITMASK(nDa & 7);
	}
#else
	if (nDa)
		return (0x0100 << ((nDa - 1) & 7)) | (0x0001 << (nDa >> 3));
#endif
	return 0;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
u16 gw3761_ConvertFn2DT(int nFn)
{

	nFn -= 1;
	return ((0x0001 << (nFn % 8)) | ((nFn / 8) << 8));
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
int gw3761_ConvertDt2Fn(u16 nDt)
{
	int i;

	for (i = 0; i < 8; i++)
	{
		if (nDt & BITMASK(i))
			break;
	}
	if (i < 8)
	{
		nDt >>= 8;
		return ((nDt << 3) | i) + 1;
	}
	return 0;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
int gw3761_ConvertDA2Map(u16 nDA, void *pData)
{
#if GW3761_TYPE < GW3761_T_GWFK2004
	u16 *p = (u16 *)pData;
#else
	int i;
	u8 *p = (u8 *)pData;
#endif
	int j, nQty = 0, nDa;

	nDa = nDA >> 8;
	if (nDa == 0)
	{
		*p = 0;
		return 1;
	}
	
	nDa -= 1;
	
#if GW3761_TYPE < GW3761_T_GWFK2004
	for (j = 0; j < 8; j++)
	{
		if (nDA & BITMASK(j))
			p[nQty++] = nDa * 8 + j + 1;
	}
#else
	for (i = 0; i < 8; i++)
	{
		if (nDa & BITMASK(i))
		{
			for (j = 0; j < 8; j++)
			{
				if (nDA & BITMASK(j))
					p[nQty++] = i * 8 + j;
			}
		}
	}
#endif
	return nQty;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void gw3761_ConvertData(void *p, u32 nData, int nDec, u32 nMark, int nSignBit, size_t nSize, int nSign)
{
    u32 nResult;
    float fData;

    if (nSign)
	{
        if ((fixpoint)nData < 0)
		{
            nData = abs((s32)nData);
            nSign = BITMASK(nSignBit);
        }
		else
		{
            nSign = 0;
        }
    }
	
    fData = (float)nData * math_pow10[nDec] * FIXP_EPSILON + 0.5f;
    nResult = bin2bcd32(fData) & nMark;
    nResult |= nSign;
	
    memcpy(p, &nResult, nSize);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void gw3761_ConvertData_Time(u8 *p, time_t tTime, int nType)
{
    u8 aTime[6];

    timet2array(tTime, aTime, 1);
    memcpy(&p[0], &aTime[1], 3);
    p += 3;
	
    switch (nType)
	{
    case GW3761_DATA_T_15:
        *p++ = aTime[4];
    case GW3761_DATA_T_17:
        *p++ = aTime[5];
    default:
        break;
    }
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void gw3761_ConvertData_01(void *p, time_t tTime)
{
    struct tm tmTime;
    u8 *pData = (u8 *)p;

    gmtime_r(&tTime, &tmTime);
    *pData++ = bin2bcd8(tmTime.tm_sec);
    *pData++ = bin2bcd8(tmTime.tm_min);
    *pData++ = bin2bcd8(tmTime.tm_hour);
    *pData++ = bin2bcd8(tmTime.tm_mday);
    *pData = bin2bcd8(tmTime.tm_mon + 1);
#if GW3761_DATA01_WDAY
	if (tmTime.tm_wday == 0)
        tmTime.tm_wday = 7;
    *pData |= (tmTime.tm_wday << 5);
#endif
    pData += 1;
    *pData++ = bin2bcd8(tmTime.tm_year - 100);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void gw3761_ConvertData_03(void *p, u32 nData, int nSign)
{

	gw3761_ConvertData(p, nData, 0, 0x0FFFFFFF, 28, 4, nSign);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void gw3761_ConvertData_05(void *p, u32 nData, int nSign)
{

	gw3761_ConvertData(p, nData, 1, 0x00007FFF, 15, 2, nSign);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void gw3761_ConvertData_05_Percent(void *p, u32 nData, int nSign)
{

	gw3761_ConvertData(p, nData, 3, 0x00007FFF, 15, 2, nSign);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void gw3761_ConvertData_06(void *p, u32 nData, int nSign)
{

	gw3761_ConvertData(p, nData, 2, 0x00007FFF, 15, 2, nSign);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void gw3761_ConvertData_07(void *p, u32 nData)
{

	gw3761_ConvertData(p, nData, 1, 0x0000FFFF, 0, 2, 0);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void gw3761_ConvertData_09(void *p, u32 nData, int nSign)
{

	gw3761_ConvertData(p, nData, 4, 0x007FFFFF, 23, 3, nSign);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void gw3761_ConvertData_11(void *p, u32 nData)
{

	gw3761_ConvertData(p, nData, 2, 0xFFFFFFFF, 0, 4, 0);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void gw3761_ConvertData_13(void *p, u32 nData)
{

	gw3761_ConvertData(p, nData, 4, 0xFFFFFFFF, 0, 4, 0);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void gw3761_ConvertData_14(u8 *p, float fData)
{
	u64 nResult = 0;
 
	fData = fData * 100.0f + 0.5f;
	nResult = bin2bcd64(fData) & (u64)0x000000FFFFFFFFFF;
	*p++ = 0;
	memcpy(p, &nResult, 4);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void gw3761_ConvertData_15(void *p, time_t tTime)
{

	gw3761_ConvertData_Time(p, tTime, GW3761_DATA_T_15);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void gw3761_ConvertData_17(void *p, time_t tTime)
{

	gw3761_ConvertData_Time(p, tTime, GW3761_DATA_T_17);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void gw3761_ConvertData_18(void *p, time_t tTime)
{

	gw3761_ConvertData_Time(p, tTime, GW3761_DATA_T_18);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void gw3761_ConvertData_22(void *p, u32 nData)
{

	gw3761_ConvertData(p, nData, 1, 0x000000FF, 0, 1, 0);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void gw3761_ConvertData_23(void *p, u32 nData)
{

	gw3761_ConvertData(p, nData, 4, 0x00FFFFFF, 0, 3, 0);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void gw3761_ConvertData_25(void *p, u32 nData, int nSign)
{

	gw3761_ConvertData(p, nData, 3, 0x007FFFFF, 23, 3, nSign);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void gw3761_ConvertData_26(void *p, u32 nData)
{

	gw3761_ConvertData(p, nData, 3, 0x0000FFFF, 0, 2, 0);
}




