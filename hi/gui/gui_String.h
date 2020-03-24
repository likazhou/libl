#ifndef _GUI_STRING_H_
#define _GUI_STRING_H_


#ifdef __cplusplus
extern "C" {
#endif

	
	
#if GUI_FONT_TYPE == GUI_FONT_STD12
#define ASC_WIDTH  			6  			//字符宽度
#define ASC_HEIGHT  		12			//字符高度
#define HZ_WIDTH			12 			//汉字宽度
#define HZ_SPAN				1			//汉字间隔
#define HZ_HEIGHT			12			//12点汉字高度
#define HZ_SIZE				18  		//汉字大小为12X12点阵，合计18个字节
#elif GUI_FONT_TYPE == GUI_FONT_STD16
#define ASC_WIDTH  			6  			//字符宽度
#define ASC_HEIGHT  		16			//字符高度
#define HZ_WIDTH			16 			//汉字宽度
#define HZ_SPAN				0			//汉字间隔
#define HZ_HEIGHT			16			//16点汉字高度
#define HZ_SIZE				32  		//汉字大小为16X16点阵，合计32个字节
#endif

#define ASC6x8_SIZE			6  			//字符的大小为6X8点阵，合计6字节
#define ASC6x12_SIZE		12  		//字符的大小为6X12点阵，合计12字节
#define ASC6x16_SIZE		12  		//字符的大小为6X16点阵，合计12字节



//External Functions
void gui_DrawString_ASC6x8(int x, int y, const char *pStr, t_color nColor);
void gui_DrawString_ASC6x12(int x, int y, const char *pStr, t_color nColor);
void gui_DrawString_ASC6x16(int x, int y, const char *pStr, t_color nColor);
int gui_DrawString_Mixed(int x, int y, const char *pStr, t_color nColor);
int gui_DrawString_Mixed_Center(int x, int y, const char *pStr, t_color nColor);
int gui_DrawString_Mixed_Right(int y, const char *pStr, t_color nColor);


#ifdef __cplusplus
}
#endif


#endif

