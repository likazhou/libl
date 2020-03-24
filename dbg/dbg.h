#ifndef __DBG_H__
#define __DBG_H__

#ifdef __cplusplus
extern "C" {
#endif


//External Defines
#define DBG_BUF_SIZE			200

#define LOG_T_STRING			0
#define LOG_T_GW3762T			1
#define LOG_T_GW3762R			2






//External Functions
void dbg_trace(const char *str);
void dbg_printf(const char *fmt, ...);

void log_Write(int nType, const void *pData, size_t nLen);
int log_Read(buf b);
void log_Read2Start(void);
void log_Read2End(void);
void log_Read2Day0(void);


#ifdef __cplusplus
}
#endif

#endif


