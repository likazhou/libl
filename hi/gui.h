#ifndef __HI_GUI_H__
#define __HI_GUI_H__


#ifdef __cplusplus
extern "C" {
#endif






//Pubilc Defines
#define GUI_FONT_NULL				0
#define GUI_FONT_STD12				1
#define GUI_FONT_STD16				2

#define GUI_FONT_CARR_T_NULL		0
#define GUI_FONT_CARR_T_DIRACCESS	1
#define GUI_FONT_CARR_T_FILE		2
#define GUI_FONT_CARR_T_NAND		3
#define GUI_FONT_CARR_T_SPIF		4

#define GUI_LCD_T_NULL				0
#define GUI_LCD_T_160_UC1698		1
#define GUI_LCD_T_128_SBN6400		2
#define GUI_LCD_T_128_NT7538		3



//Public Typedef
typedef int							t_color;




//Include Header Files
#if GUI_LCD_TYPE == GUI_LCD_T_160_UC1698
#include <drivers/lcd160uc1698.h>
#elif GUI_LCD_TYPE == GUI_LCD_T_128_SBN6400
#include <drivers/lcd128sbn6400.h>
#elif GUI_LCD_TYPE == GUI_LCD_T_128_NT7538
#include <drivers/lcd128nt7538.h>
#endif

#include <hi/gui/gui_Basic.h>
#include <hi/gui/gui_Editor.h>
#include <hi/gui/gui_String.h>

#include <hi/gui/disp.h>

#ifdef __cplusplus
}
#endif

#endif


