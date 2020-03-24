#ifndef __LIB_ECC_H__
#define __LIB_ECC_H__


#ifdef __cplusplus
extern "C" {
#endif


	
	
	

	
	
	
u8 cs8(const void *data, size_t len);
u16 cs16(int cs, const void *data, size_t len);
u16 crc16(const void *data, size_t len);
int fcs16(int fcs, const void *data, size_t len);
u8 xor8(const void *data, size_t len);




#ifdef __cplusplus
}
#endif

#endif
 
