#if GUI_LCD_TYPE == GUI_LCD_T_128_NT7538


//Private Defines
#define LCD_DELAY					50


//Private Variables
static u8 gui_aBuf[LCD_Y_MAX / 8][LCD_X_MAX];
static u8 gui_aOld[LCD_Y_MAX / 8][LCD_X_MAX];

//Private Macros
#define lcd_Rst(x)			sys_GpioSet(gpio_node(tbl_bspLcdCtrl, 0), (x))
#define lcd_Bgc(x)			sys_GpioSet(gpio_node(tbl_bspLcdCtrl, 1), (x))


//Private Functions

//向LCD写命令
static void lcd_WriteCmd(int nData)
{

	__raw_writeb(nData, GUI_LCD_ADR_CMD);
	sys_Delay(LCD_DELAY);
}

//向LCD写数据
static void lcd_WriteData(int nData)
{

	__raw_writeb(nData, GUI_LCD_ADR_DATA);
	sys_Delay(LCD_DELAY);
}


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
	lcd_Rst(0);
	sys_Delay(10000);
	lcd_Rst(1);
	sys_Delay(5000);
	//Initialize
	lcd_WriteCmd(0xAE); //Display OFF
	lcd_WriteCmd(0xA2); // 1/64 Duty 1/9 Bias
	lcd_WriteCmd(0xA0); //ADC select S0->S131(?????S1-S128)
	lcd_WriteCmd(0xC8); //com64 --> com1
	lcd_WriteCmd(0x24); //Rb/Ra
	lcd_WriteCmd(0x81); //Sets V0
	lcd_WriteCmd(0x30); //内部电位器调节对比度
	lcd_WriteCmd(0x2F); //voltage follower ON  regulator ON  booster ON
	lcd_WriteCmd(0xA6); //Normal Display (not invert dispplay)
	lcd_WriteCmd(0xA4); //Entire Display Disable    
	lcd_WriteCmd(0x40); //Set Display Start Line = com0
	lcd_WriteCmd(0xAF); //Display ON
}

void lcd_RatioSet(int nRatio)
{

	if (nRatio > 7)
		nRatio = 4;
	nRatio |= 0x20;
	lcd_WriteCmd(nRatio); //Rb/Ra
}

//背光控制
void lcd_Bgl(int nOnOff)
{

	lcd_Bgc(nOnOff);
}

//画点
void lcd_DrawPoint(int x, int y, t_color color)
{

	x &= LCD_X_MAX - 1;
	y &= LCD_Y_MAX - 1;
	color ?	SETBIT(gui_aBuf[y / 8][x], y & 7) :
			CLRBIT(gui_aBuf[y / 8][x], y & 7);
}

//清屏
void lcd_ClearAll(t_color color)
{

	if (color == COLOR_WHITE)
		memset(gui_aBuf, 0, sizeof(gui_aBuf));
	else
		memset(gui_aBuf, 0xFF, sizeof(gui_aBuf));
}

//重绘
void lcd_Redraw()
{
	int i;
	u8 *p;

	for (i = 0; i < (LCD_Y_MAX / 8); i++)
	{
		lcd_WriteCmd(0xB0 | i);	//Set Page Address
		lcd_WriteCmd(0x10);	//Set Column Address = 0
	    lcd_WriteCmd(0x00);	//Colum from 1 -> 129 auto add
		p = &gui_aBuf[i][0];
		while (p < &gui_aBuf[i][LCD_X_MAX])
		{
			lcd_WriteData(*p++);
		}
	}
}

//换写屏
void lcd_Refresh()
{
#if 0
	int i, j, k;

	if (lcd_nBusy)
		return;
	
	for (i = 0; i < (LCD_Y_MAX / 8); i++)
	{
		lcd_SetYaddr(LCD_LEFT_SCR, i);
		for (j = 0, k = LCD_X_MAX; j < (LCD_X_MAX >> 1); j++)
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
		for (j = (LCD_X_MAX >> 1), k = LCD_X_MAX; j < LCD_X_MAX; j++)
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
#endif
}


#endif

