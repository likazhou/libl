#include <ctype.h>
#include <stdarg.h>

#if LIB_MINILIBC_ENABLE

#define MINILIBC_TINY_SIZE


#ifndef RT_USING_MINILIBC
void *memset(void *s, int c, size_t count)
{
#ifdef MINILIBC_TINY_SIZE
    char *xs = (char *)s;

    while (count--)
        *xs++ = c;

    return s;
#else
#define LBLOCKSIZE      (sizeof(int))
#define UNALIGNED(X)    ((int)X & (LBLOCKSIZE - 1))
#define TOO_SMALL(LEN)  ((LEN) < LBLOCKSIZE)

    int i;
    char *m = (char *)s;
    unsigned int buffer;
    unsigned int *aligned_addr;
    unsigned int d = c & 0xff;

    if (!TOO_SMALL(count) && !UNALIGNED(s))
    {
        /* If we get this far, we know that n is large and m is word-aligned. */
        aligned_addr = (unsigned int *)s;

        /* Store D into each char sized location in BUFFER so that
         * we can set large blocks quickly.
         */
        if (LBLOCKSIZE == 4)
        {
            buffer = (d << 8) | d;
            buffer |= (buffer << 16);
        }
        else
        {
            buffer = 0;
            for (i = 0; i < LBLOCKSIZE; i ++)
                buffer = (buffer << 8) | d;
        }

        while (count >= LBLOCKSIZE * 4)
        {
            *aligned_addr++ = buffer;
            *aligned_addr++ = buffer;
            *aligned_addr++ = buffer;
            *aligned_addr++ = buffer;
            count -= 4 * LBLOCKSIZE;
        }

        while (count >= LBLOCKSIZE)
        {
            *aligned_addr++ = buffer;
            count -= LBLOCKSIZE;
        }

        /* Pick up the remainder with a bytewise loop. */
        m = (char *)aligned_addr;
    }

    while (count--)
    {
        *m++ = (char)d;
    }

    return s;

#undef LBLOCKSIZE
#undef UNALIGNED
#undef TOO_SMALL
#endif
}

/**
 * This function will copy memory content from source address to destination
 * address.
 *
 * @param dst the address of destination memory
 * @param src  the address of source memory
 * @param count the copied length
 *
 * @return the address of destination memory
 */
void *memcpy(void *dst, const void *src, size_t count)
{
#ifdef MINILIBC_TINY_SIZE
    char *tmp = (char *)dst, *s = (char *)src;

    while (count--)
        *tmp++ = *s++;

    return dst;
#else

#define UNALIGNED(X, Y)                                               \
                        (((int)X & (sizeof(int) - 1)) | \
                         ((int)Y & (sizeof(int) - 1)))
#define BIGBLOCKSIZE    (sizeof(int) << 2)
#define LITTLEBLOCKSIZE (sizeof(int))
#define TOO_SMALL(LEN)  ((LEN) < BIGBLOCKSIZE)

    char *dst_ptr = (char *)dst;
    char *src_ptr = (char *)src;
    int *aligned_dst;
    int *aligned_src;
    int len = count;

    /* If the size is small, or either SRC or DST is unaligned,
    then punt into the byte copy loop.  This should be rare. */
    if (!TOO_SMALL(len) && !UNALIGNED(src_ptr, dst_ptr))
    {
        aligned_dst = (int *)dst_ptr;
        aligned_src = (int *)src_ptr;

        /* Copy 4X long words at a time if possible. */
        while (len >= BIGBLOCKSIZE)
        {
            *aligned_dst++ = *aligned_src++;
            *aligned_dst++ = *aligned_src++;
            *aligned_dst++ = *aligned_src++;
            *aligned_dst++ = *aligned_src++;
            len -= BIGBLOCKSIZE;
        }

        /* Copy one long word at a time if possible. */
        while (len >= LITTLEBLOCKSIZE)
        {
            *aligned_dst++ = *aligned_src++;
            len -= LITTLEBLOCKSIZE;
        }

        /* Pick up any residual with a byte copier. */
        dst_ptr = (char *)aligned_dst;
        src_ptr = (char *)aligned_src;
    }

    while (len--)
        *dst_ptr++ = *src_ptr++;

    return dst;
#undef UNALIGNED
#undef BIGBLOCKSIZE
#undef LITTLEBLOCKSIZE
#undef TOO_SMALL
#endif
}

/**
 * This function will move memory content from source address to destination
 * address.
 *
 * @param dest the address of destination memory
 * @param src  the address of source memory
 * @param n the copied length
 *
 * @return the address of destination memory
 */
void *memmove(void *dest, const void *src, size_t n)
{
    char *tmp = (char *)dest, *s = (char *)src;

    if (s < tmp && tmp < s + n)
    {
        tmp += n;
        s += n;

        while (n--)
            *(--tmp) = *(--s);
    }
    else
    {
        while (n--)
            *tmp++ = *s++;
    }

    return dest;
}

/**
 * This function will compare two areas of memory
 *
 * @param cs one area of memory
 * @param ct znother area of memory
 * @param count the size of the area
 *
 * @return the result
 */
int memcmp(const void *cs, const void *ct, size_t count)
{
    const unsigned char *su1, *su2;
    int res = 0;

    for (su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--)
        if ((res = *su1 - *su2) != 0)
            break;

    return res;
}

/**
 * This function will return the length of a string, which terminate will
 * null character.
 *
 * @param s the string
 *
 * @return the length of string
 */
size_t strlen(const char *s)
{
    const char *sc;

    for (sc = s; *sc != '\0'; ++sc) /* nothing */
        ;

    return sc - s;
}

char *strcpy(char *dest, const char *src)
{
	return (char *)strncpy(dest, src, strlen(src) + 1);
}

char *strncpy(char *dst, const char *src, size_t n)
{
    if (n != 0)
    {
        char *d = dst;
        const char *s = src;

        do
        {
            if ((*d++ = *s++) == 0)
            {
                /* NUL pad the remaining n-1 bytes */
                while (--n != 0)
                    *d++ = 0;
                break;
            }
        } while (--n != 0);
    }

    return (dst);
}

size_t strlcpy(char *dst, const char *src, size_t siz)
{
	register char *d = dst;
	register const char *s = src;
	register size_t n = siz;

	/* Copy as many bytes as will fit */
	if (n != 0 && --n != 0)
	{
		do
		{
			if ((*d++ = *s++) == 0) break;
		} while (--n != 0);
	}

	/* Not enough room in dst, add NUL and traverse rest of src */
	if (n == 0)
	{
		if (siz != 0) *d = '\0';	/* NUL-terminate dst */
		while (*s++) ;
	}

	return(s - src - 1);			/* count does not include NUL */
}

int strcmp (const char *s1, const char *s2)
{
	while (*s1 && *s1 == *s2)
		s1++, s2++;
	return (*s1 - *s2);
}

/**
 * strncmp - Compare two length-limited strings
 * @cs: One string
 * @ct: Another string
 * @count: The maximum number of bytes to compare
 */
int strncmp(const char *cs,const char *ct, size_t count)
{
	register signed char __res = 0;

	while (count) {
		if ((__res = *cs - *ct++) != 0 || !*cs++)
			break;
		count--;
	}

	return __res;
}

char *strcat(char * dest, const char * src)
{
	char *tmp = dest;

	while (*dest)
		dest++;
	while ((*dest++ = *src++) != '\0')
		;

	return tmp;
}

char *strncat(char *dest, const char *src, size_t count)
{
	char *tmp = dest;

	if (count) {
		while (*dest)
			dest++;
		while ((*dest++ = *src++)) {
			if (--count == 0) {
				*dest = '\0';
				break;
			}
		}
	}

	return tmp;
}

char *strrchr(const char *t, int c) 
{
	register char ch;
	register const char *l=0;

	ch = c;
	for (;;) 
	{
		if (*t == ch) l=t; 
		if (!*t) return (char*)l; 
		++t;
	}
	
	return (char*)l;
}

int  strncasecmp ( const char* s1, const char* s2, size_t len )
{
	register unsigned int  x2;
	register unsigned int  x1;
	register const char*   end = s1 + len;

	while (1)
	{
		if ((s1 >= end) )
			return 0;

		x2 = *s2 - 'A'; if ((x2 < 26u)) x2 += 32;
		x1 = *s1 - 'A'; if ((x1 < 26u)) x1 += 32;
		s1++; s2++;

		if (x2 != x1)
			break;

		if (x1 == (unsigned int)-'A')
			break;
	}

	return x1 - x2;
}

/* private function */
#ifndef isdigit
#define isdigit(c)  ((unsigned)((c) - '0') < 10)
#endif

static __INLINE int divide(int *n, int base)
{
	int res;

	/* optimized for processor which does not support divide instructions. */
	if (base == 10)
	{
		res = ((int)*n) % 10U;
		*n = ((int)*n) / 10U;
	}
	else
	{
		res = ((int)*n) % 16U;
		*n = ((int)*n) / 16U;
	}

	return res;
}

static __INLINE int skip_atoi(const char **s)
{
	register int i=0;
	while (isdigit(**s)) i = i*10 + *((*s)++) - '0';

	return i;
}

/* use precision */
#define RT_PRINTF_PRECISION
//#define RT_PRINTF_LONGLONG
//#define RT_PRINTF_SPECIAL

#define ZEROPAD     (1 << 0)    /* pad with zero */
#define SIGN        (1 << 1)    /* unsigned/signed long */
#define PLUS        (1 << 2)    /* show plus */
#define SPACE       (1 << 3)    /* space if plus */
#define LEFT        (1 << 4)    /* left justified */
#define SPECIAL     (1 << 5)    /* 0x */
#define LARGE       (1 << 6)    /* use 'ABCDEF' instead of 'abcdef' */

#ifdef RT_PRINTF_PRECISION
static char *print_number(char *buf,
                          char *end,
                          long  num,
                          int   base,
                          int   s,
                          int   precision,
                          int   type)
#else
static char *print_number(char *buf,
                          char *end,
                          long  num,
                          int   base,
                          int   s,
                          int   type)
#endif
{
    char c, sign;
#ifdef RT_PRINTF_LONGLONG
    char tmp[32];
#else
    char tmp[16];
#endif
    const char *digits;
    static const char small_digits[] = "0123456789abcdef";
    static const char large_digits[] = "0123456789ABCDEF";
    register int i;
    register int size;

    size = s;

    digits = (type & LARGE) ? large_digits : small_digits;
    if (type & LEFT)
        type &= ~ZEROPAD;

    c = (type & ZEROPAD) ? '0' : ' ';

    /* get sign */
    sign = 0;
    if (type & SIGN)
    {
        if (num < 0)
        {
            sign = '-';
            num = -num;
        }
        else if (type & PLUS)
            sign = '+';
        else if (type & SPACE)
            sign = ' ';
    }

#ifdef RT_PRINTF_SPECIAL
    if (type & SPECIAL)
    {
        if (base == 16)
            size -= 2;
        else if (base == 8)
            size--;
    }
#endif

    i = 0;
    if (num == 0)
        tmp[i++]='0';
    else
    {
        while (num != 0)
            tmp[i++] = digits[divide(&num, base)];
    }

#ifdef RT_PRINTF_PRECISION
    if (i > precision)
        precision = i;
    size -= precision;
#else
    size -= i;
#endif

    if (!(type&(ZEROPAD | LEFT)))
    {
        if ((sign)&&(size>0))
            size--;

        while (size-->0)
        {
            if (buf <= end)
                *buf = ' ';
            ++ buf;
        }
    }

    if (sign)
    {
        if (buf <= end)
        {
            *buf = sign;
            -- size;
        }
        ++ buf;
    }

#ifdef RT_PRINTF_SPECIAL
    if (type & SPECIAL)
    {
        if (base==8)
        {
            if (buf <= end)
                *buf = '0';
            ++ buf;
        }
        else if (base == 16)
        {
            if (buf <= end)
                *buf = '0';
            ++ buf;
            if (buf <= end)
            {
                *buf = type & LARGE? 'X' : 'x';
            }
            ++ buf;
        }
    }
#endif

    /* no align to the left */
    if (!(type & LEFT))
    {
        while (size-- > 0)
        {
            if (buf <= end)
                *buf = c;
            ++ buf;
        }
    }

#ifdef RT_PRINTF_PRECISION
    while (i < precision--)
    {
        if (buf <= end)
            *buf = '0';
        ++ buf;
    }
#endif

    /* put number in the temporary buffer */
    while (i-- > 0)
    {
        if (buf <= end)
            *buf = tmp[i];
        ++ buf;
    }

    while (size-- > 0)
    {
        if (buf <= end)
            *buf = ' ';
        ++ buf;
    }

    return buf;
}

int vsnprintf(char       *buf,
                  size_t      size,
                  const char *fmt,
                  va_list     args)
{
#ifdef RT_PRINTF_LONGLONG
    unsigned long long num;
#else
    unsigned long num;
#endif
    int i, len;
    char *str, *end, c;
    const char *s;

    unsigned char base;            /* the base of number */
    unsigned char flags;           /* flags to print number */
    unsigned char qualifier;       /* 'h', 'l', or 'L' for integer fields */
    int field_width;     /* width of output field */

#ifdef RT_PRINTF_PRECISION
    int precision;      /* min. # of digits for integers and max for a string */
#endif

    str = buf;
    end = buf + size - 1;

    /* Make sure end is always >= buf */
    if (end < buf)
    {
        end  = ((char *)-1);
        size = end - buf;
    }

    for (; *fmt ; ++fmt)
    {
        if (*fmt != '%')
        {
            if (str <= end)
                *str = *fmt;
            ++ str;
            continue;
        }

        /* process flags */
        flags = 0;

        while (1)
        {
            /* skips the first '%' also */
            ++ fmt;
            if (*fmt == '-') flags |= LEFT;
            else if (*fmt == '+') flags |= PLUS;
            else if (*fmt == ' ') flags |= SPACE;
            else if (*fmt == '#') flags |= SPECIAL;
            else if (*fmt == '0') flags |= ZEROPAD;
            else break;
        }

        /* get field width */
        field_width = -1;
        if (isdigit(*fmt)) field_width = skip_atoi(&fmt);
        else if (*fmt == '*')
        {
            ++ fmt;
            /* it's the next argument */
            field_width = va_arg(args, int);
            if (field_width < 0)
            {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

#ifdef RT_PRINTF_PRECISION
        /* get the precision */
        precision = -1;
        if (*fmt == '.')
        {
            ++ fmt;
            if (isdigit(*fmt)) precision = skip_atoi(&fmt);
            else if (*fmt == '*')
            {
                ++ fmt;
                /* it's the next argument */
                precision = va_arg(args, int);
            }
            if (precision < 0) precision = 0;
        }
#endif
        /* get the conversion qualifier */
        qualifier = 0;
#ifdef RT_PRINTF_LONGLONG
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L')
#else
        if (*fmt == 'h' || *fmt == 'l')
#endif
        {
            qualifier = *fmt;
            ++ fmt;
#ifdef RT_PRINTF_LONGLONG
            if (qualifier == 'l' && *fmt == 'l')
            {
                qualifier = 'L';
                ++ fmt;
            }
#endif
        }

        /* the default base */
        base = 10;

        switch (*fmt)
        {
        case 'c':
            if (!(flags & LEFT))
            {
                while (--field_width > 0)
                {
                    if (str <= end) *str = ' ';
                    ++ str;
                }
            }

            /* get character */
            c = (unsigned char)va_arg(args, int);
            if (str <= end) *str = c;
            ++ str;

            /* put width */
            while (--field_width > 0)
            {
                if (str <= end) *str = ' ';
                ++ str;
            }
            continue;

        case 's':
            s = va_arg(args, char *);
            if (!s) s = "(NULL)";

            len = strlen(s);
#ifdef RT_PRINTF_PRECISION
            if (precision > 0 && len > precision) len = precision;
#endif

            if (!(flags & LEFT))
            {
                while (len < field_width--)
                {
                    if (str <= end) *str = ' ';
                    ++ str;
                }
            }

            for (i = 0; i < len; ++i)
            {
                if (str <= end) *str = *s;
                ++ str;
                ++ s;
            }

            while (len < field_width--)
            {
                if (str <= end) *str = ' ';
                ++ str;
            }
            continue;

        case 'p':
            if (field_width == -1)
            {
                field_width = sizeof(void *) << 1;
                flags |= ZEROPAD;
            }
#ifdef RT_PRINTF_PRECISION
            str = print_number(str, end,
                               (long)va_arg(args, void *),
                               16, field_width, precision, flags);
#else
            str = print_number(str, end,
                               (long)va_arg(args, void *),
                               16, field_width, flags);
#endif
            continue;

        case '%':
            if (str <= end) *str = '%';
            ++ str;
            continue;

            /* integer number formats - set up the flags and "break" */
        case 'o':
            base = 8;
            break;

        case 'X':
            flags |= LARGE;
        case 'x':
            base = 16;
            break;

        case 'd':
        case 'i':
            flags |= SIGN;
        case 'u':
            break;

        default:
            if (str <= end) *str = '%';
            ++ str;

            if (*fmt)
            {
                if (str <= end) *str = *fmt;
                ++ str;
            }
            else
            {
                -- fmt;
            }
            continue;
        }

#ifdef RT_PRINTF_LONGLONG
        if (qualifier == 'L') num = va_arg(args, long long);
        else if (qualifier == 'l')
#else
        if (qualifier == 'l')
#endif
        {
            num = va_arg(args, unsigned long);
            if (flags & SIGN) num = (long)num;
        }
        else if (qualifier == 'h')
        {
            num = (unsigned short)va_arg(args, long);
            if (flags & SIGN) num = (short)num;
        }
        else
        {
            num = va_arg(args, unsigned long);
            if (flags & SIGN) num = (long)num;
        }
#ifdef RT_PRINTF_PRECISION
        str = print_number(str, end, num, base, field_width, precision, flags);
#else
        str = print_number(str, end, num, base, field_width, flags);
#endif
    }

    if (str <= end) *str = '\0';
    else *end = '\0';

    /* the trailing null byte doesn't count towards the total
    * ++str;
    */
    return str - buf;
}

/**
 * This function will fill a formatted string to buffer
 *
 * @param buf the buffer to save formatted string
 * @param size the size of buffer
 * @param fmt the format
 */
int snprintf(char *buf, size_t size, const char *fmt, ...)
{
    int n;
    va_list args;

    va_start(args, fmt);
    n = vsnprintf(buf, size, fmt, args);
    va_end(args);

    return n;
}

/**
 * This function will fill a formatted string to buffer
 *
 * @param buf the buffer to save formatted string
 * @param arg_ptr the arg_ptr
 * @param format the format
 */
int vsprintf(char *buf, const char *format, va_list arg_ptr)
{
    return vsnprintf(buf, (size_t) -1, format, arg_ptr);
}

/**
 * This function will fill a formatted string to buffer
 *
 * @param buf the buffer to save formatted string
 * @param format the format
 */
int sprintf(char *buf, const char *format, ...)
{
    int n;
    va_list arg_ptr;

    va_start(arg_ptr, format);
    n = vsprintf(buf ,format, arg_ptr);
    va_end(arg_ptr);

    return n;
}
#endif

/**
 * simple_strtoul - convert a string to an unsigned long
 * @cp: The start of the string
 * @endp: A pointer to the end of the parsed string will be placed here
 * @base: The number base to use
 */
unsigned long simple_strtoul(const char *cp,char **endp,unsigned int base)
{
	unsigned long result = 0,value;

	if (!base) {
		base = 10;
		if (*cp == '0') {
			base = 8;
			cp++;
			if ((toupper(*cp) == 'X') && isxdigit(cp[1])) {
				cp++;
				base = 16;
			}
		}
	} else if (base == 16) {
		if (cp[0] == '0' && toupper(cp[1]) == 'X')
			cp += 2;
	}
	while (isxdigit(*cp) &&
	       (value = isdigit(*cp) ? *cp-'0' : toupper(*cp)-'A'+10) < base) {
		result = result*base + value;
		cp++;
	}
	if (endp)
		*endp = (char *)cp;
	return result;
}

/**
 * simple_strtol - convert a string to a signed long
 * @cp: The start of the string
 * @endp: A pointer to the end of the parsed string will be placed here
 * @base: The number base to use
 */
long simple_strtol(const char *cp,char **endp,unsigned int base)
{
	if(*cp=='-')
		return -simple_strtoul(cp+1,endp,base);
	return simple_strtoul(cp,endp,base);
}

/**
 * simple_strtoull - convert a string to an unsigned long long
 * @cp: The start of the string
 * @endp: A pointer to the end of the parsed string will be placed here
 * @base: The number base to use
 */
unsigned long long simple_strtoull(const char *cp,char **endp,unsigned int base)
{
	unsigned long long result = 0, value;

	if (*cp == '0') {
		cp++;
		if ((toupper(*cp) == 'X') && isxdigit (cp[1])) {
			base = 16;
			cp++;
		}
		if (!base) {
			base = 8;
		}
	}
	if (!base) {
		base = 10;
	}
	while (isxdigit (*cp) && (value = isdigit (*cp)
				? *cp - '0'
				: (islower (*cp) ? toupper (*cp) : *cp) - 'A' + 10) < base) {
		result = result * base + value;
		cp++;
	}
	if (endp)
		*endp = (char *) cp;
	return result;
}

/**
 * simple_strtoll - convert a string to a signed long long
 * @cp: The start of the string
 * @endp: A pointer to the end of the parsed string will be placed here
 * @base: The number base to use
 */
long long simple_strtoll(const char *cp,char **endp,unsigned int base)
{
	if(*cp=='-')
		return -simple_strtoull(cp+1,endp,base);
	return simple_strtoull(cp,endp,base);
}

size_t strspn(const char *s, const char *accept)
{
	size_t l=0;
	int a=1,i, al=strlen(accept);

	while((a)&&(*s))
	{
		for(a=i=0;(!a)&&(i<al);i++)
			if (*s==accept[i]) a=1;
		if (a) l++;
		s++;
	}
	return l;
}

size_t strcspn(const char *s, const char *reject)
{
	size_t l=0;
	int a=1,i,al=strlen(reject);

	while((a)&&(*s))
	{
		for(i=0;(a)&&(i<al);i++)
			if (*s==reject[i]) a=0;
		if (a) l++;
		s++;
	}
	return l;
}

char*strtok_r(char*s,const char*delim,char**ptrptr)
{
	char*tmp=0;

	if (s==0) s=*ptrptr;
	s += strspn(s,delim);	/* overread leading delimiter */
	if (*s)
	{
		tmp=s;
		s+=strcspn(s,delim);

		if (*s) *s++=0;		/* not the end ? => terminate it */
	}
	*ptrptr=s;
	return tmp;
}

char *strtok(char *s, const char *delim)
{
	static char *strtok_pos;
	return strtok_r(s,delim,&strtok_pos);
}

char *strchr(const char *s1, int i)
{
	const unsigned char *s = (const unsigned char *)s1;
	unsigned char c = (unsigned int)i;

	while (*s && *s != c)
	{
		s++;
	}

	if (*s != c)
	{
		s = NULL;
	}

	return (char *) s;
}

long strtol(const char *str, char **endptr, int base)
{
    return simple_strtol(str, endptr, base);
}

long long strtoll(const char *str, char **endptr, int base)
{
    return simple_strtoll(str, endptr, base);
}


int atoi(const char *s)
{
	return strtol(s, NULL, 10);
}

long int atol(const char *s)
{
	return strtol(s, NULL, 10);
}

#endif



int chartoi(char c)
{

	return isdigit((int)c) ? c - '0' : toupper(c) - 'A' + 10;
}



//-------------------------------------------------------------------------
//转换整数为字符串
//-------------------------------------------------------------------------
void bcd2str32(u32 n, char *pc)
{
	int i, nTemp;

	pc += 7;
	for (i = 0; i < 8; i++)
	{
		nTemp = n & 0x0F;
		if (nTemp <= 0x09)
			/* Add 0x30 to its ascii */
			*pc-- = nTemp + 0x30;
		else
			/* Add 0x37 to its ascii */
			*pc-- = nTemp + 0x37;
		n >>= 4;
	}
}

//-------------------------------------------------------------------------
//转换整数为字符串
//-------------------------------------------------------------------------
void int2str32(u32 n, char *pc)
{

	bcd2str32(bin2bcd32(n), pc);
}

//-------------------------------------------------------------------------
//转换整数为字符串
//-------------------------------------------------------------------------
void bcd2str16(u16 n, char *pc)
{
	int i, nTemp;

	pc += 3;
	for (i = 0; i < 4; i++)
	{
		nTemp = n & 0x0F;
		if (nTemp <= 0x09)
			/* Add 0x30 to its ascii */
			*pc-- = nTemp + 0x30;
		else
			/* Add 0x37 to its ascii */
			*pc-- = nTemp + 0x37;
		n >>= 4;
	}
}

//-------------------------------------------------------------------------
//转换整数为字符串
//-------------------------------------------------------------------------
void bcd2str8(u8 n, char *pc)
{
	int i, nTemp;

	pc += 1;
	for (i = 0; i < 2; i++)
	{
		nTemp = n & 0x0F;
		if (nTemp <= 0x09)
			/* Add 0x30 to its ascii */
			*pc-- = nTemp + 0x30;
		else
			/* Add 0x37 to its ascii */
			*pc-- = nTemp + 0x37;
		n >>= 4;
	}
}


//-------------------------------------------------------------------------
//copy并返回结束指针
//-------------------------------------------------------------------------
void *ptrcpy(void *dst, const void *src, size_t count)
{
	char *tmp = dst;

	memcpy(tmp, src, count);
	
	return tmp + count;
}


//-------------------------------------------------------------------------
//字符串是否全为某值
//-------------------------------------------------------------------------
int memtest(const void *s, const u8 c, int len)
{
	unsigned int i;
	u8 *p = (u8 *)s;

	for (i = len; i; i--)
	{
		if (*p++ != c)
			break;
	}
	return i;
}

//-------------------------------------------------------------------------
//字符串为某值的个数
//-------------------------------------------------------------------------
int memcnt(const void *s, const u8 c, int len)
{
	int i = 0;
	u8 *p, *pEnd;

	p = (u8 *)s;
	pEnd = p + len;
	for (; p < pEnd; )
	{
		if (*p++ == c)
			i += 1;
	}
	return i;
}

int memscmp(const char *cs, const char *ct)
{

	return memcmp(cs, ct, strlen(ct));
}


char *memfind(const char *s1, int l1, const char *str)
{
	int l2;

	l2 = strlen(str);
	if (!l2)
		return (char *)s1;
	
	while (l1 >= l2)
	{
		l1--;
		
		if (!memcmp(s1, str, l2))
			return (char *)(s1 + l2);
		
		s1++;
	}
	return NULL;
}

char *saftercomma(const char *cs, size_t l1)
{
	
 	while (*cs)
	{
		if (l1 == 0)
			return (char *)cs;

		if (*cs++ == ',')
			l1--;
	}

	return NULL;
}

int	sscanu8(const char *src, const char *s, u8 *dest, size_t l)
{
	int i;

	for (i = 0; i < l; i++)
	{
		dest[i] = atoi(src);
		src = memfind(src, strlen(src), s);
		if (src == NULL)
			break;
	}

	return i;
}


char *buffstr(buf b, const char *str)
{

	return memfind((const char *)b->p, b->len, str);
}


int buffbd(buf b, size_t max)
{
	int len;
	const char *sp, *ep, *tmp;

	sp = (const char *)b->p;
	ep = sp + b->len;

	for (; (ep - sp) > 6; sp++)
	{
		if (*sp == '$')
		{
			buf_Remove(b, (u8 *)sp - b->p);
			
			tmp = buffstr(b, STRING_0D0A);
			if (tmp == NULL)
			{
				if (b->len < max)
					break;
				else
					continue;
			}

			if (*(tmp - 5) != '*')
				continue;

			len = (u8 *)tmp - b->p;

			if (strtol(tmp - 4, NULL, 16) == xor8(&b->p[1], len - 6))
				return len;
		}
	}

	buf_Remove(b, (u8 *)sp - b->p);
	return 0;
}


int rnsftoi(const char *s)
{
	int i;
	char str[16];
	
	for (i = 0; i < sizeof(str); s++)
	{
		if (isdigit(*s))
		{
			str[i++] = *s;
		}
		else
		{
			if (*s != '.')
				break;
		}
	}
	str[i] = 0;

	return atoi(str);
}



