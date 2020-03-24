#ifndef __ARCH_TYPEDEF_H__
#define __ARCH_TYPEDEF_H__


#ifdef __cplusplus
extern "C" {
#endif










//Type Defines
typedef signed char				s8;
typedef unsigned char			u8;
typedef short					s16;
typedef unsigned short			u16;
typedef long					s32;
typedef unsigned long			u32;
typedef long long				s64;
typedef unsigned long long		u64;

typedef unsigned int			size_t;

typedef unsigned int			adr_t;



//InputOuput Defines
#define __raw_writeb(v,a)		(*(volatile uint8_t *)(a) = (v))
#define __raw_writew(v,a)		(*(volatile uint16_t *)(a) = (v))
#define __raw_writel(v,a)		(*(volatile uint32_t *)(a) = (v))

#define __raw_readb(a)			(*(volatile uint8_t *)(a))
#define __raw_readw(a)			(*(volatile uint16_t *)(a))
#define __raw_readl(a)			(*(volatile uint32_t *)(a))




#ifdef __cplusplus
}
#endif

#endif

