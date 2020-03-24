#ifndef __ARCH_TYPEDEF_H__
#define __ARCH_TYPEDEF_H__


#ifdef __cplusplus
extern "C" {
#endif










//Public TypeDefs
typedef signed char				s8;
typedef unsigned char			u8;
typedef short					s16;
typedef unsigned short			u16;
#if defined(__GNUC__) && (__GNUC__ > 4)    /* GNU GCC Compiler */
typedef long					s32;
typedef unsigned long			u32;
#else
typedef int						s32;
typedef unsigned int			u32;
#endif
typedef long long				s64;
typedef unsigned long long		u64;

typedef unsigned int			size_t;

typedef unsigned int			adr_t;



//InputOuput Defines
#define __raw_writeb(v,a)		(*(volatile u8 *)(a) = (v))
#define __raw_writew(v,a)		(*(volatile uint16_t *)(a) = (v))
#define __raw_writel(v,a)		(*(volatile u32 *)(a) = (v))

#define __raw_readb(a)			(*(volatile u8 *)(a))
#define __raw_readw(a)			(*(volatile uint16_t *)(a))
#define __raw_readl(a)			(*(volatile u32 *)(a))




#ifdef __cplusplus
}
#endif

#endif

