#ifndef __LCD128NT7538_H__
#define __LCD128NT7538_H__



#ifdef __cplusplus
extern "C" {
#endif



//Public Defines
#define LCD_X_MAX   				128
#define LCD_Y_MAX   				64



//External Functions
void lcd_Init(void);
void lcd_RatioSet(int nRatio);
void lcd_Bgl(int nOnOff);
void lcd_SetBR(char nBr);
void lcd_SetPM(u8 nPm);
void lcd_DrawPoint(int x, int y, t_color color);
t_color lcd_GetPoint(int x, int y);
void lcd_ClearAll(t_color color);
void lcd_Refresh(void);
void lcd_Redraw(void);



#ifdef __cplusplus
}
#endif

#endif


