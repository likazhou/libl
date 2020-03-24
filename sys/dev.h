#ifndef __SYS_DEV_H__
#define __SYS_DEV_H__



#ifdef __cplusplus
extern "C" {
#endif


//Public Defines
#define DEV_IDLECNT_ENABLE		0

#define DEV_S_IDLE				0
#define DEV_S_READY				1
#define DEV_S_BUSY				2

#define DEV_T_UART				0
#define DEV_T_I2C				1
#define DEV_T_SPI				2

#define DEV_PIN_OD				0
#define DEV_PIN_PP				1



struct dev
{
	u8	ste;
	u8	id;
#if DEV_IDLECNT_ENABLE
	u8	idlecnt;
#endif
//	u8	type;	//reserve
};




//External Functions
sys_res dev_Open(struct dev *p, size_t nTmo);
sys_res dev_Close(struct dev *p);
int dev_IsOpened(struct dev *p);


#ifdef __cplusplus
}
#endif

#endif

