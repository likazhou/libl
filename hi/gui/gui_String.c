#if GUI_ENABLE

//Private Defines
#define ASC12_OFFSET			0x00000
#define ASC16_OFFSET			0x00474
#define FNT12_OFFSET_A1			0x00A64
#define FNT12_OFFSET_A2			0x045E0
#define FNT16_OFFSET_A1			0x19058
#define FNT16_OFFSET_A2			0x1FA18




//Private Macros
#if GUI_FONT_TYPE == GUI_FONT_STD12
#if ASC_HEIGHT == 12
#define gui_DrawChar_ASC		gui_DrawString_ASC6x12
#else
#define gui_DrawChar_ASC		gui_DrawString_ASC6x8
#endif
#define gui_DrawChar_HZ			gui_DrawChar_HZ12
#endif

#if GUI_FONT_TYPE == GUI_FONT_STD16
#define gui_DrawChar_ASC		gui_DrawString_ASC6x16
#define gui_DrawChar_HZ			gui_DrawChar_HZ16
#endif




//===============================================================
//混合字符串显示，包含汉字和ASC字符
//===============================================================
static int gui_GetStringWidth(const char *pStr)
{
	int nWidth = 0;

	while (*pStr != '\0')
	{
		if ((u8)*pStr < 0x80)
		{
			nWidth += ASC_WIDTH;
			pStr += 1;
		}
		else
		{
			nWidth += (HZ_WIDTH + HZ_SPAN);
			pStr += 2;
		}
	}
	
	return nWidth;
}

//===============================================================
//6*8点阵字符显示
//在字符表里面已经包含了字符之间的间隙位置
//x,y  输入起始坐标,pStr字符串
//===============================================================
static void gui_DrawChar_ASC6x8(int x, int y, const char cChar, t_color nColor)
{
	int i, j;
	const u8 *p;

	p = FONT6x8ASCII + (int)(cChar - ' ') * ASC6x8_SIZE;	//将ASCII转换成实际值"!"的ASCII为33。

	for(i = 0; i < ASC6x8_SIZE; i++, p++)
	{
		for(j = 0; j < 8; j++)
		{
			if (*p & BITMASK(j))
				gui_DrawPoint(x + i, y + j, nColor);
			else
				gui_DrawPoint(x + i, y + j, ~nColor);
		}
	}
}

//===============================================================
//显示一个6*12点阵字符
//x,y  输入起始坐标,pStr字符串
//===============================================================
#if ASC_HEIGHT == 12
static void gui_DrawChar_ASC6x12(int x, int y, const char cChar, t_color nColor)
{
	int i, j;
	const u8 *p;

	p = FONT6x12ASCII + (int)(cChar - ' ') * ASC6x12_SIZE;	//将ASCII转换成实际值"!"的ASCII为33。

	for(i = 0; i < 12; i++, p++)
	{
        for(j = 0; j < 6; j++)
		{
            if (*p & BITMASK(j))
                lcd_DrawPoint(x + j, y + i, nColor);
            else
                lcd_DrawPoint(x + j, y + i, ~nColor);
        }
    }
}
#endif


//===============================================================
//显示一个6*16点阵字符
//x,y  输入起始坐标,pStr字符串
//===============================================================
#if ASC_HEIGHT == 16
static void gui_DrawChar_ASC6x16(int x, int y, const char cChar, t_color nColor)
{
	int i, j;
	const u8 *p1, *p2;

	p1 = FONT6x16ASCII + (int)(cChar - ' ') * ASC6x16_SIZE;	//将ASCII转换成实际值"!"的ASCII为33
	p2 = p1 + 6;

	for(i = 0; i < 6; i++, p1++, p2++)
	{
		for(j = 0; j < 8; j++)
		{
			if (*p1 & BITMASK(j))
			    lcd_DrawPoint(x + i, y + j, nColor);
			else
			    lcd_DrawPoint(x + i, y + j, ~nColor);
			
			if (*p2 & BITMASK(j))
			    lcd_DrawPoint(x + i, y + j + 8, nColor);
			else
			    lcd_DrawPoint(x + i, y + j + 8, ~nColor);
		}
	}               
}
#endif


//===============================================================
//显示一个汉字
//12*12点阵
//x,y  输入起始坐标,pStr字符串
//===============================================================
#if GUI_FONT_CARR_TYPE == GUI_FONT_CARR_T_DIRACCESS
static void gui_GetFont(int nOffset, void *pBuf, size_t nLen)
{

#if 0
	memcpy(pBuf, (const void *)(GUI_FONT_BASE + nOffset), nLen);
#else
	u8 *p1 = (u8 *)pBuf, *p2 = (u8 *)(GUI_FONT_BASE + nOffset);
	int i;

	for (i = 0; i < nLen; i++)
	{
		*p1++ = *p2++;		
	}
#endif
}
#endif

#if GUI_FONT_CARR_TYPE == GUI_FONT_CARR_T_FILE
static void gui_GetFont(int nOffset, void *pBuf, size_t nLen)
{
	int fd;

	os_thd_lock();
	
	fd = open(GUI_FONT_FILENAME, O_RDONLY, 0);
	if (fd >= 0)
	{
		lseek(fd, nOffset, DFS_SEEK_SET);
		read(fd, pBuf, nLen);
		close(fd);
	}
	
	os_thd_unlock();
}
#endif

#if GUI_FONT_CARR_TYPE == GUI_FONT_CARR_T_NAND
#define GUI_GETFONT_STATIC_BUF			(1 && NANDFLASH_ENABLE)
#if GUI_GETFONT_STATIC_BUF
static u8 gui_aGetFontBuf[NAND_PAGE_DATA + NAND_PAGE_SPARE];
#endif
static void gui_GetFont(int nOffset, void *pBuf, size_t nLen)
{
	int nTemp;
	u32 nEcc;
	u_byte4 uPage;
	u8 *pMem;

#if GUI_GETFONT_STATIC_BUF
	pMem = gui_aGetFontBuf;
#else
	pMem = mem_Malloc(NAND_PAGE_DATA + NAND_PAGE_SPARE);
	if (pMem == NULL)
		return;
#endif
	uPage.n = (GUI_FONT_BASE * NAND_BLK_PAGE) + (nOffset / NAND_PAGE_DATA);
	nOffset %= NAND_PAGE_DATA;
	nTemp = NAND_PAGE_DATA - nOffset;
	
	os_thd_lock();
	
	nand_ReadPage(&uPage, pMem, &nEcc);
	if (nTemp < nLen)
	{
		memcpy(pBuf, pMem + nOffset, nTemp);
		uPage.n += 1;
		nand_ReadPage(&uPage, pMem, &nEcc);
		memcpy((u8 *)pBuf + nTemp, pMem, nLen - nTemp);
	}
	else
	{
		memcpy(pBuf, pMem + nOffset, nLen);
	}

	os_thd_unlock();
	
#if GUI_GETFONT_STATIC_BUF == 0
	mem_free(pMem);
#endif
}
#endif

#if GUI_FONT_CARR_TYPE == GUI_FONT_CARR_T_SPIF
static void gui_GetFont(int nOffset, void *pBuf, size_t nLen)
{

	spif_Read(GUI_FONT_BASE + nOffset, pBuf, nLen);
}
#endif

static void gui_DrawChar_HZ12(int x, int y, const char *pStr, t_color nColor)
{
	int x1, y1;
	int i = 0, nOffset, nLow, nHigh;
	u8 aData[HZ_SIZE];

	if ((u8)pStr[0] >= 0xA0 + 16)
	{
		nOffset = FNT12_OFFSET_A2 + ((pStr[0] - 0xA0 - 16) * 94 + pStr[1] - 0xA1) * HZ_SIZE;
	}
	else if ((u8)pStr[0] <= 0xA0 + 9)
	{
	    //字库中没有 A1 区
		if ((u8)pStr[0] > 0xA1)
			nLow = pStr[0] - 1;
		else
			nLow = pStr[0];
		nOffset = FNT12_OFFSET_A1 + ((nLow - 0xA0) * 94 + pStr[1] - 0xA1) * HZ_SIZE;
    }
	else
	{
		nLow  = 0xA1;
		nHigh  = 0xF5;
		nOffset = FNT12_OFFSET_A1 + ((nLow - 0xA0) * 94 + nHigh - 0xA1) * HZ_SIZE;
    }    
	
	gui_GetFont(nOffset, aData, sizeof(aData));
	for (y1 = y; y1 < (y + HZ_WIDTH); y1++)
	{
		for (x1 = x; x1 < (x + HZ_WIDTH); x1++)
		{
			if (aData[i >> 3] & BITMASK(7 - (i & 7)))
				lcd_DrawPoint(x1, y1, nColor);
			else
				lcd_DrawPoint(x1, y1, ~nColor);
			
			i += 1;
		}
	}
	
#if HZ_SPAN
	x1 = x + HZ_WIDTH;
	for (i = 0; i < HZ_SPAN; i++, x1++)
	{
		for (y1 = y; y1 < (y + HZ_WIDTH); y1++)
		{
			lcd_DrawPoint(x1, y1, ~nColor);
		}
	}
#endif
}

static void gui_DrawChar_HZ16(int x, int y, const char *pStr, t_color nColor)
{
	int x1, y1;
	int i, nOffset, nLow, nHigh, nByte = 0;
	u8 aData[HZ_SIZE];
	const u8 *p = aData;

	if ((u8)pStr[0] >= 0xA0 + 16)
	{
		nOffset = FNT16_OFFSET_A2 + ((pStr[0] - 0xA0 - 16) * 94 + pStr[1] - 0xA1) * HZ_SIZE;
	}
	else if ((u8)pStr[0] <= 0xA0 + 9)
	{
	    //字库中没有 A1 区
		if ((u8)pStr[0] > 0xA1)
			nLow = pStr[0] - 1;
		else
			nLow = pStr[0];
		nOffset = FNT16_OFFSET_A1 + ((nLow - 0xA0) * 94 + pStr[1] - 0xA1) * HZ_SIZE;
    }
	else
	{
		nLow = 0xA1;
		nHigh = 0xF5;
		nOffset = FNT16_OFFSET_A1 + ((nLow - 0xA0) * 94 + nHigh - 0xA1) * HZ_SIZE;
    }    
	gui_GetFont(nOffset, aData, sizeof(aData));
	
	x1 = x + HZ_WIDTH;
	y1 = y + HZ_HEIGHT;
	for (; y < y1; y++, x -= HZ_WIDTH)
	{
		for (i = 7; x < x1; i--, x++)
		{
			if (i == 7)
				nByte = *p++;
			if (nByte & BITMASK(i))
				gui_DrawPoint(x, y, nColor);
			else
				gui_DrawPoint(x, y, ~nColor);
			
			if (i == 0)
				i = 8;
		}
 	}
	
#if HZ_SPAN
	for (i = 0; i < HZ_SPAN; i++, x1++)
	{
		for (y1 = y; y1 < (y + HZ_WIDTH); y1++)
		{
			lcd_DrawPoint(x1, y1, ~nColor);
		}
	}
#endif
}



//External Functions
//===============================================================
//
//===============================================================
void gui_DrawString_ASC6x8(int x, int y, const char *pStr, t_color nColor)
{

	while (*pStr != '\0')
	{
		gui_DrawChar_ASC6x8(x, y, *pStr++, nColor);
		x += ASC_WIDTH;
	}
}


//===============================================================
//
//===============================================================
#if ASC_HEIGHT == 12
void gui_DrawString_ASC6x12(int x, int y, const char *pStr, t_color nColor)
{

	while (*pStr != '\0')
	{
		gui_DrawChar_ASC6x12(x, y, *pStr++, nColor);
		x += ASC_WIDTH;
	}
}
#endif

//===============================================================
//
//===============================================================
#if ASC_HEIGHT == 16
void gui_DrawString_ASC6x16(int x, int y, const char *pStr, t_color nColor)
{

	while (*pStr != '\0')
	{
		gui_DrawChar_ASC6x16(x, y, *pStr++, nColor);
		x += ASC_WIDTH;
	}
}
#endif

//===============================================================
//
//===============================================================
int gui_DrawString_Mixed(int x, int y, const char *pStr, t_color nColor)
{

	while (*pStr != '\0')
	{
		if ((u8)*pStr < 0x80)
		{
			gui_DrawChar_ASC(x, y + (HZ_HEIGHT - ASC_HEIGHT), (const char *)pStr, nColor);		//ASC码
			pStr += 1;
			x += ASC_WIDTH;
		}
		else
		{
			gui_DrawChar_HZ(x, y, pStr, nColor);		//汉字
			pStr += 2;
			x += (HZ_WIDTH + HZ_SPAN);
		}
	}
	
	return x;
}

//===============================================================
//混合字符串显示，包含汉字和ASC字符
//===============================================================
int gui_DrawString_Mixed_Center(int x, int y, const char *pStr, t_color nColor)
{
	int nX, nWidth;
	
	nWidth = gui_GetStringWidth(pStr);
	nX = (LCD_X_MAX + x - nWidth) >> 1;
	gui_DrawString_Mixed(nX, y, pStr, nColor);
	
	return nX;
}

//===============================================================
//混合字符串显示，包含汉字和ASC字符
//===============================================================
int gui_DrawString_Mixed_Right(int y, const char *pStr, t_color nColor)
{
	int nX, nWidth;
	
	nWidth = gui_GetStringWidth(pStr);
	nX = LCD_X_MAX - nWidth;
	gui_DrawString_Mixed(nX, y, pStr, nColor);
	
	return nX;
}


#endif

