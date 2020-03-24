#if GUI_ENABLE
#include <hi/gui.h>

#if GUI_LCD_TYPE == GUI_LCD_T_160_UC1698
#include <drivers/lcd160uc1698.c>
#elif GUI_LCD_TYPE == GUI_LCD_T_128_SBN6400
#include <drivers/lcd128sbn6400.c>
#elif GUI_LCD_TYPE == GUI_LCD_T_128_NT7538
#include <drivers/lcd128nt7538.c>
#endif

#include <hi/gui/gui_font.c>
#include <hi/gui/gui_basic.c>
#include <hi/gui/gui_editor.c>
#include <hi/gui/gui_string.c>

#include <hi/gui/disp.c>

#endif


