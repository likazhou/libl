#if GUI_ENABLE

#include <string.h>
#include <stdlib.h>



void gui_Init()
{

	//Lcd Initialize
	lcd_Init();
	
#if GUI_LCD_RESET_EN
	lcd_Reset();
	lcd_ClearAll(COLOR_WHITE);
#endif
}


//画线
void gui_DrawLine(int x0, int y0, int x1, int y1, t_color color)
{
	int t, distance;
	int xerr=0, yerr=0, delta_x, delta_y;
	int incx, incy;

	delta_x = x1 - x0;
	delta_y = y1 - y0;
	if (delta_x > 0) 
		incx = 1;
	else if(delta_x == 0)
		incx = 0;
	else
		incx = -1;
	
	if (delta_y > 0)
		incy = 1;
	else if (delta_y == 0)
		incy = 0;
	else
		incy = -1;

	delta_x = abs(delta_x);
	delta_y = abs(delta_y);
	if (delta_x > delta_y)
		distance=delta_x;
	else
		distance = delta_y;
	
	for (t = 0; t <= distance + 1; t++)
	{
		gui_DrawPoint(x0, y0, color);
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance)
		{
			xerr -= distance;
			x0 += incx;
		}
		if (yerr > distance)
		{
			yerr -= distance;
			y0 += incy;
		}
	}
}

//画横线
void gui_DrawHLine(int x0, int y0, int x1, t_color color)
{
	int i, j;

	if (x0 <= x1)
	{
		i = x0;
		j = x1;
	}
	else
	{
		i = x1;
		j = x0;
	}
	
	for (; i <= j; i++)
	{
		gui_DrawPoint(i, y0, color);
	}
}

//画竖线
void gui_DrawRLine(int x0, int y0, int y1, t_color color)
{
	int i, j;

	if (y0 <= y1)
	{
		i = y0;
		j = y1;
	}
	else
	{
		i = y1;
		j = y0;
	}
	
	for (; i <= j; i++)
	{
		gui_DrawPoint(x0, i, color);
	}
}

//画空心矩型
void gui_DrawRect_Hol(int x1, int y1, int x2, int y2, t_color color)
{
	
	gui_DrawHLine(x1, y1, x2, color);
	gui_DrawHLine(x1, y2, x2, color);
	gui_DrawRLine(x1, y1, y2, color);
	gui_DrawRLine(x2, y1, y2, color);
}


//画实心矩型
void gui_DrawRect_Fill(int x1, int y1, int x2, int y2, t_color color)
{
	int i,j;

	for (i = y1; i <= y2; i++)
	{
		for (j = x1; j <= x2; j++)
		{
			gui_DrawPoint(j, i, color);
		}
	}
}

//画黑白图标
void gui_DrawIcon_Mono(int x1, int y1, int x2, int y2, const u8 *p)
{
	int i, x, y;

	for (y = y1; y < y2; y++)
	{
		for (x = x1; x < x2; p++)
		{
			for (i = 128; (i && (x < x2)); i >>= 1, x++)
			{
				if (*p & i)
					gui_DrawPoint(x, y, COLOR_BLACK);
				else
					gui_DrawPoint(x, y, COLOR_WHITE);
			}
		}
	}
}

//画黑白bmp文件
#define GUI_BMP_OFFSET			0x3E
void gui_DrawBMP_Mono(const u8 *p, int nIsBMP)
{
	int i, j, k;

	if (nIsBMP)
		p += GUI_BMP_OFFSET;
	
	for (i = 0; i < LCD_Y_MAX; i++)
	{
		for (j = 0; j < (LCD_X_MAX / 8); j++, p++)
		{
			for (k = 0; k < 8; k++)
			{
				if (*p & (1 << k))
					gui_DrawPoint(j * 8 + 7 - k, LCD_Y_MAX - 1 - i, COLOR_BLACK);
			}
		}
	}
}

#endif

