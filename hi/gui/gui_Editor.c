#if GUI_ENABLE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#if LCD_Y_MAX == 64
#define GUI_EDIT_INFO_Y			20
#define GUI_EDIT_DATA_Y			40
#endif
#if LCD_Y_MAX == 160
#define GUI_EDIT_INFO_Y			36
#define GUI_EDIT_DATA_Y			60
#endif




//-------------------------------------------------------------------------
//数字转换
//-------------------------------------------------------------------------
const char tbl_guiEditorSetNum[] = {'-', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
static char gui_EditorCompnet_Number(char ch, int nStart, int nEnd, int nSelect)
{
	int i;

	for (i = 0; i < ARR_SIZE(tbl_guiEditorSetNum); i++)
	{
		if (ch == tbl_guiEditorSetNum[i])
			break;
	}
	i = cycle(i, nStart, nEnd, nSelect);
	
	return tbl_guiEditorSetNum[i];
}







//-------------------------------------------------------------------------
//特殊设置
//-------------------------------------------------------------------------
int gui_Editor_Special(const char *pInfo, t_gui_spec_editor *pTable, int nQty, int nIndex, int nSelect)
{

	//绘制提示信息
	gui_DrawString_Mixed_Center(0, GUI_EDIT_INFO_Y, pInfo, COLOR_BLACK);

	nIndex = cycle(nIndex, 0, nQty - 1, nSelect);
	gui_DrawString_Mixed_Center(0, GUI_EDIT_DATA_Y, pTable[nIndex].str, COLOR_BLACK);

	return nIndex;
}

//-------------------------------------------------------------------------
//IP地址设置
//-------------------------------------------------------------------------
void gui_Editor_IPAddressCreate(char *pStr, u8 *pIP)
{

	//转换IP地址为规格字符串
	sprintf(pStr, "%03d.%03d.%03d.%03d", pIP[0], pIP[1], pIP[2], pIP[3]);
}

void gui_Editor_IPAddressSet(const char *pInfo, char *pStr, int nIndex, int nSelect)
{
	int x;

	//绘制提示信息
	gui_DrawString_Mixed_Center(0, GUI_EDIT_INFO_Y, pInfo, COLOR_BLACK);

	//整形
	pStr[3] = pStr[7] = pStr[11] = '.';
	if (nIndex > 8)
		nIndex += 3;
	else if (nIndex > 5)
		nIndex += 2;
	else if (nIndex > 2)
		nIndex += 1;

	//数字滚动
	switch (nIndex)
	{
	case 0:
	case 4:
	case 8:
	case 12:
		pStr[nIndex] = gui_EditorCompnet_Number(pStr[nIndex], 1, 3, nSelect);
		break;
	default:
		pStr[nIndex] = gui_EditorCompnet_Number(pStr[nIndex], 1, 10, nSelect);
		break;
	}

	//绘制IP地址及光标
	x = gui_DrawString_Mixed_Center(0, GUI_EDIT_DATA_Y, pStr, COLOR_BLACK) + ASC_WIDTH * nIndex;
	gui_DrawHLine(x, GUI_EDIT_DATA_Y + HZ_HEIGHT, x + (ASC_WIDTH - 1), COLOR_BLACK);
}

void gui_Editor_IPAddressDone(char *pStr, u8 *pIP)
{
	int i;

	//整形
	pStr[3] = 0;
	pStr[7] = 0;
	pStr[11] = 0;

	//转换规格字符串为IP地址
	for (i = 0; i < 4; ++i)
	{
		pIP[i] = atoi(&pStr[i << 2]);
	}
}


//-------------------------------------------------------------------------
//百分比设置
//-------------------------------------------------------------------------
void gui_Editor_PercentCreate(char *pStr, int nData)
{

	//转换为规格字符串
	sprintf(pStr, "%03d.%02d%%", nData / 100, nData % 100);
}

void gui_Editor_PercentSet(const char *pInfo, char *pStr, int nIndex, int nSelect)
{
	int x;

	//绘制提示信息
	gui_DrawString_Mixed_Center(0, GUI_EDIT_INFO_Y, pInfo, COLOR_BLACK);

	//整形
	pStr[3] = '.';
	if (nIndex > 2)
		nIndex += 1;

	//数字滚动
	pStr[nIndex] = gui_EditorCompnet_Number(pStr[nIndex], 1, 10, nSelect);

	//绘制IP地址及光标
	x = gui_DrawString_Mixed_Center(0, GUI_EDIT_DATA_Y, pStr, COLOR_BLACK) + ASC_WIDTH * nIndex;
	gui_DrawHLine(x, GUI_EDIT_DATA_Y + HZ_HEIGHT, x + (ASC_WIDTH - 1), COLOR_BLACK);
}

int gui_Editor_PercentDone(char *pStr)
{

	//整形
	memmove(&pStr[3], &pStr[4], 2);
	pStr[5] = 0;

	//转换规格字符串为数字
	return atoi(pStr);
}


//-------------------------------------------------------------------------
//时间设置
//-------------------------------------------------------------------------
void gui_Editor_TimeCreate(char *pStr, time_t tTime)
{
	u8 aTime[6];

	timet2array(tTime, aTime, 0);
	sprintf(pStr, "20%02d-%02d-%02d %02d:%02d:%02d", aTime[5], aTime[4], aTime[3], aTime[2], aTime[1], aTime[0]);
}

void gui_Editor_TimeSet(const char *pInfo, char *pStr, int nIndex, int nSelect)
{
	int x;

	//绘制提示信息
	gui_DrawString_Mixed_Center(0, GUI_EDIT_INFO_Y, pInfo, COLOR_BLACK);

	//整形
	pStr[4] = pStr[7] = '-';
	pStr[10] = ' ';
	pStr[13] = pStr[16] = ':';
	if (nIndex > 11)
		nIndex += 5;
	else if (nIndex > 9)
		nIndex += 4;
	else if (nIndex > 7)
		nIndex += 3;
	else if (nIndex > 5)
		nIndex += 2;
	else if (nIndex > 3)
		nIndex += 1;

	//数字滚动
	switch (nIndex)
	{
	case 5:
		//month(0-1)
		pStr[nIndex] = gui_EditorCompnet_Number(pStr[nIndex], 1, 2, nSelect);
		break;
	case 8:
		//mday(0-3)
		pStr[nIndex] = gui_EditorCompnet_Number(pStr[nIndex], 1, 4, nSelect);
		break;
	case 11:
		//hour(0-2)
		pStr[nIndex] = gui_EditorCompnet_Number(pStr[nIndex], 1, 3, nSelect);
		break;
	case 14:
	case 17:
		//min(0-5)
		pStr[nIndex] = gui_EditorCompnet_Number(pStr[nIndex], 1, 6, nSelect);
		break;
	default:
		pStr[nIndex] = gui_EditorCompnet_Number(pStr[nIndex], 1, 10, nSelect);
		break;
	}

	//绘制时间及光标
	x = gui_DrawString_Mixed_Center(0, GUI_EDIT_DATA_Y, pStr, COLOR_BLACK) + ASC_WIDTH * nIndex;
	gui_DrawHLine(x, GUI_EDIT_DATA_Y + HZ_HEIGHT, x + (ASC_WIDTH - 1), COLOR_BLACK);
}

time_t gui_Editor_TimeDone(char *pStr)
{
	struct tm tmTime;

	//整形
	pStr[4] = pStr[7] = pStr[10] = pStr[13] = pStr[16] = 0;
	//转换规格字符串为time_t
	tmTime.tm_year = atoi(&pStr[0]) - 1900;
	tmTime.tm_mon = atoi(&pStr[5]) - 1;
	tmTime.tm_mday = atoi(&pStr[8]);
	tmTime.tm_hour = atoi(&pStr[11]);
	tmTime.tm_min = atoi(&pStr[14]);
	tmTime.tm_sec = atoi(&pStr[17]);
	return mktime(&tmTime);
}

//-------------------------------------------------------------------------
//日期设置
//-------------------------------------------------------------------------
void gui_Editor_DayCreate(char *pStr, int nDay, time_t tTime)
{
	u8 aTime[3];

	day4timet(tTime, nDay, aTime, 0);
	//转换为规格字符串
	sprintf(pStr, "%02d-%02d-%02d", aTime[2], aTime[1], aTime[0]);
}

void gui_Editor_DaySet(const char *pInfo, char *pStr, int nIndex, int nSelect)
{
	int x;

	//绘制提示信息
	gui_DrawString_Mixed_Center(0, GUI_EDIT_INFO_Y, pInfo, COLOR_BLACK);

	if (nIndex > 3)
		nIndex += 2;
	else if (nIndex > 1)
		nIndex += 1;
	pStr[nIndex] = gui_EditorCompnet_Number(pStr[nIndex], 1, 10, nSelect);

	//绘制数字及光标
	x = gui_DrawString_Mixed_Center(0, GUI_EDIT_DATA_Y, pStr, COLOR_BLACK) + ASC_WIDTH * nIndex;
	gui_DrawHLine(x, GUI_EDIT_DATA_Y + HZ_HEIGHT, x + (ASC_WIDTH - 1), COLOR_BLACK);
}

void gui_Editor_DayDone(char *pStr, u8 *pTime)
{
	int i;

	//整形
	pStr[2] = 0;
	pStr[5] = 0;

	//转换规格字符串为IP地址
	for (i = 0; i < 3; ++i)
	{
		pTime[i] = bin2bcd8(atoi(&pStr[i * 3]));
	}
	invert(pTime, 3);
}

//-------------------------------------------------------------------------
//数字设置
//-------------------------------------------------------------------------
void gui_Editor_NumberCreate(char *pStr, s64 nNum, int nBits, int nHex)
{
	char strNum[32];
	int nTemp;

	//转换数字为规格字符串
	memset(pStr, '0', nBits);
	pStr[nBits] = 0;
	if (nHex)
		sprintf(strNum, "%llX", nNum);
	else
		sprintf(strNum, "%lld", nNum);
	nTemp = strlen(strNum);
	if (nTemp > nBits)
		memcpy(&pStr[0], &strNum[nTemp - nBits], nBits);
	else
		memcpy(&pStr[nBits - nTemp], &strNum[0], nTemp); 
}

void gui_Editor_NumberSet(const char *pInfo, char *pStr, int nIndex, int nSelect, int nSign, int nHex)
{
	int x;

	//绘制提示信息
	gui_DrawString_Mixed_Center(0, GUI_EDIT_INFO_Y, pInfo, COLOR_BLACK);

	//有符号,第一位可选-号
	if ((nIndex == 0) && nSign)
		x = 0;
	else
		x = 1;
	if (nHex)
		nHex = 15;
	else
		nHex = 9;
	pStr[nIndex] = gui_EditorCompnet_Number(pStr[nIndex], x, x + nHex, nSelect);

	//绘制数字及光标
	x = gui_DrawString_Mixed_Center(0, GUI_EDIT_DATA_Y, pStr, COLOR_BLACK) + ASC_WIDTH * nIndex;
	gui_DrawHLine(x, GUI_EDIT_DATA_Y + HZ_HEIGHT, x + (ASC_WIDTH - 1), COLOR_BLACK);
}

s64 gui_Editor_NumberDone(char *pStr, int nHex)
{
	//转换规格字符串为数字
	s64 n;
	char **ppStr = NULL;
	
	if (nHex)
		nHex = 16;
	else
		nHex = 10;
	n = strtoll(pStr, ppStr, nHex);
	return n;
}


//-------------------------------------------------------------------------
//字符串设置
//-------------------------------------------------------------------------
#define GUI_CHAR_ENTER			0x7F
static const char tbl_guiEditorStr[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '.', '@', GUI_CHAR_ENTER,
};

static char gui_EditorCompnet_Char(char ch, int nSelect)
{
	int i;

	for (i = 0; i < ARR_SIZE(tbl_guiEditorStr); i++)
	{
		if (ch == tbl_guiEditorStr[i])
			break;
	}
	i = cycle(i, 0, ARR_SIZE(tbl_guiEditorStr) - 1, nSelect);
	
	return tbl_guiEditorStr[i];
}

void gui_Editor_StringSet(const char *pInfo, char *pStr, int nIndex, int nSelect)
{
	int x;

	//绘制提示信息
	gui_DrawString_Mixed_Center(0, GUI_EDIT_INFO_Y, pInfo, COLOR_BLACK);

	pStr[nIndex] = gui_EditorCompnet_Char(pStr[nIndex], nSelect);
	//绘制字串及光标
	x = gui_DrawString_Mixed_Center(0, GUI_EDIT_DATA_Y, pStr, COLOR_BLACK) + ASC_WIDTH * nIndex;
	gui_DrawHLine(x, GUI_EDIT_DATA_Y + HZ_HEIGHT, x + (ASC_WIDTH - 1), COLOR_BLACK);
}

void gui_Editor_StringDone(char *pStr)
{
	int i;

	for (i = 0; *pStr && (i < 64); pStr++)
	{
		if (*pStr == GUI_CHAR_ENTER)
		{
			*pStr = 0;
			break;
		}
	}
}
#endif

