#ifndef __LIB_BCD_H__
#define __LIB_BCD_H__



#ifdef __cplusplus
extern "C" {
#endif

u8 bcd2bin8(u8 x);
u8 bin2bcd8(u8 x);
u16 bin2bcd16(u16 x);
u16 bcd2bin16(u16 x);
u32 bin2bcd32(u32 x);
u32 bcd2bin32(u32 x);
u64 bin2bcd64(u64 x);
int isnotbcd(const void *pAdr, size_t nLen);


#ifdef __cplusplus
}
#endif

#endif
	
