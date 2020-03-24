#include <stdarg.h>


#if 0
//这个用的时候必须要带参数，不然报错。。
#define PDEBUG(fmt, args...) fprintf(stderr, fmt, args##)

//这个可带可不带。。
#define LOGSTRINGS(fm, ...) printf(fm, ##__VA_ARGS__)



#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))
#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000
static int ITM_Sendchar(int ch)
{
	if (DEMCR & TRCENA)
	{
		while (ITM_Port32(0) == 0);
		ITM_Port8(0) = ch;  
	}

	return ch;
}
#endif




//External Functions
//void dbg_printf(const char *fmt, ...)
//{
//	va_list args;
//	char str[DBG_BUF_SIZE];

//	va_start(args, fmt);
//#if 0
//	vsnprintf(str, sizeof(str), fmt, args);
//#else
//	vsprintf(str, fmt, args);
//#endif
//	va_end(args);

//	dbg_trace(str);
//}





