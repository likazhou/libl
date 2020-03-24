#include <string.h>
#include <stdlib.h>




//-------------------------------------------------------------------------
//得到uint32中其中几位构成的值
//-------------------------------------------------------------------------
u32 int_getbits(u32 nData, int nStartBit, int nBits)
{
	u32 nMask;

	nMask = (0xFFFFFFFF >> (32 - nBits)) << nStartBit;
	return ((nData & nMask) >> nStartBit);
}

//-------------------------------------------------------------------------
//设置uint32中其中几位构成的值
//-------------------------------------------------------------------------
u32 int_setbits(u32 nData, int nStartBit, int nBits, u32 nVal)
{
	u32 nMask;

	nMask = 0xFFFFFFFF >> (32 - nBits);
	return ((nData & nMask) << nStartBit);
}

//-------------------------------------------------------------------------
//获得指定位的值
//-------------------------------------------------------------------------
int getbit(const void *pData, int i)
{
	const u8 *p = pData;
	int j;

	j = i & 7;
	i >>= 3;

	return (p[i] & BITMASK(j)) ? 1 : 0;
}

//-------------------------------------------------------------------------
//置指定位的值
//-------------------------------------------------------------------------
void setbit(void *pData, int i)
{
	u8 *p = pData;
	int j;

	j = i & 7;
	i >>= 3;
	
	SETBIT(p[i], j);
}

//-------------------------------------------------------------------------
//清指定位的值
//-------------------------------------------------------------------------
void clrbit(void *pData, int i)
{
	u8 *p = pData;
	int j;

	j = i & 7;
	i >>= 3;
	
	CLRBIT(p[i], j);
}

//-------------------------------------------------------------------------
//计算uint中有多少位1
//-------------------------------------------------------------------------
int bits1(u32 i)
{
	int j;
	
	for (j = 0; i; i &= (i - 1))
	{
		j += 1;
	}
	
	return j;
}

//-------------------------------------------------------------------------
//按位倒序
//-------------------------------------------------------------------------
u32 invert_bits(u32 nData, int nBits)
{
	int i, j, nTemp;

	for (i = 0, j = nBits - 1; i < j; i++, j--)
	{
		if (nData & BITMASK(i))
			nTemp = 1;
		else
			nTemp = 0;
		
		if (nData & BITMASK(j))
			SETBIT(nData, i);
		else
			CLRBIT(nData, i);
		
		if (nTemp)
			SETBIT(nData, j);
		else
			CLRBIT(nData, j);
	}
	
	return nData;
}

//-------------------------------------------------------------------------
//按字节倒序
//-------------------------------------------------------------------------
void invert(void *pData, size_t nLen)
{
	int nTemp;
	u8 *pStart = pData;
	u8 *pEnd = pStart + nLen - 1;

	while (pStart < pEnd)
	{
		nTemp = *pStart;
		*pStart++ = *pEnd;
		*pEnd-- = nTemp;
	}
}

//-------------------------------------------------------------------------
//copy并且翻转
//-------------------------------------------------------------------------
void cpyinvert(void *dst, const void *src, size_t count)
{

	memcpy(dst, src, count);
	invert(dst, count);
}

//-------------------------------------------------------------------------
//按字节运算
//-------------------------------------------------------------------------
void byteadd(void *pData, int nValue, size_t nLen)
{
	u8 *pStart = pData;
	u8 *pEnd = pStart + nLen;
	int nTemp;

	for (; pStart < pEnd; pStart++)
	{
		nTemp = *pStart;
		nTemp += nValue;
		*pStart = nTemp;
	}
}

//-------------------------------------------------------------------------
//计数循环
//-------------------------------------------------------------------------
int cycle(int n, int nStart, int nEnd, int nStepping)
{
	int nResult;
	u32 nSpan;

	if (nStart > nEnd)
		return n;
	
	nSpan = nEnd - nStart + 1;
	if (n > nEnd)
		nResult = nEnd;
	else if (n < nStart)
		nResult = nStart;
	else
		nResult = n;
	
	//步进
	if (nStepping < 0)
		nStepping = -(abs(nStepping) % nSpan);
	else
		nStepping = abs(nStepping) % nSpan;
	
	nResult = ((nResult - nStart + nSpan + nStepping) % nSpan) + nStart;
	
	return nResult;
}



