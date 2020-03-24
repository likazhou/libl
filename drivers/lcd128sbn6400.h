#ifndef __LCD128SBN6400_H__
#define __LCD128SBN6400_H__


//·Ö±æÂÊ
#define LCD_X_MAX   				128
#define LCD_Y_MAX   				64

#define LCD_ADDR_SETUPTIME			1
#define LCD_DATA_SETIPTIME			5


//External Functions
void lcd_Init(void);
void lcd_Bgl(int nOnOff);
void lcd_DrawPoint(int x, int y, t_color color);
void lcd_ClearAll(t_color color);
void lcd_Refresh(void);
void lcd_Redraw(void);


#endif
