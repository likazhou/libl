#ifndef __LIB_H__
#define __LIB_H__

#include <lib/bcd.h>
#include <lib/ecc.h>
#include <lib/mathlib.h>
#include <lib/timelib.h>



//-------------------------------------------------------------------------
//General External Functions
//-------------------------------------------------------------------------
#define GETBIT(n, bit)  		(((n) >> (bit)) & 1)

#define BITMASK(bit)			(1 << (bit))
#define BITANTI(bit)			(~(BITMASK(bit)))

#define SETBIT(n, bit)			((n) |= BITMASK(bit))
#define CLRBIT(n, bit)			((n) &= BITANTI(bit))

#define MAX(x, y)				(((x) < (y)) ? (y) : (x))
#define MIN(x, y)				(((x) < (y)) ? (x) : (y))

#define ABS(x, y)				(((x) < (y)) ? ((y) - (x)) : ((x) - (y)))

#define ALIGN2(n)				(((n) + 1) & ~1)
#define ALIGN4(n)				(((n) + 3) & ~3)

#define FPOS(type, field)		((size_t)&((type *)0)->field)
#define FSIZ(type, field)		(sizeof(((type *)0)->field))

#define ARR_SIZE(a)				(sizeof(a) / sizeof(a[0]))
#define ARR_ENDADR(a)			(&a[ARR_SIZE(a)])


typedef struct {
	u8	ip[4];
	u16	port;
} t_ip4_adr;

typedef union {
	u32	n;
	u8	byte[4];
} u_byte4;

typedef union {
	u16	n;
	u8	byte[2];
} u_byte2;

typedef union {
	u32	n;
	u16	word[2];
} u_word2;


//External Consts
#define STRING_0D0A				"\r\n"




u32 int_getbits(u32 nData, int nStartBit, int nBits);
u32 int_setbits(u32 nData, int nStartBit, int nBits, u32 nVal);

int getbit(const void *pData, int i);
void setbit(void *pData, int i);
void clrbit(void *pData, int i);
int bits1(u32 i);
u32 invert_bits(u32 nData, int nBits);
void invert(void *pData, size_t nLen);
void cpyinvert(void *dst, const void *src, size_t count);
void byteadd(void *pData, int nValue, size_t nLen);
int cycle(int n, int nStart, int nEnd, int nStepping);

int chartoi(char c);
void bcd2str32(u32 n, char *pc);
void int2str32(u32 n, char *pc);
void bcd2str16(u16 n, char *pc);
void bcd2str8(u8 n, char *pc);
int memtest(const void *s, const u8 c, int len);
int memcnt(const void *s, const u8 c, int len);
int memscmp(const char *cs, const char *ct);
char *memfind(const char *s1, int l1, const char *str);
char *saftercomma(const char *cs, size_t l1);
int	sscanu8(const char *src, const char *s, u8 *dest, size_t l);
char *buffstr(buf b, const char *str);
int buffbd(buf b, size_t max);
int rnsftoi(const char *s);




#endif


