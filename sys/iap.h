#ifndef __SYS_IAP_H__
#define __SYS_IAP_H__



#ifdef __cplusplus
extern "C" {
#endif




//Public Defines
#define IAP_HEADER_SIZE			ALIGN4(sizeof(t_iap))

#define IAP_MAGICWORD_1			0xFF00FF00
#define IAP_MAGICWORD_2			0x00FF00FF


//Public Typedefs
typedef struct {
	u32	magic1;
	u32	magic2;
	adr_t		dest;
	u32	len;
	u16	id;
	u16	crc;
	u8		reserve[12];
} t_iap, *p_iap;




//External Functions


#ifdef __cplusplus
}
#endif

#endif

