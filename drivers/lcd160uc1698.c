#if GUI_LCD_TYPE == GUI_LCD_T_160_UC1698

//Private Defines
#define LCD_DELAY					3

//============================================================
//寄存器指令
//============================================================
#define UC1698_SetCAL				0x00	//set column address lsb
#define UC1698_SetCAM				0x10	//set column address msb
#define UC1698_SetTC				0x24	//set temp. compensation
#define UC1698_SetPC				0x28	//set power control
#define UC1698_SetAPC				0x30	//set adv. program control,**do not using.for testing.
#define UC1698_SetSLL				0x40	//set scroll line lsb
#define UC1698_SetSLM				0x50	//set scroll line msb
#define UC1698_SetRAL				0x60	//set row address lsb
#define UC1698_SetRAM				0x70	//set row address msb
#define UC1698_SetPM				0x81	//set v-bias potentiometer
#define UC1698_SetPDC				0x84	//set partial display control
#define UC1698_SetRAC				0x88	//set ram address control
#define UC1698_SetFL				0x90	//set fixed lines
#define UC1698_SetLR				0xa0	//set line rate
#define UC1698_SetAPO				0xa4	//set all-pixel on
#define UC1698_SetINV				0xa6	//set inverse display
#define UC1698_SetDE				0xa8	//set display enable
#define UC1698_SetMAP				0xc0	//set lcd maping control
#define UC1698_SetNIV				0xc8	//set n-line inversion
#define UC1698_SetCP				0xd0	//set color pattern
#define UC1698_SetCM				0xd4	//set color mode
#define UC1698_SetCS				0xd8	//set com scan funtion
#define UC1698_SetRST				0xe2	//set system reset
#define UC1698_SetNOP				0xe3	//no operation
#define UC1698_SetBR				0xe8	//set lcd bias ratio
#define UC1698_SetCEND				0xf1	//set com end
#define UC1698_SetDST				0xf2	//set partial display start
#define UC1698_SetDEND				0xf3	//set partial display end
#define UC1698_SetWC0				0xf4	//set window program starting column address
#define UC1698_SetWR0				0xf5	//set window program starting row address
#define UC1698_SetWC1				0xf6	//set window program ending column address
#define UC1698_SetWR1				0xf7	//set window program ending row address
#define UC1698_SetWMODE				0xf8	//set window program mode
#define UC1698_SetMTPC				0xb8	//set MTP operation control
#define UC1698_SetMTPWM				0xb9	//set MTP Write Mask

//============================================================
//指令参数
//============================================================
#define UC1698_Bias_5				0
#define UC1698_Bias_10				1
#define UC1698_Bias_11				2
#define UC1698_Bias_12				3

#define UC1698_LcdCapSmall			0
#define UC1698_LcdCapLarge			1
#define UC1698_VlcdInter			2
#define UC1698_VlcdExter			0

#define UC1698_TC00					0	//v-bias temp compensation coefficient -0.00%/co
#define UC1698_TC05					1	//v-bias temp compensation coefficient -0.05%/co
#define UC1698_TC15					2	//v-bias temp compensation coefficient -0.15%/co
#define UC1698_TC25					3	//v-bias temp compensation coefficient -0.25%/co

#if 0
#define LCD_ASSERT(EX)				if (!(EX)) {return;}
#else
#define LCD_ASSERT(EX)
#endif


//Private Variables
#if GUI_COLOR_SIZE == 2
static u8 gui_aBuf[LCD_Y_MAX][LCD_X_MAX / 8 + 1];
#else
static u8 gui_aBuf[LCD_Y_MAX][LCD_X_MAX / 2];
#endif


//Private Macros
#define lcd_Rst(x)			sys_GpioSet(gpio_node(tbl_bspLcdCtrl, 0), (x))
#define lcd_Bgc(x)			sys_GpioSet(gpio_node(tbl_bspLcdCtrl, 1), (x))



//-------------------------------------------------------------------
//Internal Functions
//-------------------------------------------------------------------
static const u8 uc1698_tblParam[] = {
	UC1698_SetRAC | 1,	//Set RAM Address Control	0x89
	UC1698_SetCP | 1, 	//rgb-rgb					0xD1
	UC1698_SetCM | 2, 	//16k color mode			0xD6
	UC1698_SetSLL,		//							0x40
	UC1698_SetSLM,		//							0x50
	UC1698_SetMAP | 4,	//MX & MY disable			0xC4
	UC1698_SetFL,		//FLT,FLB Set				0x90
	0,
	UC1698_SetNIV,		//n-line inversion			0xC8
	UC1698_SetCS | 2,	//com scan function			0xDA
	UC1698_SetCEND,		//Set COM End				0xF1
	159,
		
	UC1698_SetRAL | 0,	//0x60
	UC1698_SetRAM | 0,	//0x70
	UC1698_SetCAL | 5,	//0x05
	UC1698_SetCAM | 2,	//0x12

	UC1698_SetWC0,		//set window program starting column address		0xF4
	37,
	UC1698_SetWR0,		//Set Window Program Starting Row Address		0xF5
	0,
	UC1698_SetWC1,		//Set Window Program Ending Column Address		0xF6
	92,
	UC1698_SetWR1,		//Set Window Program Ending Row Address		0xF7
	159,

	UC1698_SetWMODE,	//Window Program Mode	0xF8
	UC1698_SetPC | UC1698_LcdCapSmall | UC1698_VlcdInter,	//power control set as internal power		0x2A
	//UC1698_SetBR | UC1698_Bias_10,	//Bias Ratio:0:5,1:10,2:11,3:12 	0xE9
	UC1698_SetTC | UC1698_TC05, //set temperate compensation as 0%		0x25
	//UC1698_SetPM,		//electronic potentionmeter	0x81
	//192,

	UC1698_SetDE | 5,	//display on,select on/off mode.Green Enhance mode disable
};

#if EPI_ENABLE
static void lcd_WriteCmd(int nData)
{

	__raw_writeb(nData, GUI_LCD_ADR_CMD);
	sys_Delay(LCD_DELAY);
}

static void lcd_WriteData(int nData)
{

	__raw_writeb(nData, GUI_LCD_ADR_DATA);
	sys_Delay(LCD_DELAY);
}
#else
static lcd_SpiSend(int nData)
{
	spi_t *p;

	p = spi_Open(GUI_LCD_COMID, OS_TMO_FOREVER);
	spi_Config(p, SPI_SCKIDLE_LOW, SPI_LATCH_1EDGE, 0);
#if SPI_SEL_ENABLE
	spi_CsSel(p, GUI_LCD_CSID);
#endif
	spi_Start(p);
	spi_SendChar(p, nData);
	spi_End(p);
	spi_Close(p);
}

static void lcd_WriteCmd(int nCmd)
{

	sys_GpioSet(tbl_bspLcdCtrl[0] + 2, 0);
	lcd_SpiSend(nCmd);
}

static void lcd_WriteData(int nData)
{

	sys_GpioSet(tbl_bspLcdCtrl[0] + 2, 1);
	
	lcd_SpiSend(nData);
}
#endif


// 对比度调整
static void lcd_SetBR(char nBr)
{
	static int lcd_nBr = UC1698_Bias_10;

	if ((nBr & 0x80) == 0)
		lcd_nBr = nBr;
	
	lcd_WriteCmd(UC1698_SetBR | (lcd_nBr & 0x03));
}

static void lcd_SetPM(u8 nPm)
{
	static int lcd_nPm = 176;

	if (nPm)
		lcd_nPm = nPm;
	
	lcd_WriteCmd(UC1698_SetPM);
	lcd_WriteCmd(lcd_nPm);
}


#if EPI_ENABLE
static void lcd_SetPara()
{
	int i;

	for (i = 0; i < ARR_SIZE(uc1698_tblParam); i++)
	{
		lcd_WriteCmd(uc1698_tblParam[i]);
	}
	lcd_SetBR(0x80);
	lcd_SetPM(0);
}
#endif



//-------------------------------------------------------------------
//External Functions
//-------------------------------------------------------------------
//LCD初始化
void lcd_Init()
{
	t_gpio_def *p;

	//Control GPIO Initialize
	for (p = tbl_bspLcdCtrl[0]; p < tbl_bspLcdCtrl[1]; p++)
		{
		sys_GpioConf(p);
	}
}

void lcd_Reset()
{
	//Reset
	lcd_Rst(0);//rst low
	sys_Delay(1000);
	lcd_Rst(1);//rst high
	//Initialize
	lcd_WriteCmd(UC1698_SetRST);		//reset by command
	os_thd_sleep(150);
}

void lcd_RatioSet(int nRatio)
{

}

//背光控制
void lcd_Bgl(int nOnOff)
{

	lcd_Bgc(nOnOff);
}

//清屏
void lcd_ClearAll(t_color color)
{

	if (color == COLOR_WHITE)
		memset(gui_aBuf, 0, sizeof(gui_aBuf));
	else
		memset(gui_aBuf, 0xFF, sizeof(gui_aBuf));
}

#if GUI_COLOR_SIZE == 2
//画点
void lcd_DrawPoint(int x, int y, t_color color)
{

	LCD_ASSERT(x < LCD_X_MAX);
	LCD_ASSERT(y < LCD_Y_MAX);
	
	color ?	SETBIT(gui_aBuf[y][x / 8], x & 7) :
			CLRBIT(gui_aBuf[y][x / 8], x & 7);
}
//取点
t_color lcd_GetPoint(int x, int y)
{

	LCD_ASSERT(x < LCD_X_MAX);
	LCD_ASSERT(y < LCD_Y_MAX);
	
	return GETBIT(gui_aBuf[y][x / 8], x & 7);
}

//重绘
const u16 uc1698_tbl_map[8] = {
	0x0000, 0xF800, 0x07E0, 0xFFE0, 0x001F, 0xF81F, 0x07FF, 0xFFFF,
};
#if EPI_ENABLE
void lcd_Redraw()
{
	int i, nBus;
	u32 *pData, *pEnd;
	u32 nData;

	lcd_SetPara();
	
	pData = (u32 *)&gui_aBuf[0][0];
	pEnd = (u32 *)ARR_ENDADR(gui_aBuf);
	for (; pData < pEnd; pData = (u32 *)((u8 *)pData + 3))
	{
		nData = *pData;
		for (i = 0; i < 24; i += 3, nData >>= 3)
		{
			nBus = uc1698_tbl_map[nData & 0x07];
			lcd_WriteData(nBus >> 8);
			lcd_WriteData(nBus);
		}
	}
}
#else
void lcd_Redraw()
{
	int i, nBus;
	u32 *pData, *pEnd;
	u32 nData;
	spi_t *p;

	p = spi_Open(GUI_LCD_COMID, OS_TMO_FOREVER);
	
	spi_Config(p, SPI_SCKIDLE_LOW, SPI_LATCH_1EDGE, 0);
#if SPI_SEL_ENABLE
	spi_CsSel(p, GUI_LCD_CSID);
#endif
	spi_Start(p);

	sys_GpioSet(tbl_bspLcdCtrl[0] + 2, 0);
	for (i = 0; i < ARR_SIZE(uc1698_tblParam); i++)
	{
		spi_SendChar(p, uc1698_tblParam[i]);
	}
	spi_SendChar(p, UC1698_SetBR | UC1698_Bias_10);
	spi_SendChar(p, UC1698_SetPM);
	spi_SendChar(p, 192);
	
	sys_GpioSet(tbl_bspLcdCtrl[0] + 2, 1);
	
	pData = (u32 *)&gui_aBuf[0][0];
	pEnd = (u32 *)ARR_ENDADR(gui_aBuf);
	for (; pData < pEnd; pData = (u32 *)((u8 *)pData + 3))
	{
		nData = *pData;
		for (i = 0; i < 24; i += 3, nData >>= 3)
		{
			nBus = uc1698_tbl_map[nData & 0x07];
			spi_SendChar(p, nBus >> 8);
			spi_SendChar(p, nBus);
		}
	}
	spi_End(p);
	
	spi_Close(p);
}
#endif
#else
//画点
void lcd_DrawPoint(int x, int y, t_color color)
{
	int nTemp;

	LCD_ASSERT(x < LCD_X_MAX);
	LCD_ASSERT(y < LCD_Y_MAX);
	
	nTemp = gui_aBuf[y][x / 2];
	
	if (x & 1)
		nTemp = (nTemp & 0xF0) | (color & 0x0F);
	else
		nTemp = (nTemp & 0x0F) | (color << 4);
	
	gui_aBuf[y][x / 2] = nTemp;
}
//重绘
void lcd_Redraw()
{
	int i, j;

	for (i = 0; i < LCD_Y_MAX; i++)
	{
		for (j = 0; j < LCD_X_MAX / 2; j++)
		{
			lcd_WriteData(gui_aBuf[i][j]);
		}
		lcd_WriteData(0);
	}
}
#endif

/*------------------------------------------------------------

//换写屏
void lcd_Refresh()
{
	int i, j, k;

	if (lcd_nBusy)
		return;
		
	for (i = 0; i < (GUI_Y_MAX / 8); i++) 
	{
		lcd_SetYaddr(LCD_LEFT_SCR, i);
		for (j = 0, k = GUI_X_MAX; j < (GUI_X_MAX >> 1); j++)
		{
			if (gui_aOld[i][j] != gui_aBuf[i][j])
			{
				gui_aOld[i][j] = gui_aBuf[i][j];
				if (k != j)
				{
					k = j;
					lcd_SetXaddr(LCD_LEFT_SCR, j);
				}
				if (gui_aBuf[i][j] == 0x3F)
					gui_aBuf[i][j] = 0x3F;
				lcd_WriteData(LCD_LEFT_SCR, gui_aBuf[i][j]);
				k += 1;
			}
		}
		lcd_SetYaddr(LCD_RIGHT_SCR, i);
		for (j = (GUI_X_MAX >> 1), k = GUI_X_MAX; j < GUI_X_MAX; j++)
		{
			if (gui_aOld[i][j] != gui_aBuf[i][j])
			{
				gui_aOld[i][j] = gui_aBuf[i][j];
				if (k != j)
				{
					k = j;
					lcd_SetXaddr(LCD_RIGHT_SCR, j);
				}
				if (gui_aBuf[i][j] == 0x3F)
					gui_aBuf[i][j] = 0x3F;
				lcd_WriteData(LCD_RIGHT_SCR, gui_aBuf[i][j]);
				k += 1;
			}
		}
	}
}
--------------------------------------------------------------------*/


#endif

