#ifndef __SYS_I2C_H__
#define __SYS_I2C_H__


#ifdef __cplusplus
extern "C" {
#endif




//Public Defines
#define I2C_DQUE_RX_CHL		0x40	// not 0
#define I2C_DQUE_TX_CHL		0x50

#define I2C_S_IDLE			0

#define I2C_S_W_SB			1
#define I2C_S_W_ADR			2
#define I2C_S_W_TXE			3
#define I2C_S_W_STOP		4
#define I2C_S_W_SUCCESS		5

#define I2C_S_R_SB			9
#define I2C_S_R_ADR			10
#define I2C_S_RXNE			11

#define I2C_S_BTF			16

#define I2C_ADR_7BIT		0
#define I2C_ADR_10BIT		1



//Public Typedefs
typedef const struct
{
	u16	sclport : 3,
		sclpin : 4,
		sdaport : 3,
		sdapin : 4,
		adrmode : 2;
	u32	speed;
} t_i2c_def, *p_i2c_def;

typedef struct
{
	struct dev	parent;
#if I2C_SOFTWARE == 0
	t_i2c_def  *def;
#endif
	u8		ste;
	u8		adr;
#if I2C_SOFTWARE == 0
	u16		wlen;
	u16		rlen;
#endif
} i2c_t;





//External Functions
i2c_t *i2c_Open(int nId, size_t nTmo);
sys_res i2c_Close(i2c_t *p);
sys_res i2c_Write(i2c_t *p, int nDev, const void *pData, size_t nLen);
sys_res i2c_Read(i2c_t *p, int nDev, void *pData, size_t nLen);
sys_res i2c_WriteByte(i2c_t *p, int nDev, int nAdr, int nData);
sys_res i2c_WriteAdr(i2c_t *p, int nDev, int nAdr);




#ifdef __cplusplus
}
#endif

#endif


